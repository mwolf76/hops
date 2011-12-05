AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = -I$(top_srcdir)/src

PKG_H = array.h
PKG_C = array.c

PKG_SOURCES = $(PKG_H) $(PKG_C)

# -------------------------------------------------------

noinst_LTLIBRARIES = libarray.la
libarray_la_SOURCES = $(PKG_SOURCES)

