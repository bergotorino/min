METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = TestClass.la

TestClass_la_LDFLAGS = -shared -module
TestClass_la_SOURCES = TestClass.c TestClassCases.c
TestClass_la_LIBADD = $(top_builddir)/../min/src/utils/libminutils.la \
		      $(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
		      $(top_builddir)/../min/src/services/event_system/libminevent.la

install-data-local:
	cp .libs/TestClass.so $(DESTDIR)/usr/share/min-tests
