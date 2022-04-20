#include <gtk/gtk.h>
#include <gtk/gtkvbox.h>
#include <stdlib.h>
#include <string.h>
#include <glib-object.h>
#include "rx_track_view.h"
#include "track_view.h"
#include "ripperx.h"
#include "debug.h"
#include "id3_genre.h"

static GtkVBoxClass *parent_class = NULL;

static void rx_track_view_destroy(GtkObject * object);
static void rx_track_view_class_init(RxTrackViewClass * klass);
static void rx_track_view_init(RxTrackView * tv);

static void album_changed(GtkWidget * widget, gpointer data);
static void artist_changed(GtkWidget * widget, gpointer data);
static void track_name_changed(GtkWidget * widget, gpointer data);
static void track_selected(GtkWidget * widget, gpointer data);
static void select_all_toggled(GtkToggleButton * togglebutton, gpointer user_data);
static void genre_changed(GtkWidget *combo, gpointer user_data);

guint rx_track_view_get_type(void)
{
    static guint tv_type = 0;

    if (!tv_type) {
        static const GtkTypeInfo tv_info = {
            "RxTrackView",
            sizeof(RxTrackView),
            sizeof(RxTrackViewClass),
            (GtkClassInitFunc) rx_track_view_class_init,
            (GtkObjectInitFunc) rx_track_view_init,
            /* reserved_1 */ NULL,
            /* reserved_2 */ NULL,
            (GtkClassInitFunc) NULL,
        };

        tv_type = gtk_type_unique(gtk_vbox_get_type(), &tv_info);
    }

    return tv_type;
}

static void rx_track_view_class_init(RxTrackViewClass * klass)
{
    GtkObjectClass *object_class;

    object_class = (GtkObjectClass *) klass;
    parent_class = gtk_type_class(GTK_TYPE_VBOX);
    object_class->destroy = rx_track_view_destroy;
}

static void rx_track_view_init(RxTrackView * tv)
{
    GTK_WIDGET_SET_FLAGS(GTK_WIDGET(tv), GTK_CAN_FOCUS);
}

