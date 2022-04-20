#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <glib.h>
#include <string.h>
#include <sys/stat.h>

#include "config_handler.h"
#include "ripper_plugin.h"
#include "plugin_handling.h"
#include "ripperx.h"
#include "rx_util.h"
#include "main.h"
#include "id3.h"

GHashTable *rippers = NULL;
GHashTable *encoders = NULL;

static ripper_plugin *ripper;
static encoder_plugin *encoder;

static void add_plugin(gchar * filename);
static void scan_plugins(gchar * dirname);

static void add_plugin(gchar * filename)
{
    void *h;
    void *(*gpi) (void);

    dlog("add_plugin: '%s'.\n", filename);
    if ((h = dlopen(filename, RTLD_NOW)) == NULL) {
        fprintf(stderr, "failed to open plugin lib: %s\n", dlerror());
        return;
    }

    if ((gpi = dlsym(h, "get_ripper_plugin")) != NULL) {
        ripper_plugin *r;
        rx_cfg *cfg = NULL;

        r = gpi();
        r->handle = h;
        r->filename = g_strdup(filename);
        cfg = r->get_configuration();   /* Let plugin initialize itself. */
        if (cfg) {              /* only add plugins that find their exe */
            g_hash_table_insert(rippers, r->name, r);
        }
    }
    if ((gpi = dlsym(h, "get_encoder_plugin")) != NULL) {
        encoder_plugin *e;
        rx_cfg *cfg = NULL;

        e = gpi();
        e->handle = h;
        e->filename = g_strdup(filename);
        cfg = e->get_configuration();   /* Let plugin initialize itself. */
        if (cfg) {              /* only add plugins that find their exe */
            g_hash_table_insert(encoders, e->name, e);
        }
    }

}

static void scan_plugins(char *dirname)
{
    gchar *filename, *ext;
    DIR *dir;
    struct dirent *ent;
    struct stat statbuf;

    dir = opendir(dirname);
    if (!dir)
        return;

    while ((ent = readdir(dir)) != NULL) {
        filename = g_strdup_printf("%s/%s", dirname, ent->d_name);
        if (!stat(filename, &statbuf) && S_ISREG(statbuf.st_mode) &&
            (ext = strrchr(ent->d_name, '.')) != NULL) {
            if (!strcmp(ext, SHARED_LIB_EXT)) {
                add_plugin(filename);
            }
        }
        g_free(filename);
    }
    closedir(dir);
}


void init_plugins()
{
    GtkWidget *cb_ripper;
    GtkWidget *cb_encoder;
    gchar *pluginDir;

    rippers = g_hash_table_new(g_str_hash, g_str_equal);
    encoders = g_hash_table_new(g_str_hash, g_str_equal);
    pluginDir = rx_cfg_get_str(main_get_config(), CFG_MAIN_PLUGIN_DIR);
    scan_plugins(pluginDir);
}


void clean_up_plugin_1(gpointer key, gpointer value, gpointer user_data)
{
    general_plugin *plugin;

    plugin = (general_plugin *) value;
    dlog("clean_up_plugin_1: %s.\n", plugin->name);
    if (plugin->cleanup) {
        plugin->cleanup();
    }
}

void clean_up_plugins()
{
    dlog("clean_up_plugins.\n");
    dlog("\trippers.\n");
    g_hash_table_foreach(rippers, clean_up_plugin_1, NULL);
    dlog("\tencoders.\n");
    g_hash_table_foreach(encoders, clean_up_plugin_1, NULL);
}


int ripper_scan_cd(cddb_disc_t * disc)
{
    if (!ripper) {
        return ERR_NO_PLUGIN;
    }
    return ripper->scan_cd(disc);
}

int ripper_rip_track(cddb_disc_t * disc, cddb_track_t * track)
{
    if (!ripper) {
        return ERR_NO_PLUGIN;
    }
    return ripper->rip_track(disc, track);
}

int encoder_encode_track(cddb_disc_t * disc, cddb_track_t * track)
{
    if (!encoder) {
        return ERR_NO_PLUGIN;
    }
    if (!encoder->encode_track(disc, track)) {
        return PLUGIN_FAILED;
    }
    return PLUGIN_SUCCESS;
}

int encoder_get_progress()
{
    if (!encoder) {
        return ERR_NO_PLUGIN;
    }
    return encoder->get_progress();
}

int ripper_get_progress()
{
    return ripper->get_progress();
}

void set_ripper(gchar * name)
{
    ripper_plugin *r;

    r = (ripper_plugin *) g_hash_table_lookup(rippers, name);
    ripper = r;
}

void set_encoder(gchar * name)
{
    encoder_plugin *e;

    e = (encoder_plugin *) g_hash_table_lookup(encoders, name);
    encoder = e;
}

gchar *get_ripper_name()
{
    return ripper->name;
}

gchar *get_encoder_name()
{
    return encoder->name;
}
