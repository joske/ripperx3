#include <cddb/cddb_disc.h>
#include <glib.h>
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

#include "cddb.h"
#include "ripperx.h"
#include "rx_cfg.h"
#include "main.h"
#include "config_handler.h"
#include "cddb_select_cd.h"

#define MAX_COMMAND_LENGTH 1024
#define CDDB_FAKE_NAME "EasyCD"

/* CDDB connection structure */
static cddb_conn_t *_cddb_conn = NULL;


int cddb_lookup(cddb_disc_t * disc)
{
    cddb_popup *popup;
    char *genre;
    int rc;

    dlog("cddb_lookup()\n");
    /* get possible matches */
    rc = cddb_query(_cddb_conn, disc);
    dlog("\tcddb_query: rc = %d\n", rc);
    const char *dartist = cddb_disc_get_artist(disc);
    const char* dtitle = cddb_disc_get_title(disc);
    cddb_cat_t category = cddb_disc_get_category(disc);
    unsigned int discid = cddb_disc_calc_discid(disc);
    switch (rc) {
        case -1:
            /* error occurred */
            dlog("\tCDDB query failed: %s\n", cddb_error_str(cddb_errno(_cddb_conn)));
            return 1;
        case 0:
            /* no matches found, just return */
            return 0;
        case 1:
            /* only one match, disc fields already filled in correctly */
            break;
        default:
            /* multiple matches, prompt user */
            popup = cddb_popup_create(NULL);
            cddb_popup_clear(popup);
            /* add first disc */
            cddb_popup_insert(popup, dartist, dtitle,
                              CDDB_CATEGORY[category], discid);
            /* add any additional discs */
            while ((rc = cddb_query_next(_cddb_conn, disc))) {
                cddb_popup_insert(popup, dartist, dtitle,
                                  CDDB_CATEGORY[category], discid);
            }
            /* query user */
            if (cddb_popup_query_user(popup)) {
                genre = cddb_popup_get_genre(popup);
                cddb_disc_set_genre(disc, genre);
                cddb_disc_set_discid(disc, cddb_popup_get_discid(popup));
                dlog("Selected: genre = '%s' (%d), disc ID = %8x\n",
                     genre, disc->category, disc->discid);
                g_free(genre);
            } else {
                dlog("Nothing selected\n");
                return 0;
            }
    }
    /* read data for 1 disc */
    rc = cddb_read(_cddb_conn, disc);
    dlog("\tcddb_read: rc = %d\n", rc);
    if (!rc) {
        dlog("\tCDDB read failed: %s\n", cddb_error_str(cddb_errno(_cddb_conn)));
        return 1;
    }
    dlog("lookup successful\n");
    return 0;
}

int cddb_submit(cddb_disc_t * disc)
{
    int rc;

    dlog("cddb_submit()\n");
    // XXX make this configurable
    //cddb_disc_set_category(disc, "misc");
    /* make sure we have the correct disc ID for this disc */
    cddb_disc_calc_discid(disc);
    rc = cddb_write(_cddb_conn, disc);
    if (!rc) {
        dlog("\tCDDB write failed: %s\n", cddb_error_str(cddb_errno(_cddb_conn)));
        return 1;
    }
    dlog("submit successful\n");
    return 0;
}

void init_cddb(void)
{
    rx_cfg *cfg;

    if (_cddb_conn == NULL) {
        _cddb_conn = cddb_new();
    }
    cfg = main_get_config();
    //cddb_set_email_address(_cddb_conn, "me@my.isp.com");
    cddb_set_server_name(_cddb_conn, rx_cfg_get_str(cfg, CFG_MAIN_CDDB_SERVER));
    cddb_set_server_port(_cddb_conn, rx_cfg_get_int(cfg, CFG_MAIN_CDDB_PORT));
    if (rx_cfg_get_bool(cfg, CFG_MAIN_CDDB_USE_HTTP)) {
        cddb_http_enable(_cddb_conn);
        cddb_set_http_path_query(_cddb_conn, rx_cfg_get_str(cfg, CFG_MAIN_CDDB_URL));
        cddb_set_http_path_submit(_cddb_conn, rx_cfg_get_str(cfg, CFG_MAIN_CDDB_URL_SUBMIT));
        if (rx_cfg_get_bool(cfg, CFG_MAIN_CDDB_USE_PROXY)) {
            cddb_http_proxy_enable(_cddb_conn);
            cddb_set_http_proxy_server_name(_cddb_conn,
                                            rx_cfg_get_str(cfg, CFG_MAIN_CDDB_PROXY));
            cddb_set_http_proxy_server_port(_cddb_conn,
                                            rx_cfg_get_int(cfg, CFG_MAIN_CDDB_PROXY_PORT));
        }
    }
}
