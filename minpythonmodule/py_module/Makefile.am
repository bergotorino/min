METASOURCES=Auto
TOPDIR=.
INCLUDES = -I. \
	   -I ./src \
	   -I../../min/src \
	   -I../../min/src/services/event_system \
	   -I../../min/src/tmc \
	   -I../../min/src/tmc/test_module_api \
	   -I../../min/src/utils/logger \
	   -I../../min/src/utils/parser \
	   -I../../min/src/utils/dllist \
	   -I../../min/src/utils/lib_loader \
	   -I../../min/src/utils/ipc \
	   -I../../min/src/utils/text

pkglib_LTLIBRARIES 	= pythonscripter.la

pythonscripter_la_CFLAGS = $(AM_CFLAGS) $(PYTHON_CFLAGS)
pythonscripter_la_LDFLAGS = -module  -lpthread -ldl -lutil -lm $(PYTHON_LIBS)
pythonscripter_la_SOURCES = $(TOPDIR)/src/py_module.c $(TOPDIR)/src/py_moduleCases.c
pythonscripter_la_LIBADD =	$(top_builddir)/../min/src/utils/libminutils.la \
	    		   	$(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
			  	$(top_builddir)/../min/src/services/event_system/libminevent.la