GtkWidget *rx_track_view_new(tv_model * model)
{
    RxTrackView *tv;
    GList *genres;
    int len;
    int t;

    dlog("RxTrackView creation...");
    tv = gtk_type_new(rx_track_view_get_type());

    tv->model = model;
    tv->header_box = gtk_hbox_new(FALSE, 3);
    tv->select_all = gtk_toggle_button_new_with_label("Select All Tracks");
    gtk_box_pack_start(GTK_BOX(tv->header_box), tv->select_all, FALSE, FALSE, 5);
    g_signal_connect(G_OBJECT(tv->select_all), "toggled", G_CALLBACK(select_all_toggled), model);
    tv->artist_lb = gtk_label_new("Artist");
    gtk_box_pack_start(GTK_BOX(tv->header_box), tv->artist_lb, FALSE, FALSE, 5);
    tv->artist_tb = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(tv->artist_tb), tv->model->artist);
    tv->artist_h = g_signal_connect(G_OBJECT(tv->artist_tb), "changed",
                                    G_CALLBACK(artist_changed), model);
    gtk_box_pack_start(GTK_BOX(tv->header_box), tv->artist_tb, FALSE, FALSE, 5);
    tv->album_lb = gtk_label_new("Album");
    gtk_box_pack_start(GTK_BOX(tv->header_box), tv->album_lb, FALSE, FALSE, 5);
    tv->album_tb = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(tv->album_tb), tv->model->album);
    tv->album_h = g_signal_connect(G_OBJECT(tv->album_tb), "changed",
                                   G_CALLBACK(album_changed), model);
    gtk_box_pack_start(GTK_BOX(tv->header_box), tv->album_tb, FALSE, FALSE, 5);
    tv->genre_lb = gtk_label_new("Genre");
    gtk_box_pack_start(GTK_BOX(tv->header_box), tv->genre_lb, FALSE, FALSE, 5);
    tv->genre_cb = gtk_option_menu_new();
    tv->menu = gtk_menu_new();
    genres = id3_get_list();
    gtk_option_menu_set_menu(GTK_OPTION_MENU(tv->genre_cb), GTK_MENU_SHELL(tv->menu));
    len = g_list_length(genres);
    for (t = 0; t < len; t++) {
        GtkWidget *mi;
        char *label;
        
        label = g_list_nth_data(genres, t);
        mi = gtk_menu_item_new_with_label(label);
        gtk_menu_append(GTK_MENU_SHELL(tv->menu), GTK_MENU_ITEM(mi));
    }
    gtk_option_menu_set_history(tv->genre_cb, 0);
    tv->genre_h = g_signal_connect(G_OBJECT(tv->genre_cb), "changed",
                                   G_CALLBACK(genre_changed), model);
    gtk_box_pack_start(GTK_BOX(tv->header_box), tv->genre_cb, FALSE, FALSE, 5);

    gtk_box_pack_start(GTK_BOX(tv), tv->header_box, FALSE, FALSE, 5);

    tv->scrollpane = gtk_scrolled_window_new(NULL, NULL);
    tv->track_vbox = gtk_vbox_new(TRUE, 5);

    for (t = 0; t < tv->model->tracks; t++) {
        tv_track track = tv->model->track[t];

        tv->track_hbox[t] = gtk_hbox_new(FALSE, 5);
        tv->play_b[t] = gtk_button_new();
        gtk_box_pack_start(GTK_BOX(tv->track_hbox[t]), tv->play_b[t], FALSE, FALSE, 5);
        tv->tracknum[t] = gtk_label_new(track.tracknum);
        gtk_box_pack_start(GTK_BOX(tv->track_hbox[t]), tv->tracknum[t], FALSE, FALSE, 5);
        tv->tracklen[t] = gtk_label_new(track.length);
        gtk_box_pack_start(GTK_BOX(tv->track_hbox[t]), tv->tracklen[t], FALSE, FALSE, 5);
        tv->tracksel[t] = gtk_check_button_new();
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tv->tracksel[t]), track.include);
        tv->tracksel_h[t] = g_signal_connect(G_OBJECT(tv->tracksel[t]),
                                             "toggled", G_CALLBACK(track_selected),
                                             &(tv->model->track[t]));
        gtk_box_pack_start(GTK_BOX(tv->track_hbox[t]), tv->tracksel[t], FALSE, FALSE, 5);
        tv->trackname[t] = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(tv->trackname[t]), track.name);
        tv->trackname_h[t] = g_signal_connect(G_OBJECT(tv->trackname[t]), "changed",
                                              G_CALLBACK(track_name_changed),
                                              &(tv->model->track[t]));
        gtk_box_pack_start(GTK_BOX(tv->track_hbox[t]), tv->trackname[t], TRUE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(tv->track_vbox), tv->track_hbox[t], TRUE, TRUE, 5);
    }
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(tv->scrollpane), tv->track_vbox);
    gtk_box_pack_start(GTK_BOX(tv), tv->scrollpane, TRUE, TRUE, 5);

    g_object_ref(G_OBJECT(tv));
    gtk_object_sink(GTK_OBJECT(tv));

    gtk_widget_show_all(GTK_WIDGET(tv));
    dlog("complete\n");

    return GTK_WIDGET(tv);
}

static void rx_track_view_destroy(GtkObject * object)
{
    int i;
    RxTrackView *tv;

    g_return_if_fail(object != NULL);
    g_return_if_fail(RX_IS_TRACK_VIEW(object));
    dlog("RxTrackView destroying...");

    tv = RX_TRACK_VIEW(object);

    g_return_if_fail(RX_IS_TRACK_VIEW(tv));
    g_return_if_fail(tv->model != NULL);
    dlog("tv->model :%p", tv->model);

//    gdk_threads_enter();
    for (i = 0; i < tv->model->tracks; i++) {
        gtk_widget_destroy(tv->trackname[i]);
        gtk_widget_destroy(tv->tracksel[i]);
        gtk_widget_destroy(tv->tracklen[i]);
        gtk_widget_destroy(tv->tracknum[i]);
        gtk_widget_destroy(tv->play_b[i]);
        gtk_widget_destroy(tv->track_hbox[i]);
    }
    gtk_widget_destroy(tv->track_vbox);
    gtk_widget_destroy(tv->scrollpane);
    gtk_widget_destroy(tv->album_lb);
    gtk_widget_destroy(tv->album_tb);
    gtk_widget_destroy(tv->artist_lb);
    gtk_widget_destroy(tv->artist_tb);
    gtk_widget_destroy(tv->header_box);
    g_free(tv->model);
//    gdk_threads_leave();

    /* Chain up */
    if (GTK_OBJECT_CLASS(parent_class)->destroy)
        (*GTK_OBJECT_CLASS(parent_class)->destroy) (object);

    dlog("destroyed\n");
    tv = NULL;
}

