METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = EventsTestModule.la
INCLUDES = -I. \
	   -I../../../min/src \
	   -I../../../min/src/services/event_system \
	   -I../../../min/src/tmc \
	   -I../../../min/src/tmc/test_module_api \
	   -I../../../min/src/utils/logger \
	   -I../../../min/src/utils/dllist \
	   -I../../../min/src/utils/ipc \
	   -I../../../min/src/utils/lib_loader \
	   -I../../../min/src/utils/text \
	   -I../../../min/src/utils/parser \
	   -I../../../min/shared

EventsTestModule_la_LDFLAGS = -module -shared
EventsTestModule_la_SOURCES = events.c eventsCases.c 
EventsTestModule_la_LIBADD  = $(top_builddir)/../min/src/utils/libminutils.la \
	    		      $(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
			      $(top_builddir)/../min/src/services/event_system/libminevent.la

install-data-local:
	chrpath -d .libs/EventsTestModule.so
	cp .libs/EventsTestModule.so $(DESTDIR)/usr/lib/min-tests

