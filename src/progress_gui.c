#include <stdio.h>
#include <time.h>

#include "ripperx.h"
#include "progress_gui.h"
#include "main.h"
#include "misc_util.h"
#include "rx_progress.h"

static GtkWidget *pd = NULL;

void show_progress()
{
    if (pd == NULL) {
        pd = GTK_WIDGET(rx_progress_new());
    }
    gtk_grab_add(pd);
    gtk_widget_show_all(GTK_WIDGET(pd));

}

void update_progress(time_stat * stats)
{
    rx_update_progress(pd, stats);
}

void hide_progress()
{
    gtk_widget_hide(pd);
    gtk_grab_remove(pd);
    gtk_widget_destroy(pd);
    pd = NULL;
}
