INCLUDES = 	-I. -I../.. -I../../utils/dllist  \
		-I../../utils/ipc -I../../utils/logger \
		-I../../utils/parser -I../../utils/settings \
	        -I../../in_plugins/cui \
		-I../data_structures_api \
		-I../../services/event_system \
		-I../../services/scripter_module -I../../utils/text \
		-I../../mintfwif -I../../tmc
METASOURCES = AUTO
noinst_LIBRARIES = libmintec.a
noinst_LTLIBRARIES = libmintecext.la 
check_LIBRARIES = libmintectest.a
libmintec_a_SOURCES = tec.c \
		      tec_events.c \
		      tec_rcp_handling.c \
		      ../../services/scripter_module/scripter_keyword.c \
		      min_engine_api.c tec_tcp_handling.c \
		      ../../mintfwif/mintfwif.c
libmintec_a_CFLAGS =  -DMIN_BIN_DIR='"$(bindir)"' \
		       -DMIN_CONF_DIR='"$(sysconfdir)"'
noinst_HEADERS =  tec_events.h 
include_HEADERS = tec.h tec_rcp_handling.h min_engine_api.h

libmintec_a_LIBADD = $(top_builddir)/src/utils/libminutils.la \
	              $(top_builddir)/src/engine/data_structures_api/libmindsa.la 

libmintectest_a_SOURCES = tec.c \
			  tec_events.c \
			  tec_rcp_handling.c \
			  tec_tcp_handling.c \
			  tec.test \
			  tec_events.tests \
			  min_engine_api.c \
			  ../../mintfwif/mintfwif.c \
			  ../../services/scripter_module/scripter_keyword.c

libmintectest_a_CFLAGS = $(libmintec_a_CFLAGS) -DMIN_UNIT_TEST 
libmintectest_a_LIBADD = $(libmintec_a_LIBADD)

libmintecext_la_SOURCES = tec.c \
			  tec_events.c \
			  tec_rcp_handling.c \
			  ../../services/scripter_module/scripter_keyword.c \
			  min_engine_api.c 

libmintecext_la_CFLAGS = -DMIN_EXTIF -DMIN_CONF_DIR='"$(sysconfdir)"' \
			  -DMIN_BIN_DIR='"$(bindir)"'
libmintecext_la_LIBADD = $(libmintec_a_LIBADD)
