AC_INIT(configure.ac)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(min, 0.1)
AM_PROG_CC_C_O
AC_LANG_C

AM_PROG_LIBTOOL

mintestmoddir=${libdir}/min
AC_SUBST(mintestmoddir)
PKG_CHECK_MODULES([GLIB],[glib-2.0])
PKG_CHECK_MODULES([DBUS_GLIB],[dbus-glib-1])
PKG_CHECK_MODULES([GOBJECT],[gobject-2.0])

LT_CURRENT=0
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)


AC_OUTPUT(\
	Makefile \
        src/Makefile \
        data/Makefile)

