ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src

PACKAGE = $(PACKAGE_NAME)
VERSION = $(PACKAGE_VERSION)

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" @CPPFLAGS@
READMES = LGPL-2.1 NEWS README

pkgconfdir = $(libdir)/pkgconfig
pkgconf_DATA = hops.pc

# Test suite
test:
	cd $(top_builddir)/test && $(MAKE)
