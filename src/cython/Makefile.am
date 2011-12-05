SUBDIRS = 
FILES = FAQ
EXTRA_DIST = $(FILES)

all:
	CFLAGS="-I../ -L../src/.libs" python setup.py build_ext

clean:
	@rm -f *.c
	@rm -rf build
