INCLUDES = -I. -I.. -I../.. -I../../utils/dllist -I../../utils/logger -I../../utils/ipc -I../../utils/text -I../../tmc
METASOURCES = AUTO
include_HEADERS = tllib.h
noinst_LTLIBRARIES = libmintl.la
check_LIBRARIES = libmintltest.a
libmintl_la_SOURCES = tllib.c
libmintltest_a_SOURCES = tllib.c tllib.tests
libmintltest_a_CFLAGS = -DMIN_UNIT_TEST


#libmintl_la_LIBADD = 	$(top_builddir)/src/utils/text/libmintext.la \
#			$(top_builddir)/src/utils/logger/libminlogger.la 
