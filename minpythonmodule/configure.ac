AC_INIT(configure.ac)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(min, 0.1)
AM_PROG_CC_C_O
AC_LANG_C

AM_PROG_LIBTOOL
AM_PATH_PYTHON
mintestmoddir=${libdir}/min
AC_SUBST(mintestmoddir)
if  test -f /usr/include/python2.6/Python.h; then 
pythoncheckhack="python2.6"
else
pythoncheckhack="python2.5"
fi
AC_SUBST(pythoncheckhack)


LT_CURRENT=0
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)


AC_OUTPUT(\
	Makefile \
        min_ext/Makefile \
        py_module/Makefile)

