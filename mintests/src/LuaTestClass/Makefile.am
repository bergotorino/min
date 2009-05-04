METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = LuaTestClass.la

LuaTestClass_la_CFLAGS = -DLUA_USE_POSIX
LuaTestClass_la_LDFLAGS = -shared -module
LuaTestClass_la_SOURCES = LuaTestClass.c LuaTestClassCases.c
LuaTestClass_la_LIBADD = $(top_builddir)/../min/src/utils/libminutils.la \
		      $(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
		      $(top_builddir)/../min/src/services/event_system/libminevent.la

install-data-local:
	cp .libs/LuaTestClass.so $(DESTDIR)/usr/share/min-tests
