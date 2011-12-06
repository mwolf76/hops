AUTOMAKE_OPTIONS = subdir-objects
INCLUDES = -I$(top_srcdir)/src/c/

PKG_H = avl.h
PKG_C = avl.c

PKG_SOURCES = $(PKG_H) $(PKG_C)

# -------------------------------------------------------

noinst_LTLIBRARIES = libavl.la
libavl_la_SOURCES = $(PKG_SOURCES)

