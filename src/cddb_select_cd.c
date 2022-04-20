
#include "cddb_select_cd.h"
#include "debug.h"


#define new_column(n,c,...) gtk_tree_view_column_new_with_attributes(n,c,__VA_ARGS__)

static cddb_popup *instance = NULL;

/* The column names and column count for the list */
enum {
    CDDB_ARTIST,
    CDDB_TITLE,
    CDDB_GENRE,
    CDDB_DISCID,
    CDDB_DISCID_INT,
    CDDB_NUM
};

static char *CDDB_COL[] = { "Artist", "Title", "Genre", "Disc ID" };


/*
 * Some extra curly braces were added to the body of this function for
 * easy identification of parent/child relationships amongst the
 * widgets.
 */
cddb_popup *cddb_popup_create(GtkWindow * parent)
{
    if (!instance) {
        GtkWidget *w;

        instance = g_malloc(sizeof(cddb_popup));

        /* main popup window */
        w = gtk_dialog_new_with_buttons("CD selection",
                                        parent,
                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_STOCK_OK,
                                        GTK_RESPONSE_OK,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
        instance->window = w;

        {
            /* the list with CDs */
            GtkListStore *model;
            GtkTreeView *list;

            /* The model contains one extra column with the disc ID in
             * an integer instead of a string.  This column will not
             * be rendered by the view.  It will be used for
             * retrieving the disc ID of the selected entry as
             * integer afterwards.
             */
            model = gtk_list_store_new(CDDB_NUM, G_TYPE_STRING, G_TYPE_STRING,
                                       G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
            list = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(model)));
            g_object_unref(G_OBJECT(model));    /* list now has a ref */
            gtk_widget_show(GTK_WIDGET(list));
            gtk_box_pack_start(GTK_BOX(GTK_DIALOG(w)->vbox), GTK_WIDGET(list), TRUE, TRUE, 0);

            {
                /* the columns */
                GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
                GtkTreeViewColumn *column;

                /* the artist column */
                column = new_column(CDDB_COL[CDDB_ARTIST], renderer, "text", CDDB_ARTIST, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

                /* the value column */
                column = new_column(CDDB_COL[CDDB_TITLE], renderer, "text", CDDB_TITLE, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

                /* the description column */
                column = new_column(CDDB_COL[CDDB_GENRE], renderer, "text", CDDB_GENRE, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

                /* the disc ID column */
                column = new_column(CDDB_COL[CDDB_DISCID], renderer, "text", CDDB_DISCID, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
            }
            instance->list = list;
        }
    }
    return instance;
}

void cddb_popup_clear(cddb_popup * popup)
{
    GtkListStore *store;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(popup->list));
    gtk_list_store_clear(store);
}

void cddb_popup_insert(cddb_popup * popup, const char *artist, const char *title,
                       const char *genre, const glong discid)
{
    GtkListStore *store;
    GtkTreeIter iter;
    gchar buf[9];

    g_snprintf(buf, sizeof(buf), "%08x", discid);
    store = GTK_LIST_STORE(gtk_tree_view_get_model(popup->list));
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       CDDB_ARTIST, artist,
                       CDDB_TITLE, title,
                       CDDB_GENRE, genre,
                       CDDB_DISCID, buf,
                       CDDB_DISCID_INT, discid,
                       -1);
}

gboolean cddb_popup_query_user(cddb_popup * popup)
{
    gint response;
    GtkTreeSelection *selection;
    GtkTreeModel *model;

    gtk_widget_show(GTK_WIDGET(popup->window));
    response = gtk_dialog_run(GTK_DIALOG(popup->window));
    gtk_widget_hide(GTK_WIDGET(popup->window));
    if (response == GTK_RESPONSE_OK) {
        selection = gtk_tree_view_get_selection(popup->list);
        return gtk_tree_selection_get_selected(selection, &model, &(popup->iter));
    } else {
        /* cancelled */
        return FALSE;
    }
}

gchar *cddb_popup_get_genre(cddb_popup * popup)
{
    GtkTreeModel *model;
    gchar *genre;

    model = GTK_TREE_MODEL(gtk_tree_view_get_model(popup->list));
    gtk_tree_model_get(model, &(popup->iter), CDDB_GENRE, &genre, -1);
    return genre;
}

glong cddb_popup_get_discid(cddb_popup * popup)
{
    GtkTreeModel *model;
    glong discid;

    model = GTK_TREE_MODEL(gtk_tree_view_get_model(popup->list));
    gtk_tree_model_get(model, &(popup->iter), CDDB_DISCID_INT, &discid, -1);
    return discid;
}
