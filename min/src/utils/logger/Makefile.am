INCLUDES = -I../../ -I../dllist -I../ipc -I../text
METASOURCES = AUTO
include_HEADERS = min_system_logger.h min_logger.h min_logger_common.h min_logger_output.h
noinst_LTLIBRARIES = libminlogger.la 
check_LIBRARIES = libminloggertest.a
libminlogger_la_SOURCES = min_system_logger.c min_logger.c min_logger_output.c
libminloggertest_a_SOURCES = min_system_logger.c min_logger.c min_logger_output.c
libminloggertest_a_LIBADD= $(top_builddir)/src/utils/dllist/libmindllist.la \
			$(top_builddir)/src/utils/text/libmintext.la
libminloggertest_a_CFLAGS = -DMIN_UNIT_TEST
