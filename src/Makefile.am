
SUBDIRS=lib plugins

INCLUDES = -Ilib

AM_CFLAGS = @RIPPERX_CFLAGS@

bin_PROGRAMS = ripperX

ripperX_SOURCES = \
	main.c\
	main.h\
	ripper_plugin.h\
	ripperx.h\
	plugin_handling.c\
	plugin_handling.h\
	track_view.c\
	track_view.h\
	rx_main.c\
	rx_main.h\
	rx_track_view.c\
	rx_track_view.h\
	rx_progress.c\
	rx_progress.h\
    config_handler.c\
    config_handler.h\
    config_defines.h\
    misc_util.c\
    misc_util.h\
    progress_gui.h\
    progress_gui.c\
    cddb.c\
    cddb.h\
    cddb_select_cd.c\
    cddb_select_cd.h\
    job.h\
    job.c\
    id3_genre.h\
    id3_genre.c\
	debug.h

ripperX_LDFLAGS = @RIPPERX_LIBS@ -Llib -lrxutil
