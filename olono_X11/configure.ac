dnl Process this file with autoconf to produce a configure script.

AC_INIT([OloNO], [0.4], [ail@isep.fr])

dnl Definition
sinclude(ac-helpers/ac_define_dir.m4)

dnl Non GNU, but very handy :
AC_DEFINE_DIR([LIBDIR],     [libdir],     [Where the libs are placed to.])
AC_DEFINE_DIR([DATADIR],    [datadir],    [Where data are placed to.])
AC_DEFINE_DIR([INCLUDEDIR], [includedir], [Where headers are placed to.])
AC_DEFINE_DIR([BINDIR],     [bindir],     [Where bin are places to.])

AC_CONFIG_HEADERS([config.h])
###AC_CONFIG_LIBOBJ_DIR([src])

###AC_LIBOBJ([libIA])
###AC_LIBOBJ([libPlay])
###AC_LIBOBJ([libDisplay])
###AC_LIBOBJ([libCases])

PACKAGE=olono
VERSION=0.4

AM_INIT_AUTOMAKE($PACKAGE,$VERSION)

AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB      dnl This is required if any libraries are built in the package.

AC_STDC_HEADERS

PKG_CHECK_MODULES(DEPS, libglade-2.0 libgnomeui-2.0 >= 1.110.0)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AM_CONDITIONAL(DEBUG, test "$enable_debug" = yes)

### Attention : libs before the rest
AC_CONFIG_FILES([
Makefile
src/Makefile 
imgs/Makefile
glade/Makefile
])
AC_OUTPUT
