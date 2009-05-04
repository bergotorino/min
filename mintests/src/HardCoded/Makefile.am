METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = HardCoded.la

HardCoded_la_LDFLAGS = -shared -module
HardCoded_la_SOURCES = HardCoded.c HardCodedCases.c
HardCoded_la_LIBADD = $(top_builddir)/../min/src/utils/libminutils.la \
		      $(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
		      $(top_builddir)/../min/src/services/event_system/libminevent.la

install-data-local:
	cp .libs/HardCoded.so $(DESTDIR)/usr/share/min-tests

