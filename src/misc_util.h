#ifndef MISC_UTIL_H
#define MISC_UTIL_H 1

#include <glib.h>

gchar *sector2time(long length);
void seconds2time(gchar buf[], int len, int seconds);

#endif
