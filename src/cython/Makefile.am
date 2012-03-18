SUBDIRS =
FILES =
EXTRA_DIST = $(FILES)

all:
	CFLAGS="-I$(top_srcdir)/src/c/ 	 	\
	-L$(top_srcdir)/src/c/avl/.libs/ 	\
	-L$(top_srcdir)/src/c/array/.libs/ 	\
	-L$(top_srcdir)/src/c/ht/.libs/" 	\
	python setup.py build_ext

clean:
	@rm -f *.c
	@rm -rf build
