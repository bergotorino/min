METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = EventsTestModule.la

EventsTestModule_la_LDFLAGS = -module -shared
EventsTestModule_la_SOURCES = events.c eventsCases.c 
EventsTestModule_la_LIBADD  = -lminutils -lmintmapi -lminevent

install-data-local:
	cp .libs/EventsTestModule.so $(DESTDIR)/usr/share/min-tests

