
INCLUDES = -I..

AM_CFLAGS = @RIPPERX_CFLAGS@ -Wall -g
AM_LIBFLAGS = @RIPPERX_LIBS@ -module -avoid-version

lib_LTLIBRARIES = librxutil.la

libdir = $(pluginsdir)

librxutil_la_SOURCES =  rx_cfg.h rx_cfg.c \
                        rx_util.h rx_util.c \
                        rx_pixmap.h rx_pixmap.c \
                        id3.h id3.c 

librxutil_la_LIBADD = @RIPPERX_LIBS@
