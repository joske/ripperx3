#ifndef PLUGIN_HANDLING_H
#define PLUGIN_HANDLING_H 1

#define SHARED_LIB_EXT ".so"
#define ERR_NO_PLUGIN  -1

#include <glib.h>
#include <cddb/cddb.h>

extern GHashTable *rippers;
extern GHashTable *encoders;

void init_plugins(void);
void clean_up_plugins(void);
int ripper_scan_cd(cddb_disc_t * disc);
int ripper_rip_track(cddb_disc_t * disc, cddb_track_t * track);
int encoder_encode_track(cddb_disc_t * disc, cddb_track_t * track);
int ripper_get_progress(void);
int encoder_get_progress(void);
void set_ripper(gchar * name);
void set_encoder(gchar * name);
gchar *get_ripper_name(void);
gchar *get_encoder_name(void);

#endif
