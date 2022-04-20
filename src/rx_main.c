#include <gtk/gtk.h>
#include <gtk/gtkwindow.h>
#include <glib-object.h>
#include "rx_main.h"
#include "debug.h"
#include "main.h"
#include "rx_pixmap.h"

static GtkWindowClass *parent_class = NULL;

static void rx_main_destroy(GtkObject * object);
static void rx_main_class_init(RxMainClass * klass);
static void rx_main_init(RxMain * mw);

static void config_clicked(GtkButton * button, gpointer user_data);
static void scan_clicked(GtkButton * button, gpointer user_data);
static void stop_clicked(GtkButton * button, gpointer user_data);
static void cddb_clicked(GtkButton * button, gpointer user_data);
static void submit_clicked(GtkButton * button, gpointer user_data);
static void go_clicked(GtkButton * button, gpointer user_data);
static void exit_clicked(GtkButton * button, gpointer user_data);
static void select_all_toggled(GtkToggleButton * togglebutton, gpointer user_data);
static void rip_rb_toggled(GtkToggleButton * togglebutton, gpointer user_data);
static void enc_rb_toggled(GtkToggleButton * togglebutton, gpointer user_data);


guint rx_main_get_type(void)
{
    static guint mw_type = 0;

    if (!mw_type) {
        static const GtkTypeInfo mw_info = {
            "RxMain",
            sizeof(RxMain),
            sizeof(RxMainClass),
            (GtkClassInitFunc) rx_main_class_init,
            (GtkObjectInitFunc) rx_main_init,
            /* reserved_1 */ NULL,
            /* reserved_2 */ NULL,
            (GtkClassInitFunc) NULL,
        };

        mw_type = gtk_type_unique(gtk_window_get_type(), &mw_info);
    }

    return mw_type;
}

static void rx_main_class_init(RxMainClass * klass)
{
    GtkObjectClass *object_class;

    object_class = (GtkObjectClass *) klass;
    parent_class = gtk_type_class(GTK_TYPE_WINDOW);
    object_class->destroy = rx_main_destroy;
}

static void rx_main_init(RxMain * mw)
{
}

