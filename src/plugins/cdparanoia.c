#include <cddb/cddb_disc.h>
#include <cddb/cddb_track.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h>

#include "../ripper_plugin.h"
#include "../ripperx.h"
#include "../config_defines.h"
#include "rx_util.h"


#define BYTES_PER_SECTOR    1176

#define CFG_CDP_DEVICE            "device"
#define CFG_CDP_PATH              "path"
#define CFG_CDP_NO_PARANOIA       "disableParanoia"
#define CFG_CDP_NO_EXTRA_PARANOIA "disableExtraParanoia"
#define CFG_CDP_NO_SCRATCH_DETECT "disableScratchDetection"
#define CFG_CDP_NO_SCRATCH_REPAIR "disableScratchRepair"

static int progress;
static rx_cfg *config = NULL;

//static void calculate_discid(_disc * disc);
static int scan_cd(cddb_disc_t * disc);
static int rip_track(cddb_disc_t * disc, cddb_track_t * track);
static int get_progress(void);
static rx_cfg *get_configuration(void);
static void cleanup_plugin(void);

ripper_plugin cdparanoia = {
    NULL,                       /*handle - filled in by main */
    NULL,                       /*filename of this plugin - filled in by main */
    "cdparanoia",
    "cdparanoia plugin",        /*description */
    get_configuration,
    cleanup_plugin,
    // What follows are ripper-only values
    scan_cd,
    rip_track,
    get_progress,
};

ripper_plugin *get_ripper_plugin(void)
{
    return &cdparanoia;
}

static int scan_cd(cddb_disc_t * disc)
{
    FILE *p;
    gchar buffer[100];
    GString *cmd;

    dlog("cdparanoia.scan_cd().\n");
    // Build paranoia command
    cmd = g_string_new(rx_cfg_get_str(config, CFG_CDP_PATH));
    g_string_append(cmd, " -Q -d ");
    g_string_append(cmd, rx_cfg_get_str(config, CFG_CDP_DEVICE));
    if (rx_cfg_get_bool(config, CFG_CDP_NO_PARANOIA)) {
        g_string_append(cmd, " -Z");
    }
    if (rx_cfg_get_bool(config, CFG_CDP_NO_EXTRA_PARANOIA)) {
        g_string_append(cmd, " -Y");
    }
    g_string_append(cmd, " 2>&1");
    dlog("cdparanoia.scan_cd: '%s'.\n", cmd->str);
    // Execute paranoia command
    p = popen(cmd->str, "r");
    if (p == NULL) {
        return PLUGIN_FAILED;
    }
    while (fgets(buffer, sizeof(buffer) - 1, p) != 0) {
        int nr;
        long len, offset, tot;
        cddb_track_t *track;

        dlog("..Read: %s", buffer);
        if (strncmp(buffer, "Unable", 6) == 0) {
            return PLUGIN_FAILED;
        }
        if (sscanf(buffer, "%d.\t%ld", &nr, &len) == 2) {
            gchar *sub = strchr(buffer, ']');

            sscanf(sub, "]\t%ld ", &offset);
            dlog("..Nr: %2d\tLen: %6ld\tOffset: %6ld\n", nr, len, offset);
            track = cddb_track_new();
            cddb_track_set_length(track, len / SECTORS_PER_SECOND);

            cddb_track_set_frame_offset(track, offset + 150);
            cddb_track_set_title(track, g_strdup_printf("track %d", nr));
            cddb_disc_add_track(disc, track);
        }
        if (sscanf(buffer, "TOTAL\t%ld ", &tot) == 1) {
            dlog("..Total length: %ld\n", tot);
            cddb_disc_set_length(disc, (tot + 150) / 75);
        }
    }
    pclose(p);
    //calculate_discid(disc);
    cddb_disc_calc_discid(disc);        /* XXX: error handling */
    // Clean up command
    g_string_free(cmd, TRUE);
    dlog("cdparanoia.scan_cd done\n");
    cddb_disc_print(disc);
    return PLUGIN_SUCCESS;
}

static int rip_track(cddb_disc_t * disc, cddb_track_t * track)
{
    FILE *p;
    gchar *command;
    gchar buffer[100];
    gchar *fn;
    gchar *efn;

    progress = 0;
    fn = get_source_file(config, disc, track);
    efn = escape(fn);
    command = g_strdup_printf("%s -d %s -e %s %s %d '%s' 2>&1",
                              rx_cfg_get_str(config, CFG_CDP_PATH),
                              rx_cfg_get_str(config, CFG_CDP_DEVICE), 
                              rx_cfg_get_bool(config, CFG_CDP_NO_PARANOIA) ? "-Z" : "",
                              rx_cfg_get_bool(config, CFG_CDP_NO_EXTRA_PARANOIA) ? "-Y" : "",
                              cddb_track_get_number(track), efn);
    dlog("cdparanoia: executing command: %s\n", command);
    p = popen(command, "r");
    while (fgets(buffer, sizeof(buffer) - 1, p) != 0) {
        long sector;
        long bytes;
        gchar *at;

        at = strstr(buffer, "wrote]");
        if (at != NULL) {
            sscanf(at, "wrote] @ %ld", &bytes);
            sector = bytes / BYTES_PER_SECTOR;
            /* XXX: offset: frames <-> sectors */
            progress =
                (sector - cddb_track_get_frame_offset(track) * 100.0 / (cddb_track_get_length(track) * SECTORS_PER_SECOND));
        }
    }
    progress = 100;
    pclose(p);
    free(fn);
    free(efn);
    free(command);
    dlog("cdparanoia.rip_track done\n");
    return PLUGIN_SUCCESS;
}

static int get_progress()
{
    return progress;
}

static rx_cfg *get_configuration(void)
{
    dlog("cdparanoia.get_configuration().\n");
    // This method will be called during RipperX start-up, so no
    // synchronization necessary to initialize the configuration
    // structure.
    if (!config) {
        gchar *exe = locate_exe(cdparanoia.name);

        if (exe) {
            config = rx_cfg_new(NULL);
            rx_cfg_add_str(config, CFG_CDP_DEVICE, "", "/dev/cdrom");
            rx_cfg_add_str(config, CFG_CDP_PATH, "", exe);
            g_free(exe);
            rx_cfg_add_bool(config, CFG_CDP_NO_PARANOIA, "", FALSE);
            rx_cfg_add_bool(config, CFG_CDP_NO_EXTRA_PARANOIA, "", FALSE);
            rx_cfg_add_bool(config, CFG_CDP_NO_SCRATCH_DETECT, "", FALSE);
            rx_cfg_add_bool(config, CFG_CDP_NO_SCRATCH_REPAIR, "", FALSE);
        }
    }
    return config;
}

static void cleanup_plugin()
{
    rx_cfg_free(config);
}
