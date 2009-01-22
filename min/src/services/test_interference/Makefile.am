INCLUDES = -I. -I../.. -I../../utils/logger -I../../utils/dllist -I../../utils/ipc -I../../utils/lib_loader \
           -I../../utils/text -I../../tmc -I../event_system
METASOURCES = AUTO

include_HEADERS = min_test_interference.h

lib_LTLIBRARIES = libmininterference.la

libmininterference_la_SOURCES = min_test_interference.c

libmininterference_la_LIBADD = $(top_builddir)/src/utils/libminutils.la -lpthread