GtkWidget *rx_main_new(gboolean encode)
{
    RxMain *mw;
    GtkWidget *tmp_toolbar_icon;
    int t;

    dlog("RxTrackView creation...");
    mw = gtk_type_new(rx_main_get_type());

    gtk_window_set_title(GTK_WINDOW(mw), "RipperX");
    gtk_window_set_default_size(GTK_WINDOW(mw), 100, 600);
    gtk_window_set_wmclass(GTK_WINDOW(mw), "RipperX", "RipperX");

    mw->table1 = gtk_table_new(4, 1, FALSE);
    gtk_widget_show(mw->table1);
    gtk_container_add(GTK_CONTAINER(mw), mw->table1);

    mw->toolbar = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(mw->toolbar);
    gtk_table_attach(GTK_TABLE(mw->table1), mw->toolbar, 0, 1, 0, 1,
                     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);

    mw->tb_logo = GTK_WIDGET(create_pixmap(GTK_WIDGET(mw), "ripperX.xpm"));
    gtk_widget_show(mw->tb_logo);
    gtk_box_pack_start(GTK_BOX(mw->toolbar), mw->tb_logo, FALSE, FALSE, 0);

    tmp_toolbar_icon = GTK_WIDGET(create_pixmap(GTK_WIDGET(mw), "config.xpm"));
    mw->tb_config = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(mw->tb_config), tmp_toolbar_icon);
    gtk_box_pack_start(GTK_BOX(mw->toolbar), mw->tb_config, FALSE, FALSE, 0);
    gtk_widget_show(mw->tb_config);

    tmp_toolbar_icon = GTK_WIDGET(create_pixmap(GTK_WIDGET(mw), "scan.xpm"));
    mw->tb_scan = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(mw->tb_scan), tmp_toolbar_icon);
    gtk_box_pack_start(GTK_BOX(mw->toolbar), mw->tb_scan, FALSE, FALSE, 0);
    gtk_widget_show(mw->tb_scan);

    tmp_toolbar_icon = GTK_WIDGET(create_pixmap(GTK_WIDGET(mw), "stop.xpm"));
    mw->tb_stop = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(mw->tb_stop), tmp_toolbar_icon);
    gtk_box_pack_start(GTK_BOX(mw->toolbar), mw->tb_stop, FALSE, FALSE, 0);
    gtk_widget_show(mw->tb_stop);

    tmp_toolbar_icon = GTK_WIDGET(create_pixmap(GTK_WIDGET(mw), "cddb.xpm"));
    mw->tb_cddb = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(mw->tb_cddb), tmp_toolbar_icon);
    gtk_box_pack_start(GTK_BOX(mw->toolbar), mw->tb_cddb, FALSE, FALSE, 0);
    gtk_widget_show(mw->tb_cddb);

    tmp_toolbar_icon = GTK_WIDGET(create_pixmap(GTK_WIDGET(mw), "submit.xpm"));
    mw->tb_submit = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(mw->tb_submit), tmp_toolbar_icon);
    gtk_box_pack_start(GTK_BOX(mw->toolbar), mw->tb_submit, FALSE, FALSE, 0);
    gtk_widget_show(mw->tb_submit);

    tmp_toolbar_icon = GTK_WIDGET(create_pixmap(GTK_WIDGET(mw), "go.xpm"));
    mw->tb_go = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(mw->tb_go), tmp_toolbar_icon);
    gtk_box_pack_start(GTK_BOX(mw->toolbar), mw->tb_go, FALSE, FALSE, 0);
    gtk_widget_show(mw->tb_go);

    tmp_toolbar_icon = GTK_WIDGET(create_pixmap(GTK_WIDGET(mw), "exit.xpm"));
    mw->tb_exit = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(mw->tb_exit), tmp_toolbar_icon);
    gtk_box_pack_end(GTK_BOX(mw->toolbar), mw->tb_exit, FALSE, FALSE, 10);
    gtk_widget_show(mw->tb_exit);

    mw->statusbar = gtk_statusbar_new();
    gtk_widget_show(mw->statusbar);
    gtk_table_attach(GTK_TABLE(mw->table1), mw->statusbar, 0, 1, 3, 4,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

    mw->frame = gtk_frame_new("RipperX");
    gtk_widget_show(mw->frame);
    gtk_table_attach(GTK_TABLE(mw->table1), mw->frame, 0, 1, 2, 3,
                     (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

    mw->hbox1 = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(mw->hbox1);
    gtk_table_attach(GTK_TABLE(mw->table1), mw->hbox1, 0, 1, 1, 2,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

    mw->rip_rb = gtk_radio_button_new_with_label(mw->ripenc_group, "Rip");
    mw->ripenc_group = gtk_radio_button_group(GTK_RADIO_BUTTON(mw->rip_rb));
    gtk_widget_show(mw->rip_rb);
    gtk_box_pack_start(GTK_BOX(mw->hbox1), mw->rip_rb, FALSE, FALSE, 0);

    mw->enc_rb = gtk_radio_button_new_with_label(mw->ripenc_group, "Encode");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mw->enc_rb), encode);
    mw->ripenc_group = gtk_radio_button_group(GTK_RADIO_BUTTON(mw->enc_rb));
    gtk_widget_show(mw->enc_rb);
    gtk_box_pack_start(GTK_BOX(mw->hbox1), mw->enc_rb, FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(mw->tb_config), "clicked", G_CALLBACK(config_clicked), NULL);
    g_signal_connect(G_OBJECT(mw->tb_scan), "clicked", G_CALLBACK(scan_clicked), NULL);
    g_signal_connect(G_OBJECT(mw->tb_stop), "clicked", G_CALLBACK(stop_clicked), NULL);
    g_signal_connect(G_OBJECT(mw->tb_cddb), "clicked", G_CALLBACK(cddb_clicked), NULL);
    g_signal_connect(G_OBJECT(mw->tb_submit), "clicked", G_CALLBACK(submit_clicked), NULL);
    g_signal_connect(G_OBJECT(mw->tb_go), "clicked", G_CALLBACK(go_clicked), NULL);
    g_signal_connect(G_OBJECT(mw->tb_exit), "clicked", G_CALLBACK(exit_clicked), NULL);
    g_signal_connect(G_OBJECT(mw->rip_rb), "toggled", G_CALLBACK(rip_rb_toggled), NULL);
    g_signal_connect(G_OBJECT(mw->enc_rb), "toggled", G_CALLBACK(enc_rb_toggled), NULL);

    g_object_ref(G_OBJECT(mw));
    gtk_object_sink(GTK_OBJECT(mw));

    gtk_widget_show_all(GTK_WIDGET(mw));
    dlog("complete\n");

    return GTK_WIDGET(mw);
}

static void rx_main_destroy(GtkObject * object)
{
    int i;
    RxMain *mw;

    g_return_if_fail(object != NULL);
    g_return_if_fail(RX_IS_MAIN(object));
    dlog("RxMain destroying...");

    mw = RX_MAIN(object);

    g_return_if_fail(RX_IS_MAIN(mw));

    /* Chain up */
    if (GTK_OBJECT_CLASS(parent_class)->destroy)
        (*GTK_OBJECT_CLASS(parent_class)->destroy) (object);

    dlog("destroyed\n");
    mw = NULL;
}

void scan_clicked(GtkButton * button, gpointer user_data)
{
    main_scan();
}


void exit_clicked(GtkButton * button, gpointer user_data)
{
    main_exit();
}


void config_clicked(GtkButton * button, gpointer user_data)
{
    main_config();
}


void stop_clicked(GtkButton * button, gpointer user_data)
{

}


void cddb_clicked(GtkButton * button, gpointer user_data)
{
    main_cddb();
}

void submit_clicked(GtkButton * button, gpointer user_data)
{
    main_cddb_submit();
}

void go_clicked(GtkButton * button, gpointer user_data)
{
    main_go();
}


void rip_rb_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    main_must_encode(!gtk_toggle_button_get_active(togglebutton));
}


void enc_rb_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    main_must_encode(gtk_toggle_button_get_active(togglebutton));
}

void rx_main_set_track_view(RxMain * mw, GtkWidget * tv)
{
    g_return_if_fail(mw != NULL);
    g_return_if_fail(tv != NULL);
    g_return_if_fail(RX_IS_MAIN(mw));
    g_return_if_fail(GTK_IS_WIDGET(tv));

    gtk_container_add(GTK_CONTAINER(mw->frame), tv);
    gtk_widget_show_all(mw->frame);
}
