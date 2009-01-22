INCLUDES = -I. -I../ -I../../ -I../../utils/lib_loader/ -I../../services/event_system\
-I../../utils/dllist  -I../../utils/ipc -I../../utils/logger -I../../utils/text
METASOURCES = AUTO
include_HEADERS = test_module_api.h
lib_LTLIBRARIES = libmintmapi.la
check_LIBRARIES = libmintmapitest.a

libmintmapi_la_SOURCES = test_module_api.c
libmintmapi_la_LIBADD = $(top_builddir)/src/utils/libminutils.la 

libmintmapitest_a_CFLAGS = $(libmintmapi_a_CFLAGS) -DMIN_UNIT_TEST -lcheck
libmintmapitest_a_SOURCES = test_module_api.c test_module_api.tests
libmintmapitest_a_LIBADD = $(libmintmapi_a_LIBADD)
