#ifndef MAIN_H
#define MAIN_H 1

#include <glib.h>
#include <gtk/gtk.h>
#include <cddb/cddb.h>

#include "ripperx.h"
#include "rx_cfg.h"

/* public interface */
void main_config(void);
void main_scan(void);
void main_cddb(void);
void main_cddb_submit(void);
void main_go(void);
void main_exit(void);
void main_must_encode(gboolean encode);
void main_set_track_view(GtkWidget * tv);
rx_cfg *main_get_config(void);
cddb_disc_t *main_get_disc(void);

#endif
