AUTOMAKE_OPTIONS = subdir-objects
INCLUDES = -I$(top_srcdir)/src/c/

PKG_H = ht.h
PKG_C = ht.c

PKG_SOURCES = $(PKG_H) $(PKG_C)

# -------------------------------------------------------

noinst_LTLIBRARIES = libht.la
libht_la_SOURCES = $(PKG_SOURCES)
