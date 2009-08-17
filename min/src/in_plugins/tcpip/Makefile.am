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

pkglib_LTLIBRARIES = min_tcpip.la
min_tcpip_la_SOURCES = tcpip_plugin.c
min_tcpip_la_LDFLAGS = -module
min_tcpip_la_CFLAGS = $(AM_CFLAGS)
