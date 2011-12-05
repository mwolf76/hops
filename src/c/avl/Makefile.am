AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = -I$(top_srcdir)/src

PKG_H = avl.h
PKG_C = avl.c

PKG_SOURCES = $(PKG_H) $(PKG_C)

utilsdir = $(includedir)/nusmv/utils
nobase_utils_HEADERS = $(PKG_H)


# -------------------------------------------------------

noinst_LTLIBRARIES = libavl.la
libavl_la_SOURCES = $(PKG_SOURCES)

