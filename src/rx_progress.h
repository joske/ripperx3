#ifndef RX_PROGRESS_H
#define RX_PROGRESS_H 1

#include <gtk/gtkwidget.h>
#include <gtk/gtk.h>

typedef struct _RxProgress RxProgress;
typedef struct _RxProgressClass RxProgressClass;

#define RX_PROGRESS(obj)          GTK_CHECK_CAST (obj, rx_progress_get_type (), RxProgress)
#define RX_PROGRESS_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, rx_progress_get_type (), RxProgressClass)
#define RX_IS_PROGRESS(obj)       GTK_CHECK_TYPE (obj, rx_progress_get_type ())

struct _RxProgress {
    GtkDialog dialog;

    GtkWidget *pframe;
    GtkWidget *ptable;
    GtkWidget *lb_rip;
    GtkWidget *lb_enc;
    GtkWidget *pb_rip;
    GtkWidget *pb_enc;
    GtkWidget *sframe;
    GtkWidget *stable;
    GtkWidget *lb_e;
    GtkWidget *lb_elapsed;
    GtkWidget *lb_r;
    GtkWidget *lb_remain;
    GtkWidget *lb_t;
    GtkWidget *lb_total;
    GtkWidget *lb_c;
    GtkWidget *lb_complete;
    GtkWidget *lb_tr;
    GtkWidget *lb_tremain;
    GtkWidget *buttonbox;
    GtkWidget *abort;
};

struct _RxProgressClass {
    GtkDialogClass parent_class;
};

#endif
