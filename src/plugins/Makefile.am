INCLUDES = -I.. -I../lib

AM_CFLAGS = @RIPPERX_CFLAGS@ -Wall -g

AM_LIBFLAGS=@RIPPERX_LIBS@ -module -avoid-version
lib_LTLIBRARIES = libcdparanoia.la libgogo.la liblame.la liboggenc.la

libdir = $(pluginsdir)

libcdparanoia_la_SOURCES = cdparanoia.c
libgogo_la_SOURCES = gogo.c
liblame_la_SOURCES = lame.c
liboggenc_la_SOURCES = oggenc.c

libcdparanoia_la_LIBADD = @RIPPERX_LIBS@ -L../lib -lrxutil
libgogo_la_LIBADD = @RIPPERX_LIBS@ -L../lib -lrxutil
liblame_la_LIBADD = @RIPPERX_LIBS@ -L../lib -lrxutil
liboggenc_la_LIBADD = @RIPPERX_LIBS@ -L../lib -lrxutil

