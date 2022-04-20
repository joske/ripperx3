#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

/*
 * An internal structure to store some information about the selection
 * popup.
 */
typedef struct {
    GtkWidget *window;
    GtkTreeView *list;
    GtkTreeIter iter;
} cddb_popup;

cddb_popup *cddb_popup_create(GtkWindow * parent);

void cddb_popup_clear(cddb_popup * popup);

void cddb_popup_insert(cddb_popup * popup, const char *artist, const char *title,
                       const char *genre, const glong discid);

gboolean cddb_popup_query_user(cddb_popup * popup);

gchar *cddb_popup_get_genre(cddb_popup * popup);

glong cddb_popup_get_discid(cddb_popup * popup);
