METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = TestClass.la
INCLUDES = -I. \
	   -I../../../min/src \
	   -I../../../min/src/services/event_system \
	   -I../../../min/src/tmc \
	   -I../../../min/src/tmc/test_module_api \
	   -I../../../min/src/utils/logger \
	   -I../../../min/src/utils/dllist \
	   -I../../../min/src/utils/ipc 

TestClass_la_LDFLAGS = -shared -module
TestClass_la_SOURCES = TestClass.c TestClassCases.c
TestClass_la_LIBADD = $(top_builddir)/../min/src/utils/libminutils.la \
		      $(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
		      $(top_builddir)/../min/src/services/event_system/libminevent.la

install-data-local:
	cp .libs/TestClass.so $(DESTDIR)/usr/share/min-tests
