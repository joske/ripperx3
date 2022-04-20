#include <cddb/cddb_track.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ripper_plugin.h"
#include "../ripperx.h"
#include "../config_defines.h"
#include "rx_util.h"

static int progress;
static rx_cfg *config = NULL;

static void strip_null(gchar * s, int len);
static int encode_track(cddb_disc_t * disc, cddb_track_t * track);
static int get_progress(void);
static rx_cfg *get_configuration(void);
static void cleanup_plugin(void);

encoder_plugin oggenc = {
    NULL,
    NULL,
    "oggenc",
    "OGG Vorbis plugin",
    get_configuration,
    cleanup_plugin,
    // What follows are encoder-only values
    encode_track,
    get_progress,
};

encoder_plugin *get_encoder_plugin(void)
{
    return &oggenc;
}

static int encode_track(cddb_disc_t * disc, cddb_track_t * track)
{
    FILE *p;
    gchar command[MAX_COMMAND_LENGTH];
    gchar buffer[100];
    gchar *fn_in, *fn_out;
    gchar *efn_in, *efn_out;

    progress = 0;
    fn_in = get_source_file(config, disc, track);
    fn_out = get_dest_file(config, disc, track);
    efn_in = escape(fn_in);
    efn_out = escape(fn_out);
    if (rx_cfg_get_bool(config, CFG_MAIN_CREATE_TAG)) {
        g_snprintf(command, MAX_COMMAND_LENGTH,
                   "%s '%s' -a '%s' -t '%s' -b %d -o '%s.ogg' 2>&1",
                   rx_cfg_get_str(config, CFG_PLUGIN_PATH),
                   efn_in,
                   cddb_track_get_artist(track),
                   cddb_track_get_title(track),
                   rx_cfg_get_int(config, CFG_MAIN_BIT_RATE),
                   efn_out);
    } else {
        g_snprintf(command, MAX_COMMAND_LENGTH,
                   "%s '%s' -b %d -o '%s.ogg' 2>&1",
                   rx_cfg_get_str(config, CFG_PLUGIN_PATH),
                   efn_in,
                   rx_cfg_get_int(config, CFG_MAIN_BIT_RATE),
                   efn_out);
    }
    dlog("command line for encoding:\n%s\n", command);
    p = popen(command, "r");
    while (fgets(buffer, sizeof(buffer) - 1, p) != 0) {
        float perc;
        gchar *at;

        strip_null(buffer, 99);
        at = strstr(buffer, "[");
        if (at != NULL) {
            sscanf(at + 1, " %f%% ", &perc);
            progress = (int) perc;
        }
    }
    pclose(p);
    free(fn_in);
    free(fn_out);
    free(efn_in);
    free(efn_out);
    progress = 100;
    return PLUGIN_SUCCESS;
}

static void strip_null(gchar * s, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (s[i] == '\0') {
            s[i] = ' ';
        }
    }
}

static int get_progress()
{
    return progress;
}

static rx_cfg *get_configuration(void)
{
    dlog("oggenc.get_configuration().\n");
    // This method will be called during RipperX start-up, so no
    // synchronization necessary to initialize the configuration
    // structure.
    if (!config) {
        gchar *exe = locate_exe(oggenc.name);

        if (exe) {
            config = rx_cfg_new(NULL);
            rx_cfg_add_str(config, CFG_PLUGIN_PATH, "", exe);
            g_free(exe);
        }
    }
    return config;
}

static void cleanup_plugin()
{
    rx_cfg_free(config);
}

