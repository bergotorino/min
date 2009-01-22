METASOURCES=Auto
TOPDIR=.
INCLUDES = -I$(TOPDIR)/src
pkglib_LTLIBRARIES 	= pythonscripter.la

pythonscripter_la_CFLAGS = $(AM_CFLAGS)
pythonscripter_la_LDFLAGS = -module  -lpthread -ldl -lutil -lm
pythonscripter_la_SOURCES = $(TOPDIR)/src/py_module.c $(TOPDIR)/src/py_moduleCases.c
pythonscripter_la_LIBADD = -lminutils -lmintmapi -lminevent


