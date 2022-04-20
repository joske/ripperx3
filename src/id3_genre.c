
#include <glib.h>
#include "id3_genre.h"
#include "debug.h"

/*
 * Known CDDB categories (genres) acquired by executing the 'cddb
 * lscat' command on freedb.org:888.  Mapping to ID3 genres follows:
 *
 *    data       255 (unknown ??)   CDDB_CAT_DATA
 *    folk        80                CDDB_CAT_FOLK
 *    jazz         8                CDDB_CAT_JAZZ
 *    misc        12 (other)        CDDB_CAT_MISC
 *    rock        17                CDDB_CAT_ROCK
 *    country      2                CDDB_CAT_COUNTRY
 *    blues        0                CDDB_CAT_BLUES
 *    newage      10                CDDB_CAT_NEWAGE
 *    reggae      16                CDDB_CAT_REGGAE
 *    classical   32                CDDB_CAT_CLASSICAL
 *    soundtrack  24                CDDB_CAT_SOUNDTRACK
 */

const char *ID3_GENRE[256] = {
    /* 0 */
    "blues", "classic rock", "country", "dance", "disco", "funk", "grunge",
    "hip hop", "jazz", "metal", "new age", "oldies", "other", "pop",
    "rythm and blues", "rap", "reggae", "rock", "techno", "industrial",
    "alternative", "ska", "death metal", "pranks", "soundtrack",
    "euro techno", "ambient", "trip hop", "vocal", "jazz funk", "fusion",
    "trance", "classical", "instrumental", "acid", "house", "game",
    "sound clip", "gospel", "noise", "alternrock", "bass", "soul", "punk",
    "space", "meditative", "instrumental pop", "instrumental rock", "ethnic",
    "gothic", "darkwave", "techno industrial", "electronic", "pop folk",
    "eurodance", "dream", "southern rock", "comedy", "cult", "gangsta",
    "top 40", "christian rap", "pop funk", "jungle", "native american",
    "cabaret", "new wave", "psychadelic", "rave", "showtunes", "trailer",
    "lo fi", "tribal", "acid punk", "acid jazz", "polka", "retro",
    "musical", "rock and roll", "hard rock", "folk", "folk rock",
    "national folk", "swing", "fast fusion", "bebob", "latin", "revival",
    "celtic", "bluegrass", "avantgarde", "gothic rock", "progressive rock",
    "psychedelic rock", "symphonic rock", "slow rock", "big band",
    "chorus", "easy listening", "acoustic", "humour", "speech", "chanson",
    "opera", "chamber music", "sonata", "symphony", "booty bass", "primus",
    "porn groove", "satire", "slow jam", "club", "tango", "samba",
    "folklore", "ballad", "power ballad", "rhythmic soul", "freestyle",
    "duet", "punk rock", "drum solo", "a capella", "euro house",
    "dance hall",
    /* 126 */
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    /* 255 */
    "unknown"
};

static GHashTable *cddb_to_id3_map = NULL;
static GList *id3_genre_list = NULL;


gint *new_int(gint i)
{
    gint *iptr;

    iptr = g_malloc(sizeof(gint));
    *iptr = i;
    return iptr;
}

id3_genre cddb_to_id3(cddb_cat_t genre)
{
    gpointer *p;

    if (cddb_to_id3_map == NULL) {
        dlog("Initializing CDDB to ID3 mapping\n");
        cddb_to_id3_map = g_hash_table_new(g_str_hash, g_str_equal);
        g_hash_table_insert(cddb_to_id3_map, "data", new_int(ID3_UNKNOWN));
        g_hash_table_insert(cddb_to_id3_map, "folk", new_int(ID3_FOLK));
        g_hash_table_insert(cddb_to_id3_map, "jazz", new_int(ID3_JAZZ));
        g_hash_table_insert(cddb_to_id3_map, "misc", new_int(ID3_OTHER));
        g_hash_table_insert(cddb_to_id3_map, "rock", new_int(ID3_ROCK));
        g_hash_table_insert(cddb_to_id3_map, "country", new_int(ID3_COUNTRY));
        g_hash_table_insert(cddb_to_id3_map, "blues", new_int(ID3_BLUES));
        g_hash_table_insert(cddb_to_id3_map, "newage", new_int(ID3_NEW_AGE));
        g_hash_table_insert(cddb_to_id3_map, "reggae", new_int(ID3_REGGAE));
        g_hash_table_insert(cddb_to_id3_map, "classical", new_int(ID3_CLASSICAL));
        g_hash_table_insert(cddb_to_id3_map, "soundtrack", new_int(ID3_SOUNDTRACK));
    }
    p = g_hash_table_lookup(cddb_to_id3_map, CDDB_CATEGORY[genre]);
    if (p == NULL) {
        return ID3_UNKNOWN;
    } else {
        return (id3_genre) (*p);
    }
}

cddb_cat_t id3_to_cddb(id3_genre genre)
{
    cddb_cat_t g;

    switch (genre) {
        case ID3_FOLK:
        case ID3_FOLK_ROCK:
        case ID3_NATIONAL_FOLK:
            g = CDDB_CAT_FOLK;
            break;
        case ID3_JAZZ:
        case ID3_JAZZ_FUNK:
            g = CDDB_CAT_JAZZ;
            break;
        case ID3_ALTERNROCK:
        case ID3_CLASSIC_ROCK:
        case ID3_GOTHIC_ROCK:
        case ID3_GRUNGE:
        case ID3_HARD_ROCK:
        case ID3_INSTRUMENTAL_ROCK:
        case ID3_METAL:
        case ID3_PROGRESSIVE_ROCK:
        case ID3_PSYCHEDELIC_ROCK:
        case ID3_PUNK_ROCK:
        case ID3_ROCK:
        case ID3_ROCK_AND_ROLL:
        case ID3_SLOW_ROCK:
        case ID3_SOUTHERN_ROCK:
        case ID3_SYMPHONIC_ROCK:
            g = CDDB_CAT_JAZZ;
            break;
        case ID3_COUNTRY:
            g = CDDB_CAT_COUNTRY;
            break;
        case ID3_BLUES:
        case ID3_RYTHM_AND_BLUES:
            g = CDDB_CAT_BLUES;
            break;
        case ID3_NEW_AGE:
            g = CDDB_CAT_NEWAGE;
            break;
        case ID3_REGGAE:
            g = CDDB_CAT_REGGAE;
            break;
        case ID3_CLASSICAL:
        case ID3_SONATA:
        case ID3_SYMPHONY:
            g = CDDB_CAT_CLASSICAL;
            break;
        case ID3_SOUNDTRACK:
        case ID3_TRAILER:
            g = CDDB_CAT_SOUNDTRACK;
            break;
        default:
            g = CDDB_CAT_MISC;
    }
    return g;
}

GList *id3_get_list(void)
{
    if (id3_genre_list == NULL) {
        GList *list = NULL;
        int i;

        for (i=0; i<256; i++) {
            if (ID3_GENRE[i] != NULL) {
                list = g_list_append(list, (gpointer)ID3_GENRE[i]);
            }
        }
        id3_genre_list = list;
    }
    return id3_genre_list;
}
