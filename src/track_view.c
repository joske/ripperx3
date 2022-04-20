#include <cddb/cddb_disc.h>
#include <cddb/cddb_track.h>
#include <stdio.h>
#include <string.h>
#include <cddb/cddb.h>

#include "track_view.h"
#include "ripperx.h"
#include "misc_util.h"
#include "main.h"
#include "config_handler.h"
#include "rx_track_view.h"

static GtkWidget *tv = NULL;

static tv_model *create_model(void);
static void update_model(tv_model *);

void track_view_init()
{
    static GtkWidget *frame = NULL;
    tv_model *model = NULL;

    dlog("track_view_init() called\n");
    if (tv) {
        gtk_widget_destroy(tv);
    }
    model = create_model();
    update_model(model);
    tv = rx_track_view_new(model);
    model->tv = RX_TRACK_VIEW(tv);
    main_set_track_view(tv);
}

static tv_model *create_model()
{
    tv_model *model = NULL;

    model = (tv_model *) g_malloc(sizeof(tv_model));
    return model;
}

static void update_model(tv_model * model)
{
    int i;
    cddb_disc_t *disc = main_get_disc();
    cddb_track_t *track;
    gchar tmp[6];

    const char *artist = cddb_track_get_artist(track);
    const char *dartist = cddb_disc_get_artist(disc);
    const char* title = cddb_track_get_title(track);
    const char* dtitle = cddb_disc_get_title(disc);
    int tlength = cddb_track_get_length(track);
    dlog("update_model()");
    dlog("\tdisc->artist = '%s'\n", dartist);
    if (dartist) {
        strncpy(model->artist, dartist, MAX_NAME - 1);
    }
    dlog("\tdisc->title = '%s'\n", disc->title);
    if (dtitle) {
        strncpy(model->album, dtitle, MAX_NAME - 1);
    }
    model->tracks = cddb_disc_get_track_count(disc);
    model->genre = cddb_to_id3(cddb_disc_get_category(disc));
    for (track = cddb_disc_get_track_first(disc), i = 0;
         track != NULL; track = cddb_disc_get_track_next(disc), i++) {
        dlog("\ttrack->length = '%d'\n", tlength);
        seconds2time(tmp, sizeof(tmp), tlength);
        model->track[i].nr = i;
        g_snprintf(model->track[i].tracknum, MAX_LN - 1, "%d", i + 1);
        strncpy(model->track[i].length, tmp, MAX_LL - 1);
        dlog("\ttrack->title = '%s'\n", track->title);
        if (title) {
            strncpy(model->track[i].name, title, MAX_NAME - 1);
        }
        model->track[i].model = model;
    }
}

void update_track(tv_track * track)
{
    int idx;
    cddb_disc_t *disc = main_get_disc();
    cddb_track_t *t;

    idx = track->nr;
    t = cddb_disc_get_track(disc, idx - 1);
    cddb_track_set_title(t, track->name);
    dlog("trackview.c: track_name_changed called : track %d=%s\n", idx, track->name);
}

void update_album(gchar * album)
{
    cddb_disc_set_title(main_get_disc(), album);
    dlog("trackview.c: update_album called : album=%s\n", album);
}

void update_artist(gchar * artist)
{
    cddb_disc_set_artist(main_get_disc(), artist);
    dlog("trackview.c: update_artist called : artist=%s\n", artist);
}

void update_view()
{
    update_model(RX_TRACK_VIEW(tv)->model);
    rx_track_view_sync(RX_TRACK_VIEW(tv));
}

gboolean is_track_selected(int idx)
{
    return RX_TRACK_VIEW(tv)->model->track[idx].include;
}
