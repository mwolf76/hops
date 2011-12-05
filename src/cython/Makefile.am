SUBDIRS = 
FILES = FAQ
EXTRA_DIST = $(FILES)

all:
	CFLAGS="-I$(top_srcdir)/src/c/ -L$(top_srcdir)/src/c/avl/.libs/" python setup.py build_ext

clean:
	@rm -f *.c
	@rm -rf build
