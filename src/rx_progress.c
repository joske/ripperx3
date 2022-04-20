#include <gtk/gtk.h>
#include <gtk/gtkdialog.h>
#include <glib-object.h>

#include "rx_progress.h"
#include "progress_gui.h"
#include "ripperx.h"

static GtkDialogClass *parent_class = NULL;

static void rx_progress_destroy(GtkObject * object);
static void rx_progress_class_init(RxProgressClass * klass);
static void rx_progress_init(RxProgress * pd);
static void abort_clicked(GtkButton * button, gpointer user_data);

guint rx_progress_get_type(void)
{
    static guint pd_type = 0;

    if (!pd_type) {
        static const GtkTypeInfo pd_info = {
            "RxProgress",
            sizeof(RxProgress),
            sizeof(RxProgressClass),
            (GtkClassInitFunc) rx_progress_class_init,
            (GtkObjectInitFunc) rx_progress_init,
            /* reserved_1 */ NULL,
            /* reserved_2 */ NULL,
            (GtkClassInitFunc) NULL,
        };

        pd_type = gtk_type_unique(gtk_dialog_get_type(), &pd_info);
    }

    return pd_type;
}

static void rx_progress_class_init(RxProgressClass * klass)
{
    GtkObjectClass *object_class;

    object_class = (GtkObjectClass *) klass;
    parent_class = gtk_type_class(GTK_TYPE_DIALOG);
    object_class->destroy = rx_progress_destroy;
}

static void rx_progress_init(RxProgress * pd)
{
    GTK_WIDGET_SET_FLAGS(GTK_WIDGET(pd), GTK_CAN_FOCUS);
}

GtkWidget *rx_progress_new()
{
    RxProgress *pd;

    dlog("RxProgress creation...");
    pd = gtk_type_new(rx_progress_get_type());

    g_signal_connect(G_OBJECT(pd), "delete", G_CALLBACK(abort_clicked), NULL);
    pd->pframe = gtk_frame_new("Progress");
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pd)->vbox), pd->pframe, TRUE, FALSE, 5);
    pd->ptable = gtk_table_new(2, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(pd->pframe), pd->ptable);
    pd->lb_rip = gtk_label_new("Not Ripping");
    pd->pb_rip = gtk_progress_bar_new();
    pd->lb_enc = gtk_label_new("Not Encoding");
    pd->pb_enc = gtk_progress_bar_new();
    gtk_table_attach(GTK_TABLE(pd->ptable), pd->lb_rip, 0, 1, 0, 1,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->ptable), pd->pb_rip, 1, 2, 0, 1,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->ptable), pd->lb_enc, 0, 1, 1, 2,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->ptable), pd->pb_enc, 1, 2, 1, 2,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    pd->sframe = gtk_frame_new("Statistics");
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pd)->vbox), pd->sframe, TRUE, FALSE, 5);
    pd->stable = gtk_table_new(5, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(pd->sframe), pd->stable);
    pd->lb_e = gtk_label_new("Time Elapsed");
    pd->lb_r = gtk_label_new("Time Remaining");
    pd->lb_t = gtk_label_new("Total Time");
    pd->lb_c = gtk_label_new("Tracks Completed");
    pd->lb_tr = gtk_label_new("Tracks Remaining");
    pd->lb_elapsed = gtk_label_new("");
    pd->lb_remain = gtk_label_new("");
    pd->lb_total = gtk_label_new("");
    pd->lb_complete = gtk_label_new("");
    pd->lb_tremain = gtk_label_new("");
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_e, 0, 1, 0, 1,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_elapsed, 1, 2, 0, 1,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_r, 0, 1, 1, 2,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_remain, 1, 2, 1, 2,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_t, 0, 1, 2, 3,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_total, 1, 2, 2, 3,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_c, 0, 1, 3, 4,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_complete, 1, 2, 3, 4,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_tr, 0, 1, 4, 5,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_table_attach(GTK_TABLE(pd->stable), pd->lb_tremain, 1, 2, 4, 5,
                     (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

    pd->buttonbox = gtk_button_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pd)->vbox), pd->buttonbox, TRUE, FALSE, 5);
    pd->abort = gtk_button_new_with_label("Abort");
    gtk_container_add(GTK_CONTAINER(pd->buttonbox), pd->abort);

    g_signal_connect(G_OBJECT(pd->abort), "clicked", G_CALLBACK(abort_clicked), NULL);

    g_object_ref(G_OBJECT(pd));
    gtk_object_sink(GTK_OBJECT(pd));

    dlog("done\n");
    return GTK_WIDGET(pd);
}

