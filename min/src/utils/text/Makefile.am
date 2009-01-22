INCLUDES =  -I. -I../..
METASOURCES = AUTO
include_HEADERS = min_text.h
noinst_LTLIBRARIES = libmintext.la
check_LIBRARIES = libmintexttest.a
libmintext_la_SOURCES = min_text.c
libmintexttest_a_SOURCES = min_text.c text.tests
libmintexttest_a_CFLAGS = -I. -I../.. -DMIN_UNIT_TEST
