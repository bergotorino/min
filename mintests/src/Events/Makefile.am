METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = EventsTestModule.la

EventsTestModule_la_LDFLAGS = -module -shared
EventsTestModule_la_SOURCES = events.c eventsCases.c 
EventsTestModule_la_LIBADD  = $(top_builddir)/../min/src/utils/libminutils.la \
	    		      $(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
			      $(top_builddir)/../min/src/services/event_system/libminevent.la

install-data-local:
	cp .libs/EventsTestModule.so $(DESTDIR)/usr/share/min-tests

