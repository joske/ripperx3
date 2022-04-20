#ifndef TRACK_VIEW_H
#define TRACK_VIEW_H
#include "rx_track_view.h"

void track_view_init(void);
void update_view(void);
void update_track(tv_track * track);
void update_artist(gchar * artist);
void update_album(gchar * album);
void select_all(gboolean on);
gboolean is_track_selected(int idx);

#endif
