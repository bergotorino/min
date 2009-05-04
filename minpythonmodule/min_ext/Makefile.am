METASOURCES=Auto
TOPDIR=.
INCLUDES = -I.
pkglib_LTLIBRARIES 	= min_ext.la

min_ext_la_CFLAGS = $(AM_CFLAGS)
min_ext_la_LDFLAGS = -module -lpthread -lpython2.5 
min_ext_la_SOURCES = min_ext.c
min_ext_LDADD = $(top_builddir)/src/utils/libminutils.la \
	    	$(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
		$(top_builddir)/src/services/eventsysten/libminevent.la

