#ifndef RX_MAIN_H
#define RX_MAIN_H 1

#define RX_MAIN(obj)          GTK_CHECK_CAST (obj, rx_main_get_type (), RxMain)
#define RX_MAIN_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, rx_main_get_type (), RxMainClass)
#define RX_IS_MAIN(obj)       GTK_CHECK_TYPE (obj, rx_main_get_type ())

typedef struct _RxMain RxMain;
typedef struct _RxMainClass RxMainClass;

struct _RxMain {
    GtkWindow window;

    GtkWidget *table1;
    GtkWidget *toolbar;
    GtkWidget *tb_logo;
    GtkWidget *tb_config;
    GtkWidget *tb_scan;
    GtkWidget *tb_stop;
    GtkWidget *tb_cddb;
    GtkWidget *tb_submit;
    GtkWidget *tb_go;
    GtkWidget *tb_exit;
    GtkWidget *statusbar;
    GtkWidget *frame;
    GtkWidget *hbox1;
    GSList *ripenc_group;
    GtkWidget *rip_rb;
    GtkWidget *enc_rb;
};

struct _RxMainClass {
    GtkWindowClass parent_class;
};

void rx_main_set_track_view(RxMain * mw, GtkWidget * tv);
GtkWidget *rx_main_new(gboolean encode);

#endif
