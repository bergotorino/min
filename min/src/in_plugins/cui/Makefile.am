METASOURCES = AUTO
INCLUDES = -I. -I../ -I../.. -I../../utils/dllist \
		-I../../utils/logger -I../../utils/parser \
		-I../../utils/ipc -I../../services/event_system \
		-I../../engine/tec -I../../engine/data_structures_api \
		-I../../utils/text

pkglib_LTLIBRARIES = min_cui.la
include_HEADERS = callback.h consoleui.h
min_cui_la_SOURCES = callback.c consoleui.c
min_cui_la_LIBADD = $(top_builddir)/src/engine/data_structures_api/libmindsa.la
min_cui_la_LDFLAGS = -module
min_cui_la_CFLAGS = $(AM_CFLAGS)

