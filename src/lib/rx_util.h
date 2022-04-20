#ifndef RX_UTIL_H
#define RX_UTIL_H 1

#include <glib.h>
#include <cddb/cddb.h>

#include "rx_cfg.h"
#include "ripperx.h"

#define RX_DIR_OK                   0
#define RX_DIR_DOES_NOT_EXIST      -1
#define RX_DIR_NOT_DIR             -2
#define RX_DIR_NOT_WRITABLE        -3

gchar *get_dest_file(rx_cfg * config, cddb_disc_t * disc, cddb_track_t * track);
gchar *get_source_file(rx_cfg * config, cddb_disc_t * disc, cddb_track_t * track);
gchar *locate_exe(gchar * exe);
int create_basedir(gchar * path);
int check_dir(char *dir);
gchar *escape(gchar *orig);

#endif /* RX_UTIL_H */
