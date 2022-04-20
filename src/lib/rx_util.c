#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "rx_util.h"
#include "config_defines.h"

static gchar *parse_template(gchar * format, cddb_disc_t * disc, cddb_track_t * track);

gchar *get_dest_file(rx_cfg * config, cddb_disc_t * disc, cddb_track_t * track)
{
    gchar *enc_path = NULL;
    gchar *dest = NULL;
    gchar *dirname = NULL;
    gchar *filename = NULL;
    gchar *dformat = NULL;
    gchar *fformat = NULL;
    gboolean ad;

    ad = rx_cfg_get_bool(config, CFG_MAIN_DIR_ALBUM);
    dformat = rx_cfg_get_str(config, CFG_MAIN_DIR_TEMPLATE);
    dirname = parse_template(dformat, disc, track);
    fformat = rx_cfg_get_str(config, CFG_MAIN_FILE_TEMPLATE);
    filename = parse_template(fformat, disc, track);
    enc_path = rx_cfg_get_str(config, CFG_MAIN_ENC_PATH);
    if (track) {
        dest = g_strdup_printf("%s/%s/%s.mp3", enc_path, (ad ? dirname : ""), filename);
    } else {                    //only dir part requested
        dest = g_strdup_printf("%s/%s", enc_path, (ad ? dirname : ""));
    }
    g_free(dirname);
    g_free(filename);
    return dest;
}

gchar *get_source_file(rx_cfg * config, cddb_disc_t * disc, cddb_track_t * track)
{
    gchar *src_path = NULL;
    gchar *source = NULL;
    gchar *dirname = NULL;
    gchar *filename = NULL;
    gchar *dformat = NULL;
    gchar *fformat = NULL;
    gboolean ad;

    ad = rx_cfg_get_bool(config, CFG_MAIN_DIR_ALBUM);
    dformat = rx_cfg_get_str(config, CFG_MAIN_DIR_TEMPLATE);
    dirname = parse_template(dformat, disc, track);
    fformat = rx_cfg_get_str(config, CFG_MAIN_FILE_TEMPLATE);
    filename = parse_template(fformat, disc, track);
    src_path = rx_cfg_get_str(config, CFG_MAIN_SRC_PATH);
    if (track) {
        source = g_strdup_printf("%s/%s/%s.wav", src_path, (ad ? dirname : ""), filename);
    } else {                    //only dir part requested
        source = g_strdup_printf("%s/%s", src_path, (ad ? dirname : ""));
    }
    g_free(dirname);
    g_free(filename);
    return source;
}

gchar *locate_exe(gchar * exe)
{
    FILE *p = NULL;
    char cmd[256];
    char buffer[1024];
    gchar *path = NULL;

    snprintf(cmd, 255, "which %s", exe);
    p = popen(cmd, "r");
    fgets(buffer, 1023, p);
    if (strncmp(buffer, "which: no", 9) != 0) {
        int len = strlen(buffer);

        path = g_malloc(len);
        sscanf(buffer, "%s", path);
    }
    pclose(p);
    return path;
}

int check_dir(char *dir)
{
    int rc;
    struct stat ds;

    rc = access(dir, F_OK);
    if (rc != 0) {
        return RX_DIR_DOES_NOT_EXIST;
    }
    rc = stat(dir, &ds);
    if (rc != 0) {
        return RX_DIR_NOT_DIR;
    }
    if (!S_ISDIR(ds.st_mode)) {
        return RX_DIR_NOT_DIR;
    }
    rc = access(dir, X_OK | W_OK | R_OK);
    if (rc != 0) {
        return RX_DIR_NOT_WRITABLE;
    }
    return RX_DIR_OK;
}

int create_basedir(gchar * path)
{
    char *parent;
    char *tmp;
    int rc;

    tmp = strdup(path);         /*preserve our string, it gets modified by dirname */
    parent = dirname(tmp);
    rc = check_dir(parent);
    if (strcmp(parent, "/") == 0) {
        return -1;              /* stop endless loop */
    }
    if (rc == RX_DIR_NOT_DIR || rc == RX_DIR_NOT_WRITABLE) {
        return -1;
    } else if (rc == RX_DIR_DOES_NOT_EXIST) {
        /* recursively create */
        if (create_basedir(parent) != 0) {
            return -1;
        }
    }
    rc = mkdir(path, 0755);
    free(tmp);
    return rc;
}

static gchar *parse_template(gchar * format, cddb_disc_t * disc, cddb_track_t * track)
{
    gchar buffer[MAX_FILE_LENGTH];
    int i = 0;
    int len = 0;

    memset(buffer, 0, MAX_FILE_LENGTH);
    while (format[i] != '\0') {
        if (format[i] == '%' && format[i + 1]) {
            gchar c;

            c = format[i + 1];
            switch (c) {
                case '#':
                    if (track) {
                        len += 2;
                        g_snprintf(buffer, MAX_FILE_LENGTH - len, "%s%02d",
                                   buffer, cddb_track_get_number(track));
                    }
                    break;
                case 'a':
                  const char *artist = cddb_track_get_artist(track);
                  const char *dartist = cddb_disc_get_artist(disc);
                  if (track && artist) {
                    len += strlen(artist);
                    g_snprintf(buffer, MAX_FILE_LENGTH - len, "%s%s", buffer,
                               artist);
                  } else if (disc && dartist) {
                    len += strlen(dartist);
                    g_snprintf(buffer, MAX_FILE_LENGTH - len, "%s%s", buffer,
                               dartist);
                  }
                    break;
                case 't':
                    const char* title = cddb_track_get_title(track);
                    if (track && title) {
                        len += strlen(title);
                        g_snprintf(buffer, MAX_FILE_LENGTH - len, "%s%s", buffer, title);
                    }
                    break;
                case 'v':
                    const char* dtitle = cddb_disc_get_title(disc);
                    if (disc && dtitle) {
                        len += strlen(dtitle);
                        g_snprintf(buffer, MAX_FILE_LENGTH - len, "%s%s", buffer, dtitle);
                    }
                    break;
            }
            i += 2;             //eat 2 chars of template input
        } else {                // not a template char
            len += 1;
            g_snprintf(buffer, MAX_FILE_LENGTH - len, "%s%c", buffer, format[i++]);
        }
    }
    return g_strdup(buffer);
}

/* escape the filename before using it in a popen() call.
 * since the filenames are enclosed in single quotes, that's 
 * all we have to escape
 */
gchar *escape(gchar * orig)
{
    gchar *buffer, *p;
    int i, j, len;

    len = strlen(orig);
    p = orig;
    while (*p) {
        if (*p == '\'') {
            len += 4;
        }
        p++;
    }
    buffer = malloc(len + 1);
    for (i = 0, j = 0; i < strlen(orig); i++) {
        if (orig[i] == '\'') {
            buffer[j++] = '\'';
            buffer[j++] = '"';
            buffer[j++] = '\'';
            buffer[j++] = '"';
            buffer[j++] = '\'';
        } else {
            buffer[j++] = orig[i];
        }
    }
    buffer[j] = '\0';
    return buffer;
}
