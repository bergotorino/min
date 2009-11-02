METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = min_unit_selftest.la dllist_tests.la item_parser_tests.la logger_tests.la settings_tests.la
SUBDIRS = data
INCLUDES = -I. \
	   -I../min/src \
	   -I../min/src/services/event_system \
	   -I../min/src/tmc \
	   -I../min/src/tmc/test_module_api \
	   -I../min/src/utils/logger \
	   -I../min/src/utils/dllist \
	   -I../min/src/utils/ipc \
	   -I../min/src/utils/lib_loader \
	   -I../min/src/utils/text \
	   -I../min/src/utils/parser \
	   -I../min/src/utils/settings \
           -I../min/shared

min_unit_selftest_la_CFLAGS = -DCASES_FILE='"src/min_unit_selftest_cases.c"'
min_unit_selftest_la_LDFLAGS = -shared -module
min_unit_selftest_la_SOURCES = ../min/shared/minunit.c 
min_unit_selftest_la_LIBADD =  \
	$(top_builddir)/../min/src/utils/libminutils.la \
	$(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/../min/src/services/event_system/libminevent.la

dllist_tests_la_CFLAGS = -DCASES_FILE='"src/dllist_cases.c"'
dllist_tests_la_LDFLAGS = -shared -module
dllist_tests_la_SOURCES = ../min/shared/minunit.c src/equal.c
dllist_tests_la_LIBADD =  \
	$(top_builddir)/../min/src/utils/libminutils.la \
	$(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/../min/src/services/event_system/libminevent.la

item_parser_tests_la_CFLAGS = -DCASES_FILE='"src/item_parser_cases.c"'
item_parser_tests_la_LDFLAGS = -shared -module
item_parser_tests_la_SOURCES = ../min/shared/minunit.c
item_parser_tests_la_LIBADD =  \
	$(top_builddir)/../min/src/utils/libminutils.la \
	$(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/../min/src/services/event_system/libminevent.la

logger_tests_la_CFLAGS = -DCASES_FILE='"src/logger_tests_cases.c"'
logger_tests_la_LDFLAGS = -shared -module
logger_tests_la_SOURCES = ../min/shared/minunit.c 
logger_tests_la_LIBADD =  \
	$(top_builddir)/../min/src/utils/libminutils.la \
	$(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/../min/src/services/event_system/libminevent.la 

settings_tests_la_CFLAGS = -DCASES_FILE='"src/settings_tests_cases.c"'
settings_tests_la_LDFLAGS = -shared -module
settings_tests_la_SOURCES = ../min/shared/minunit.c \
		            src/settings_tests_utils.c
settings_tests_la_LIBADD =  \
	$(top_builddir)/../min/src/utils/libminutils.la \
	$(top_builddir)/../min/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/../min/src/services/event_system/libminevent.la 


