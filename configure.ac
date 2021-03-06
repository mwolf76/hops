# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)

AC_INIT(hops, 0.0.1, marco.pensallorto@gmail.com)

# where to look for install.sh and other helpers
AC_CONFIG_AUX_DIR(helpers)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.9 tar-ustar no-define])

AC_CANONICAL_HOST

AM_CONFIG_HEADER([config.h])
AC_DEFINE_UNQUOTED(PACKAGE_BUILD_DATE, "`date -u`", Build date)

# Library informations
AC_DEFINE_UNQUOTED(LIBRARY_NAME, "hops", Library Name)
AC_DEFINE_UNQUOTED(LIBRARY_VERSION, "0.0.1", Library Version)
AC_DEFINE_UNQUOTED(LIBRARY_BUILD_DATE, "`date -u`", Library build date)
AC_DEFINE_UNQUOTED(LIBRARY_WEBSITE, "https://github.com/mwolf76", Library Website)
AC_DEFINE_UNQUOTED(LIBRARY_EMAIL, "marco.pensallorto@gmail.com", Library Email)
AC_DEFINE_UNQUOTED(LIBRARY_BUGREPORT, "Please report bugs to <marco.pensallorto@gmail.com>", Library bug message)

# Checks for programs.
AC_MSG_NOTICE([Checking for Python])
have_python=no

AM_PATH_PYTHON(2.4,[],[AC_MSG_WARN([Python not found])])
if test -z "$PYTHON" || test "$PYTHON" = : ; then
    have_python=no
else
    have_python=yes
fi

AC_MSG_CHECKING([for Cython >= 0.11.3])
CYTHON_CHECK_VERSION(0.11.3,  [AC_MSG_RESULT(yes)
			      have_cython=yes],
			      [AC_MSG_RESULT(no)
			      have_cython=no])

oldcflags=$CFLAGS
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
CFLAGS="-Wall -Wstrict-prototypes -O0 -g -fomit-frame-pointer -funroll-loops $excflags$oldcflags"

if test "x$ac_cv_prog_CPP" != "x"; then
AC_DEFINE(HAVE_CPP, 1, Have preprocessor)
AC_DEFINE_UNQUOTED(PROG_CPP, "$ac_cv_prog_CPP -x c", Preprocessor call string)
else
AC_DEFINE(HAVE_CPP, 0, Have preprocessor)
fi

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# portability issues
AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])


# Checks for libraries.
AC_CHECK_LIB([m], [exp])

AM_CONDITIONAL(HAVE_LIBEXPAT, test "x$ac_have_expat" = "xyes")
AC_SUBST(HAVE_LIBEXPAT)

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h memory.h stddef.h stdlib.h string.h \
  sys/ioctl.h sys/param.h sys/time.h sys/resource.h unistd.h signal.h sys/signal.h \
  errno.h regex.h inttypes.h])

# This is for malloc:
AC_CHECK_HEADER(sys/types.h)
AC_CHECK_HEADER(malloc.h,
	[ AC_DEFINE(HAVE_MALLOC_H, 1, Defined to 1 if the system provides malloc.h)],
	[ AC_CHECK_HEADER(sys/malloc.h,
	    [AC_DEFINE(HAVE_SYS_MALLOC_H, 1, Defined to 1 if the system provides sys/malloc.h)],
	    [],
	    [
	      #if HAVE_SYS_TYPES_H
	      # include <sys/types.h>
	      #endif
	    ]
	    )],
	[
	 #if HAVE_SYS_TYPES_H
	 # include <sys/types.h>
	 #endif
	])


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([floor memmove memset pow strcasecmp strchr \
		strrchr strstr strtol, random srandom getpid \
		mkstemp mktemp tmpnam getenv setvbuf system popen isatty])

AC_CONFIG_FILES([Makefile
		 src/Makefile
                 src/c/Makefile
		 src/c/avl/Makefile
                 src/c/ht/Makefile
		 src/c/array/Makefile
		 src/cython/Makefile
                 test/Makefile
		 hops.pc])

AC_OUTPUT

echo

dnl if  [ test "x$ac_have_minisat" = "xno" ] && [ test "x$ac_have_zchaff" = "xno" ]; then
dnl echo ------------------------------------------------------------------
dnl echo "WARNING: No SAT solver will be linked to NuSMV."
dnl echo "WARNING: All NuSMV commands and options that depend on a SAT "
dnl echo "WARNING: solver will not be available."
dnl echo "WARNING: If you want to use SAT solver NuSMV features, please "
dnl echo "WARNING: compile and link the zchaff SAT solver or the MiniSat"
dnl echo "WARNING: SAT solver. "
dnl echo "WARNING: For further help, try \"./configure --help\"."
dnl echo ------------------------------------------------------------------
dnl fi
