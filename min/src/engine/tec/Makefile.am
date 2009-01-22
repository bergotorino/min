INCLUDES = -I. -I../.. -I../../utils/dllist -I../../utils/ipc -I../../utils/logger -I../../utils/parser -I../../consoleui -I../data_structures_api -I../treport_log -I../../services/event_system -I../../services/scripter_module -I../../utils/text
METASOURCES = AUTO
noinst_LIBRARIES = libmintec.a  
noinst_LTLIBRARIES = libmintecext.la 
check_LIBRARIES = libmintectest.a
libmintec_a_SOURCES = tec.c tec_events.c tec_set_handling.c
libmintec_a_CFLAGS =  -DMIN_BIN_DIR='"$(bindir)"' \
		       -DMIN_CONF_DIR='"$(sysconfdir)"'
noinst_HEADERS =  tec_events.h 
include_HEADERS = tec.h tec_rcp_handling.h

libmintec_a_LIBADD = $(top_builddir)/src/utils/libminutils.la \
	              $(top_builddir)/src/engine/data_structures_api/libmindsa.la \
		      $(top_builddir)/src/engine/treport_log/libmintreport.la 

libmintectest_a_SOURCES = tec.c tec_events.c tec_set_handling.c tec.test tec_events.tests
libmintectest_a_CFLAGS = $(libmintec_a_CFLAGS) -DMIN_UNIT_TEST 
libmintectest_a_LIBADD = $(libmintec_a_LIBADD)

libmintecext_la_SOURCES = tec.c tec_events.c tec_set_handling.c tec_rcp_handling.c ../../services/scripter_module/scripter_keyword.c
libmintecext_la_CFLAGS = -DMIN_EXTIF -DMIN_CONF_DIR='"$(sysconfdir)"' \
			  -DMIN_BIN_DIR='"$(bindir)"'
libmintecext_la_LIBADD = $(libmintec_a_LIBADD)
