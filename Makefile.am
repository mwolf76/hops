ACLOCAL_AMFLAGS = -I m4

PACKAGE = $(PACKAGE_NAME)
VERSION = $(PACKAGE_VERSION)

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" @CPPFLAGS@
READMES = LGPL-2.1 NEWS README

pkgconfdir = $(libdir)/pkgconfig
pkgconf_DATA = hops.pc

all:
	cd $(top_builddir)/src && $(MAKE)
	find . -name "*.so" -exec cp {} $(top_builddir)/lib \;

clean:
	cd $(top_builddir)/src && $(MAKE) clean
	rm -f lib/*

# Test suite
test:
	cd $(top_builddir)/test && $(MAKE)
