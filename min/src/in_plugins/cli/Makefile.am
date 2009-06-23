METASOURCES = AUTO
INCLUDES = -I. 	-I../ -I../.. \
	       	-I../../utils/dllist \
		-I../../utils/logger \
                -I../../utils/parser \
		-I../../utils/ipc \
                -I../../utils/lib_loader \
	        -I../../tmc \
	        -I../../services/event_system \
		-I../../engine/tec \
		-I../../engine/data_structures_api \
		-I../../utils/text

pkglib_LTLIBRARIES = min_cli.la
min_cli_la_SOURCES = cli.c
min_cli_la_LIBADD = $(top_builddir)/src/engine/data_structures_api/libmindsa.la
min_cli_la_LDFLAGS = -module
min_cli_la_CFLAGS = $(AM_CFLAGS)
