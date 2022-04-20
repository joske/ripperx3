#include <cddb/cddb_track.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "job.h"
#include "main.h"
#include "ripperx.h"
#include "config_handler.h"
#include "rx_cfg.h"
#include "plugin_handling.h"
#include "progress_gui.h"
#include "misc_util.h"
#include "rx_util.h"

static GList *rip_queue = NULL;
static GList *encode_queue = NULL;

static void *do_rip(void *q);
static void *do_encode(void *q);
static void *do_tick(void *q);
static void calculate(time_stat * stats);

static pthread_t rip_thread;
static pthread_t encode_thread;
static pthread_t clock_thread;
static time_stat *stats;

void start_jobs()
{
    cddb_track_t *track;
    rx_cfg *config = main_get_config();
    cddb_disc_t *disc = main_get_disc();
    int i;
    gboolean ad;

    stats = g_malloc(sizeof(time_stat));
    stats->done = 0;
    stats->todo = 0;
    stats->remain = 0;
    stats->rip_nr = -1;
    stats->enc_nr = -1;
    stats->total_time = 0;
    stats->seconds_ripped = 0;
    stats->seconds_encoded = 0;
    for (track = cddb_disc_get_track_first(disc), i = 0;
         track != NULL; track = cddb_disc_get_track_next(disc), i++) {
        unsigned int length = cddb_track_get_length(track);
        if (is_track_selected(i)) {
            rip_queue = g_list_prepend(rip_queue, track);
            stats->todo++;
            stats->total_time += length;
        }
    }
    ad = rx_cfg_get_bool(config, CFG_MAIN_DIR_ALBUM);
    if (ad) {
        gchar * spath = get_source_file(config, disc, NULL);
        gchar * epath = get_dest_file(config, disc, NULL);
        
        create_basedir(spath);
        create_basedir(epath);
    }
    time(&stats->start);
    stats->rip_start = -1;
    stats->enc_start = -1;
    if (stats->todo > 0) {
        rip_queue = g_list_reverse(rip_queue);
        pthread_create(&rip_thread, NULL, do_rip, NULL);
        pthread_create(&encode_thread, NULL, do_encode, NULL);
        do_tick(NULL);
        pthread_join(rip_thread, NULL);
        pthread_join(encode_thread, NULL);
    }
    g_free(stats);
    dlog("return to mainloop\n");
}

void stop_jobs()
{
    pthread_cancel(rip_thread);
    pthread_cancel(encode_thread);
}

static void *do_rip(void *q)
{
    cddb_disc_t *disc = main_get_disc();
    
    time(&stats->rip_start);
    while (stats->todo > 0 && rip_queue != NULL) {
        cddb_track_t *track = (cddb_track_t *) rip_queue->data;
        unsigned int num = cddb_track_get_number(track);
        unsigned int length = cddb_track_get_length(track);

        /* tracks start counting themselves at 1 */
        stats->rip_nr = (num - 1);
        ripper_rip_track(disc, track);
        if (rx_cfg_get_bool(main_get_config(), CFG_MAIN_ENCODE) == TRUE) {
            encode_queue = g_list_append(encode_queue, track);
        } else {
            stats->todo--;
            stats->done++;
        }
        stats->seconds_ripped += length;
        rip_queue = g_list_remove(rip_queue, track);
        stats->rip_nr = -1;
    }
    pthread_exit(NULL);
}

static void *do_encode(void *q)
{
    rx_cfg *config = main_get_config();
    if (rx_cfg_get_bool(config, CFG_MAIN_ENCODE) == TRUE) {
        cddb_disc_t *disc = main_get_disc();
        while (encode_queue == NULL) {
            sleep(1);
        }
        time(&stats->enc_start);
        while (stats->todo > 0 && encode_queue != NULL) {
            cddb_track_t *track = (cddb_track_t *) encode_queue->data;
        unsigned int num = cddb_track_get_number(track);
        unsigned int length = cddb_track_get_length(track);

            /* tracks start counting themselves at 1 */
            stats->enc_nr = (num - 1);
            encoder_encode_track(disc, track);
            stats->todo--;
            stats->done++;
            stats->enc_nr = -1;
            stats->seconds_encoded += length;
            if (!rx_cfg_get_bool(config, CFG_MAIN_KEEP_WAV)) {
                cddb_disc_t *disc = main_get_disc();
                gchar *fn = get_source_file(config, disc, track);

                remove(fn);
                free(fn);
            }
            while (stats->todo > 0 && encode_queue->next == NULL) {
                sleep(1);       /* wait for next track to get ripped */
            }
            encode_queue = g_list_remove(encode_queue, track);
        }
    }
    pthread_exit(NULL);
}



static void *do_tick(void *q)
{
    dlog("do_tick() going in\n");
    show_progress();
    while (stats->todo > 0) {
        time(&stats->now);
        stats->enc_progress = encoder_get_progress();
        stats->rip_progress = ripper_get_progress();
        calculate(stats);
        update_progress(stats);
        /* WARNING black magic ahead WARNING */
        while (gtk_events_pending()) {
            gtk_main_iteration_do(FALSE);
        }
        usleep(50000);
    }
    hide_progress();
    dlog("do_tick() got out\n");
    return NULL;
}

static void calculate(time_stat * stats)
{
    int rip_remain = 0;
    int enc_remain = 0;
    double rip_ratio, enc_ratio;
    cddb_disc_t *disc;
    cddb_track_t *track;
    rx_cfg *config;

    disc = main_get_disc();
    config = main_get_config();
    rip_ratio = rx_cfg_get_float(config, CFG_MAIN_RIP_RATIO);
    enc_ratio = rx_cfg_get_float(config, CFG_MAIN_ENC_RATIO);
    //dlog("rip = %3.2f\tenc = %3.2f\n", rip_ratio, enc_ratio);
    /* do some useful estimation of remaining time here */
    if (stats->rip_nr > -1) {
        long rip_done, rip_elapsed;

        track = cddb_disc_get_track(disc, stats->rip_nr);
        unsigned int length = cddb_track_get_length(track);
        rip_elapsed = stats->now - stats->rip_start;
        rip_done = stats->seconds_ripped + (length * stats->rip_progress / 100.0);
        if (rip_done != 0) {
            rip_ratio = (.9 * rip_ratio) + (.1 * rip_elapsed / rip_done);
            rx_cfg_upd_float(config, CFG_MAIN_RIP_RATIO, rip_ratio);
        }
        rip_remain = (stats->total_time - rip_done) * rip_ratio;
    }
    if (stats->enc_nr > -1) {
        long enc_done, enc_elapsed;

        track = cddb_disc_get_track(disc, stats->enc_nr);
        unsigned int length = cddb_track_get_length(track);
        enc_elapsed = stats->now - stats->enc_start;
        enc_done = stats->seconds_encoded + (length * stats->enc_progress / 100.0);
        if (enc_done != 0) {
            enc_ratio = (.9 * enc_ratio) + (.1 * enc_elapsed / enc_done);
            rx_cfg_upd_float(config, CFG_MAIN_ENC_RATIO, enc_ratio);
        }
        enc_remain = (stats->total_time - enc_done) * enc_ratio;
    } else if (rx_cfg_get_bool(main_get_config(), CFG_MAIN_ENCODE)) {
        /* if not encoding now, but encoding necessary in near future */
        enc_remain = enc_ratio * (stats->total_time - stats->seconds_encoded);
    }
    stats->remain = rip_remain + enc_remain;
    stats->work_time = (stats->now - stats->start) + stats->remain;
}
