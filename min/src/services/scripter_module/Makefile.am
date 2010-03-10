METASOURCES = AUTO
INCLUDES = -I. -I.. -I../.. -I$(top_srcdir)/. \
	-I$(top_srcdir)/conf \
	-I$(top_srcdir)/integration \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/ext_interface \
	-I$(top_srcdir)/src/consoleui \
	-I$(top_srcdir)/src/engine \
	-I$(top_srcdir)/src/engine/data_structures_api \
	-I$(top_srcdir)/src/engine/tec \
	-I$(top_srcdir)/src/engine/treport_log \
	-I$(top_srcdir)/src/services \
	-I$(top_srcdir)/src/services/event_system \
	-I$(top_srcdir)/src/services/test_interference \
	-I$(top_srcdir)/src/services/scripter_module \
	-I$(top_srcdir)/src/mintfwif \
	-I$(top_srcdir)/src/test_libraries \
	-I$(top_srcdir)/src/tmc \
	-I$(top_srcdir)/src/tmc/test_module_api \
	-I$(top_srcdir)/src/utils \
	-I$(top_srcdir)/src/utils/lib_loader \
	-I$(top_srcdir)/src/utils/dllist \
	-I$(top_srcdir)/src/utils/ipc \
	-I$(top_srcdir)/src/utils/logger \
	-I$(top_srcdir)/src/utils/parser \
	-I$(top_srcdir)/src/utils/text \
	-I$(top_srcdir)/tests 


pkglib_LTLIBRARIES = scripter.la scripter_cli.la
check_LIBRARIES = libminlegointerface_tests.a
noinst_HEADERS = scripter_dsa.h \
	         min_scripter_if.h \
	         scripted_test_process.h \
		 scripter_plugin.h \
		 min_lego_interface.h \
		 scripter_keyword.h \
		 combined_test_process.h \
		 scripter_common.h
scripter_la_SOURCES = min_scripter_if.c \
	              scripter_dsa.c \
	              scripter_plugin.c \
	              scripted_test_process.c \
		      min_lego_interface.c \
	              scripter_keyword.c \
		      combined_test_process.c

scripter_la_LDFLAGS = -module
scripter_la_CFLAGS = $(AM_CFLAGS)

scripter_cli_la_SOURCES = scripter_cli.c \
			  $(scripter_la_SOURCES)


scripter_la_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/src/tmc/libtmc_utils.la                            \
	$(top_builddir)/src/services/event_system/libminevent.la          \
	-lpthread \
	$(top_builddir)/src/services/test_interference/libmininterference.la  \
        $(top_builddir)/src/utils/libminutils.la -ldl 

scripter_cli_la_LDFLAGS = -module
scripter_cli_la_CFLAGS = $(AM_CFLAGS) -DSCRIPTER_CLI
scripter_cli_la_LIBADD = $(scripter_la_LIBADD)

noinst_LIBRARIES = libscripter_dsa_tests.a 

libscripter_dsa_tests_a_SOURCES = scripter_dsa.c scripter_dsa.tests

libminlegointerface_tests_a_SOURCES = \
	$(scripter_la_SOURCES) min_lego_interface.tests

libminlegointerface_tests_a_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/src/tmc/libtmc_utils.la                            \
	$(top_builddir)/src/services/event_system/libminevent.la          \
	$(top_builddir)/src/services/test_interference/libmininterference.la  \
        $(top_builddir)/src/utils/libminutils.la  
libminlegointerface_tests_a_CFLAGS = -DMIN_UNIT_TEST 

