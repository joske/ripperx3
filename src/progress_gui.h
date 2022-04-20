#ifndef PROGRESS_GUI_H
#define PROGRESS_GUI_H 1

#include <time.h>

typedef struct _time_stat time_stat;

struct _time_stat {
    time_t start;
    time_t now;
    time_t rip_start;
    time_t enc_start;
    long remain;                /* estimated remaining time */
    long total_time;            /* sum of all track lengths */
    long work_time;             /* estimated total working time */
    long seconds_ripped;
    long seconds_encoded;
    int todo;                   /* no. of tracks still in queue */
    int done;                   /* no. of tracks already processed */
    /* the following is for current track */
    int rip_nr;
    int enc_nr;
    int rip_progress;
    int enc_progress;
};

void show_progress(void);
void update_progress(time_stat * stats);
void hide_progress(void);

#endif
