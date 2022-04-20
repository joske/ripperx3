#ifndef RX_TRACK_VIEW_H
#define RX_TRACK_VIEW_H 1

#include <gtk/gtkwidget.h>
#include <gtk/gtk.h>

#define MAX_TRACKS                  256
#define MAX_NAME                    256
#define MAX_LN                      4
#define MAX_LL                      6

#define RX_TRACK_VIEW(obj)          GTK_CHECK_CAST (obj, rx_track_view_get_type (), RxTrackView)
#define RX_TRACK_VIEW_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, rx_track_view_get_type (), RxTrackViewClass)
#define RX_IS_TRACK_VIEW(obj)       GTK_CHECK_TYPE (obj, rx_track_view_get_type ())

typedef struct _RxTrackView RxTrackView;
typedef struct _RxTrackViewClass RxTrackViewClass;
typedef struct _tv_model tv_model;
typedef struct _tv_track tv_track;

struct _RxTrackView {
    GtkVBox vbox;

    tv_model *model;
    gulong album_h;
    gulong artist_h;
    gulong genre_h;
    gulong tracksel_h[MAX_TRACKS];
    gulong trackname_h[MAX_TRACKS];
    GtkWidget *header_box;
    GtkWidget *select_all;
    GtkWidget *artist_lb;
    GtkWidget *album_lb;
    GtkWidget *artist_tb;
    GtkWidget *album_tb;
    GtkWidget *menu;
    GtkWidget *genre_lb;
    GtkWidget *genre_cb;
    GtkWidget *scrollpane;
    GtkWidget *track_vbox;
    GtkWidget *track_hbox[MAX_TRACKS];
    GtkWidget *play_b[MAX_TRACKS];
    GtkWidget *tracknum[MAX_TRACKS];
    GtkWidget *tracklen[MAX_TRACKS];
    GtkWidget *tracksel[MAX_TRACKS];
    GtkWidget *trackname[MAX_TRACKS];
};

struct _RxTrackViewClass {
    GtkVBoxClass parent_class;
    void (*track_name_changed) (RxTrackView * tv, gpointer data);
};

struct _tv_track {
    gint nr;
    gchar artist[MAX_NAME];
    gchar name[MAX_NAME];
    gchar tracknum[MAX_LN];
    gchar length[MAX_LL];
    gboolean include;
    tv_model *model;
};

struct _tv_model {
    gchar artist[MAX_NAME];
    gchar album[MAX_NAME];
    gint tracks;
    tv_track track[MAX_TRACKS];
    RxTrackView *tv;
    int genre;
};

guint rx_track_view_get_type(void);
GtkWidget *rx_track_view_new(tv_model * model);
void rx_track_view_sync(RxTrackView * tv);

#endif
