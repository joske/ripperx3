#ifndef RX_PIXMAP_H
#define RX_PIXMAP_H 1

#include <gtk/gtk.h>

GtkWidget *create_pixmap(GtkWidget * widget, const gchar * filename);
void add_pixmap_directory(const gchar * directory);

#endif
