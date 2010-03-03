METASOURCES=Auto
TOPDIR=.
INCLUDES = -I. \
	   -I../../min/src \
	   -I../../min/src/services/event_system \
	   -I../../min/src/tmc \
	   -I../../min/src/tmc/test_module_api \
	   -I../../min/src/utils/logger \
	   -I../../min/src/utils/dllist \
	   -I../../min/src/utils/lib_loader \
	   -I../../min/src/utils/ipc \
	   -I../../min/src/utils/text \
	   -I/usr/include/python$(PYTHON_VERSION)


pkglib_LTLIBRARIES 	= min_ext.la

min_ext_la_CFLAGS = $(AM_CFLAGS)
min_ext_la_LDFLAGS = -module -lpthread -lpython$(PYTHON_VERSION) 
min_ext_la_SOURCES = min_ext.c
min_ext_la_LIBADD = ../../min/src/utils/libminutils.la \
	    	    ../../min/src/tmc/test_module_api/libmintmapi.la \
		    ../../min/src/services/event_system/libminevent.la

