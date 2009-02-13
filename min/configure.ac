AC_INIT(configure.ac)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(min, 0.1)
AM_PROG_CC_C_O
AC_LANG_C

AM_PROG_LIBTOOL

mintestmoddir=${libdir}/min
AC_SUBST(mintestmoddir)


LT_CURRENT=0
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_CONFIG_FILES(pkgconfig/min.pc pkgconfig/min-dev.pc)

AC_OUTPUT(\
	Makefile \
        src/Makefile \
        src/main/Makefile \
        src/consoleui/Makefile \
        src/ext_interface/Makefile \
        src/engine/Makefile \
        src/mintfwif/Makefile \
        src/engine/data_structures_api/Makefile \
        src/engine/treport_log/Makefile \
        src/tmc/Makefile \
        src/tmc/test_module_api/Makefile \
        src/utils/Makefile \
	src/utils/dllist/Makefile \
        src/utils/ipc/Makefile \
        src/utils/logger/Makefile \
        src/utils/parser/Makefile \
        src/utils/lib_loader/Makefile \
        tests/Makefile \
        mintests/Makefile \
        src/engine/tec/Makefile \
 	src/services/Makefile \
        src/services/event_system/Makefile \
        src/services/scripter_module/Makefile \
        src/services/test_interference/Makefile \
        src/test_libraries/Makefile \
        data/Makefile \
        pkgconfig/Makefile \
        src/utils/text/Makefile \
        src/utils/lua-5.1.3/Makefile \
        src/utils/lua-5.1.3/src/Makefile \
        src/services/lua_scripter_module/Makefile \
	src/in_plugins/Makefile \
	shared/Makefile \
	man/Makefile)

