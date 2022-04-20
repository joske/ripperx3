#ifndef ID3_H
#define ID3_H 1

#include <cddb/cddb.h>
#include "id3_genre.h"
#include "rx_cfg.h"


typedef struct _id3tag id3tag;

struct _id3tag {
    char magic[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    id3_genre genre;
};

void id3_tag(rx_cfg *config, cddb_track_t *track);

#endif