static void album_changed(GtkWidget * widget, gpointer data)
{
    gchar *tmp;
    tv_model *model;

    model = (tv_model *) data;
//    gdk_threads_enter();
    tmp = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);
    strncpy(model->album, tmp, MAX_NAME);
    g_free(tmp);
//    gdk_threads_leave();
    update_album(model->album);
}

static void artist_changed(GtkWidget * widget, gpointer data)
{
    gchar *tmp;
    tv_model *model;

    model = (tv_model *) data;
//    gdk_threads_enter();
    tmp = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);
    strncpy(model->artist, tmp, MAX_NAME);
    g_free(tmp);
//    gdk_threads_leave();
    update_artist(model->artist);
}

static void track_name_changed(GtkWidget * widget, gpointer data)
{
    tv_track *t;
    gchar *tmp;

    t = (tv_track *) data;
//    gdk_threads_enter();
    tmp = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);
    strncpy(t->name, tmp, MAX_NAME);
    g_free(tmp);
//    gdk_threads_leave();
    dlog("rx_track_view: track changed: %d - %s\n", t->nr, t->name);
    update_track(t);
}

static void track_selected(GtkWidget * widget, gpointer data)
{
    tv_track *t;

    t = (tv_track *) data;
//    gdk_threads_enter();
    t->include = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
//    gdk_threads_leave();
    dlog("rx_track_view: track selected: %d\n", t->include);
    update_track(t);
}

void rx_track_view_sync(RxTrackView * tv)
{
    int i;

    g_return_if_fail(RX_IS_TRACK_VIEW(tv));

//    gdk_threads_enter();
    g_signal_handler_block(tv->album_tb, tv->album_h);
    g_signal_handler_block(tv->artist_tb, tv->artist_h);
    gtk_entry_set_text(GTK_ENTRY(tv->artist_tb), tv->model->artist);
    gtk_entry_set_text(GTK_ENTRY(tv->album_tb), tv->model->album);
    g_signal_handler_unblock(tv->album_tb, tv->album_h);
    g_signal_handler_unblock(tv->artist_tb, tv->artist_h);
    gtk_option_menu_set_history(tv->genre_cb, tv->model->genre);
    for (i = 0; i < tv->model->tracks; i++) {
        tv_track t = tv->model->track[i];

        g_signal_handler_block(tv->tracksel[i], tv->tracksel_h[i]);
        g_signal_handler_block(tv->trackname[i], tv->trackname_h[i]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tv->tracksel[i]), t.include);
        gtk_entry_set_text(GTK_ENTRY(tv->trackname[i]), t.name);
        g_signal_handler_unblock(tv->tracksel[i], tv->tracksel_h[i]);
        g_signal_handler_unblock(tv->trackname[i], tv->trackname_h[i]);
    }
//    gdk_threads_leave();
}

static void select_all_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    int i;
    tv_model *model;
    RxTrackView *tv;
    gboolean on = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));

    model = (tv_model *) user_data;
    tv = model->tv;
//    gdk_threads_enter();
    for (i = 0; i < model->tracks; i++) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tv->tracksel[i]), on);
    }
//    gdk_threads_leave();
}

static void genre_changed(GtkWidget *combo, gpointer user_data)
{
    tv_model *model;
    RxTrackView *tv;
    int g;

    model = (tv_model *) user_data;
    tv = model->tv;
//    gdk_threads_enter();
    g = gtk_option_menu_get_history(tv->genre_cb);
    tv->model->genre = g;
    dlog("rx_track_view: genre_changed %d\n", g);
//    gdk_threads_leave();
}
