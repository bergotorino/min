INCLUDES = -I. -I../.. -I../../utils/logger -I../../utils/dllist -I../../utils/ipc -I../../utils/lib_loader \
           -I../../utils/text -I../../tmc 
METASOURCES = AUTO
include_HEADERS = min_test_event_if.h
lib_LTLIBRARIES = libminevent.la
check_LIBRARIES = libminevent_test.a
libminevent_la_SOURCES = min_test_event_if.c
libminevent_la_LIBADD = $(top_builddir)/src/utils/libminutils.la

libminevent_test_a_SOURCES = min_test_event_if.c min_tests_event_if.tests
libminevent_test_a_LIBADD = $(top_builddir)/src/utils/libminutils.la

libminevent_test_a_CFLAGS = -DMIN_UNIT_TEST 
