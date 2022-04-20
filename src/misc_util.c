#include <string.h>
#include <stdio.h>

#include "ripperx.h"
#include "misc_util.h"

gchar *sector2time(long length)
{
    int min, sec, tmp;
    gchar *s;

    s = (gchar *) g_malloc(6);
    tmp = length / SECTORS_PER_SECOND;
    min = tmp / 60;
    sec = tmp % 60;
    sprintf(s, "%02d:%02d", min, sec);
    return s;
}

void seconds2time(gchar buf[], int len, int seconds)
{
    int min;
    int sec;

    min = seconds / 60;
    sec = seconds % 60;

    g_snprintf(buf, len, "%02d:%02d", min, sec);
}
