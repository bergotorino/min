INCLUDES =  -I. -I../..
METASOURCES = AUTO
include_HEADERS = dllist.h
noinst_LTLIBRARIES = libmindllist.la
check_LIBRARIES = libmindllisttest.a
libmindllist_la_SOURCES = dllist.c
libmindllisttest_a_SOURCES = dllist.c dllist.tests
libmindllisttest_a_CFLAGS = -I. -I../.. -DMIN_UNIT_TEST
