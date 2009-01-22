INCLUDES = -I. -I../../ -I../logger -I../dllist -I../text
METASOURCES = AUTO
include_HEADERS = min_ipc_mechanism.h
noinst_LTLIBRARIES = libminipc.la 
check_LIBRARIES = libminipctest.a
libminipc_la_SOURCES = min_ipc_mechanism.c
libminipctest_a_SOURCES = min_ipc_mechanism.c
libminipctest_a_CFLAGS =  -DMIN_UNIT_TEST
