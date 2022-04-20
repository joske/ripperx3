#ifndef ID3_GENRE_H
#define ID3_GENRE_H 1

#include <glib.h>
#include <cddb/cddb.h>


/**
 * Enumeration of all ID3 genres.
 */
typedef enum {
    ID3_BLUES = 0,              /*   0 */
    ID3_CLASSIC_ROCK,
    ID3_COUNTRY,
    ID3_DANCE,
    ID3_DISCO,
    ID3_FUNK,
    ID3_GRUNGE,
    ID3_HIP_HOP,
    ID3_JAZZ,
    ID3_METAL,
    ID3_NEW_AGE,                /*  10 */
    ID3_OLDIES,
    ID3_OTHER,
    ID3_POP,
    ID3_RYTHM_AND_BLUES,
    ID3_RAP,
    ID3_REGGAE,
    ID3_ROCK,
    ID3_TECHNO,
    ID3_INDUSTRIAL,
    ID3_ALTERNATIVE,            /*  20 */
    ID3_SKA,
    ID3_DEATH_METAL,
    ID3_PRANKS,
    ID3_SOUNDTRACK,
    ID3_EURO_TECHNO,
    ID3_AMBIENT,
    ID3_TRIP_HOP,
    ID3_VOCAL,
    ID3_JAZZ_FUNK,
    ID3_FUSION,                 /*  30 */
    ID3_TRANCE,
    ID3_CLASSICAL,
    ID3_INSTRUMENTAL,
    ID3_ACID,
    ID3_HOUSE,
    ID3_GAME,
    ID3_SOUND_CLIP,
    ID3_GOSPEL,
    ID3_NOISE,
    ID3_ALTERNROCK,             /*  40 */
    ID3_BASS,
    ID3_SOUL,
    ID3_PUNK,
    ID3_SPACE,
    ID3_MEDITATIVE,
    ID3_INSTRUMENTAL_POP,
    ID3_INSTRUMENTAL_ROCK,
    ID3_ETHNIC,
    ID3_GOTHIC,
    ID3_DARKWAVE,               /*  50 */
    ID3_TECHNO_INDUSTRIAL,
    ID3_ELECTRONIC,
    ID3_POP_FOLK,
    ID3_EURODANCE,
    ID3_DREAM,
    ID3_SOUTHERN_ROCK,
    ID3_COMEDY,
    ID3_CULT,
    ID3_GANGSTA,
    ID3_TOP_40,                 /*  60 */
    ID3_CHRISTIAN_RAP,
    ID3_POP_FUNK,
    ID3_JUNGLE,
    ID3_NATIVE_AMERICAN,
    ID3_CABARET,
    ID3_NEW_WAVE,
    ID3_PSYCHADELIC,
    ID3_RAVE,
    ID3_SHOWTUNES,
    ID3_TRAILER,                /*  70 */
    ID3_LO_FI,
    ID3_TRIBAL,
    ID3_ACID_PUNK,
    ID3_ACID_JAZZ,
    ID3_POLKA,
    ID3_RETRO,
    ID3_MUSICAL,
    ID3_ROCK_AND_ROLL,
    ID3_HARD_ROCK,
    ID3_FOLK = 80,              /*  80, Winamp extensions */
    ID3_FOLK_ROCK,
    ID3_NATIONAL_FOLK,
    ID3_SWING,
    ID3_FAST_FUSION,
    ID3_BEBOB,
    ID3_LATIN,
    ID3_REVIVAL,
    ID3_CELTIC,
    ID3_BLUEGRASS,
    ID3_AVANTGARDE,             /*  90 */
    ID3_GOTHIC_ROCK,
    ID3_PROGRESSIVE_ROCK,
    ID3_PSYCHEDELIC_ROCK,
    ID3_SYMPHONIC_ROCK,
    ID3_SLOW_ROCK,
    ID3_BIG_BAND,
    ID3_CHORUS,
    ID3_EASY_LISTENING,
    ID3_ACOUSTIC,
    ID3_HUMOUR,                 /* 100 */
    ID3_SPEECH,
    ID3_CHANSON,
    ID3_OPERA,
    ID3_CHAMBER_MUSIC,
    ID3_SONATA,
    ID3_SYMPHONY,
    ID3_BOOTY_BASS,
    ID3_PRIMUS,
    ID3_PORN_GROOVE,
    ID3_SATIRE,                 /* 110 */
    ID3_SLOW_JAM,
    ID3_CLUB,
    ID3_TANGO,
    ID3_SAMBA,
    ID3_FOLKLORE,
    ID3_BALLAD,
    ID3_POWER_BALLAD,
    ID3_RHYTHMIC_SOUL,
    ID3_FREESTYLE,
    ID3_DUET,                   /* 120 */
    ID3_PUNK_ROCK,
    ID3_DRUM_SOLO,
    ID3_A_CAPELLA,
    ID3_EURO_HOUSE,
    ID3_DANCE_HALL,             /* 125 */
    ID3_UNKNOWN = 255           /* 255 */
} id3_genre;

/**
 * String values for the CDDB categories.
 */
extern const char *ID3_GENRE[256];

/**
 * Map a CDDB genre string to an ID3 numeric value.
 */
id3_genre cddb_to_id3(cddb_cat_t genre);

/**
 * Map an ID3 numeric value to a CDDB genre string.  The caller should
 * free the string that is returned.
 */
cddb_cat_t id3_to_cddb(id3_genre genre);

/**
 * Returns a GList with the ID3 genre strings.
 */
GList *id3_get_list(void);


#endif /* ID3_GENRE_H */
