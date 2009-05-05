METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = LuaTestClass.la
INCLUDES = -I. \
	   -I../../../min/src \
	   -I../../../min/src/services/event_system \
	   -I../../../min/src/tmc \
	   -I../../../min/src/tmc/test_module_api \
	   -I../../../min/src/utils/logger \
	   -I../../../min/src/utils/dllist \
	   -I../../../min/src/utils/lib_loader \
	   -I../../../min/src/utils/ipc \ 
	   -I../../../min/src/utils/parser \
	   -I../../../min/src/utils/text 

LuaTestClass_la_CFLAGS = -DLUA_USE_POSIX
LuaTestClass_la_LDFLAGS = -shared -module
LuaTestClass_la_SOURCES = LuaTestClass.c LuaTestClassCases.c
LuaTestClass_la_LIBADD = $(top_builddir)/../min/src/utils/libminutils.la \
		      $(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
		      $(top_builddir)/../min/src/services/event_system/libminevent.la

install-data-local:
	cp .libs/LuaTestClass.so $(DESTDIR)/usr/share/min-tests
