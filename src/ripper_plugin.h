#ifndef RIPPER_PLUGIN_H
#define RIPPER_PLUGIN_H 1

#include <cddb/cddb.h>

#include "ripperx.h"
#include "rx_cfg.h"

#define CFG_PLUGIN_PATH             "path"

#define PLUGIN_FAILED               -1
#define PLUGIN_SUCCESS               1

typedef struct {
    void *handle;               /* handle returned by dlopen */
    gchar *filename;            /* filename of this plugin */
    gchar *name;
    gchar *description;
    rx_cfg *(*get_configuration) (void);        /* and the new config is returned here */
    void (*cleanup) (void);
} general_plugin;

typedef struct {
    void *handle;               /* handle returned by dlopen */
    gchar *filename;            /* filename of this plugin */
    gchar *name;
    gchar *description;
    rx_cfg *(*get_configuration) (void);        /* and the new config is returned here */
    void (*cleanup) (void);
    int (*scan_cd) (cddb_disc_t * disc);
    int (*rip_track) (cddb_disc_t * disc, cddb_track_t * track);
    int (*get_progress) (void);
} ripper_plugin;

typedef struct {
    void *handle;               /* handle returned by dlopen */
    gchar *filename;            /* filename of this plugin */
    gchar *name;
    gchar *description;
    rx_cfg *(*get_configuration) (void);
    void (*cleanup) (void);
    int (*encode_track) (cddb_disc_t * disc, cddb_track_t * track);
    int (*get_progress) (void);
} encoder_plugin;

ripper_plugin *get_ripper_plugin(void);
encoder_plugin *get_encoder_plugin(void);

#endif
