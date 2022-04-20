#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib/gthread.h>
#include <string.h>

#include "ripperx.h"
#include "main.h"
#include "plugin_handling.h"
#include "config_handler.h"
#include "rx_cfg.h"
#include "rx_pixmap.h"
#include "cddb.h"

static cddb_disc_t *disc = NULL;
static GtkWidget *main_window;

int main(int argc, char *argv[])
{
    rx_cfg *config;

#ifdef ENABLE_NLS
    bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
    textdomain(PACKAGE);
#endif

    g_thread_init(NULL);
    gdk_threads_init();
    gtk_set_locale();
    gtk_init(&argc, &argv);

    add_pixmap_directory(PACKAGE_DATA_DIR "/pixmaps");
    add_pixmap_directory(PACKAGE_SOURCE_DIR "/pixmaps");

    init_plugins();
    init_config();
    init_cddb();
    config = get_configuration();
    main_window = GTK_WIDGET(rx_main_new(rx_cfg_get_bool(config, CFG_MAIN_ENCODE)));
    set_ripper(rx_cfg_get_str(config, CFG_MAIN_RIPPER));
    set_encoder(rx_cfg_get_str(config, CFG_MAIN_ENCODER));
    gtk_widget_show(main_window);

    gtk_main();
    return 0;
}

void main_config(void)
{
    show_config();
}

void main_scan(void)
{
    dlog("main_scan() called\n");
    if (disc != NULL) {
        cddb_disc_destroy(disc);
    }
    disc = cddb_disc_new();
    if (ripper_scan_cd(disc) > 0) {
        track_view_init();
    }
}

void main_cddb(void)
{
    cddb_lookup(disc);
    update_view();
}

void main_cddb_submit(void)
{
    cddb_submit(disc);
}

void main_go(void)
{
    start_jobs();
}

void main_exit(void)
{
    rx_cfg *config;

    config = get_configuration();
    // First write the configuration
    save_config(config);
    // Then do some cleaning up
    clean_up_plugins();
    dlog("Free main configuration.\n");
    rx_cfg_free(config);
    gtk_main_quit();
}

void main_must_encode(gboolean encode)
{
    rx_cfg *config;

    config = get_configuration();
    rx_cfg_upd_bool(config, CFG_MAIN_ENCODE, encode);
}

void main_set_track_view(GtkWidget * tv)
{
    rx_main_set_track_view(main_window, tv);
}

rx_cfg *main_get_config()
{
    rx_cfg *config;

    config = get_configuration();
    return config;
}

cddb_disc_t *main_get_disc()
{
    return disc;
}
