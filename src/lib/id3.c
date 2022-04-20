#include <cddb/cddb_track.h>
#include <stdio.h>
#include <string.h>

#include "ripperx.h"
#include "id3.h"
#include "debug.h"
#include "config_defines.h"
#include "rx_util.h"
#include "main.h"

#define MAGIC  "TAG"

static id3tag *create_tag(cddb_disc_t * disc, cddb_track_t *track);
static void tag_file(const gchar * file, id3tag * tag);

void id3_tag(rx_cfg *config, cddb_track_t *track)
{
    id3tag *tag;
    cddb_disc_t *disc = main_get_disc();
    gchar *dest = NULL;

    tag = create_tag(disc, track);
    dest = get_dest_file(config, disc, track);
    tag_file(dest, tag);
    g_free(dest);
    g_free(tag);
}

static id3tag *create_tag(cddb_disc_t * disc, cddb_track_t *track)
{
    id3tag *tag = NULL;

    tag = (id3tag *) g_malloc(sizeof(id3tag));
    strncpy(tag->magic, MAGIC, 3);
    const char* title = cddb_track_get_title(track);
    const char *artist = cddb_track_get_artist(track);
    const char* dtitle = cddb_disc_get_title(disc);
    if (title) {
        strncpy(tag->title, title, 29);
    } else {
        g_snprintf(tag->title, 29, "track %02d", cddb_track_get_number(track));
    }
    if (artist) {
        strncpy(tag->artist, artist, 29);
    } else {
        strncpy(tag->artist, "Unknown", 29);
    }
    if (dtitle) {
        strncpy(tag->album, dtitle, 29);
    } else {
        strncpy(tag->album, "Unknown", 29);
    }
    return tag;
}

static void tag_file(const gchar * file, id3tag * tag)
{
    FILE *f;

    dlog("id3:tag_file() opening file: %s\n", file);
    f = fopen(file, "a");
    if (f) {
        dlog("id3:tag_file() writing tag\n");
        fwrite(tag, 128, 1, f);
        fclose(f);
    }
}