static void rx_progress_destroy(GtkObject * object)
{

    RxProgress *pd = NULL;

    g_return_if_fail(object != NULL);
    g_return_if_fail(RX_IS_PROGRESS(object));
    dlog("RxProgress destroying...");

    pd = RX_PROGRESS(object);
    gtk_widget_destroy(pd->abort);
    gtk_widget_destroy(pd->buttonbox);
    gtk_widget_destroy(pd->lb_tremain);
    gtk_widget_destroy(pd->lb_complete);
    gtk_widget_destroy(pd->lb_total);
    gtk_widget_destroy(pd->lb_remain);
    gtk_widget_destroy(pd->lb_elapsed);
    gtk_widget_destroy(pd->lb_tr);
    gtk_widget_destroy(pd->lb_c);
    gtk_widget_destroy(pd->lb_t);
    gtk_widget_destroy(pd->lb_r);
    gtk_widget_destroy(pd->lb_e);
    gtk_widget_destroy(pd->stable);
    gtk_widget_destroy(pd->sframe);
    gtk_widget_destroy(pd->pb_enc);
    gtk_widget_destroy(pd->lb_enc);
    gtk_widget_destroy(pd->pb_rip);
    gtk_widget_destroy(pd->lb_rip);
    gtk_widget_destroy(pd->ptable);
    gtk_widget_destroy(pd->pframe);

    /* Chain up */
    if (GTK_OBJECT_CLASS(parent_class)->destroy)
        (*GTK_OBJECT_CLASS(parent_class)->destroy) (object);

    dlog("destroyed\n");
    pd = NULL;
}

void rx_update_progress(RxProgress * pd, time_stat * stats)
{
    gchar enc_text[50];
    gchar rip_text[50];
    gchar elapsed_text[10];
    gchar complete_text[5];
    gchar remain_text[10];
    gchar total_text[10];
    gchar tremain_text[5];

    g_return_if_fail(pd != NULL);
    g_return_if_fail(RX_IS_PROGRESS(pd));

    g_snprintf(complete_text, sizeof(complete_text), "%2d", stats->done);
    g_snprintf(tremain_text, sizeof(tremain_text), "%2d", stats->todo);
    seconds2time(remain_text, sizeof(remain_text), stats->remain);
    seconds2time(elapsed_text, sizeof(elapsed_text), stats->now - stats->start);
    seconds2time(total_text, sizeof(total_text), stats->work_time);
    if (stats->rip_nr != -1) {
        g_snprintf(rip_text, sizeof(rip_text), "Ripping track %d", stats->rip_nr + 1);
        gtk_progress_set_value(GTK_PROGRESS(pd->pb_rip), stats->rip_progress);
    } else {
        g_snprintf(rip_text, sizeof(rip_text), "Not ripping");
        gtk_progress_set_value(GTK_PROGRESS(pd->pb_rip), 0);
    }
    if (stats->enc_nr != -1) {
        g_snprintf(enc_text, sizeof(enc_text), "Encoding track %d", stats->enc_nr + 1);
        gtk_progress_set_value(GTK_PROGRESS(pd->pb_enc), stats->enc_progress);
    } else {
        g_snprintf(enc_text, sizeof(enc_text), "Not encoding");
        gtk_progress_set_value(GTK_PROGRESS(pd->pb_enc), 0);
    }
    gtk_label_set_text(GTK_LABEL(pd->lb_rip), rip_text);
    gtk_label_set_text(GTK_LABEL(pd->lb_enc), enc_text);
    gtk_label_set_text(GTK_LABEL(pd->lb_elapsed), elapsed_text);
    gtk_label_set_text(GTK_LABEL(pd->lb_complete), complete_text);
    gtk_label_set_text(GTK_LABEL(pd->lb_tremain), tremain_text);
    gtk_label_set_text(GTK_LABEL(pd->lb_remain), remain_text);
    gtk_label_set_text(GTK_LABEL(pd->lb_total), total_text);
}

static void abort_clicked(GtkButton * button, gpointer user_data)
{
    dlog("rx_progress: abort_clicked()");
    stop_jobs();
}

