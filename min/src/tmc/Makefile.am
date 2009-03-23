INCLUDES =  -I. -I./test_module_api -I../utils/lib_loader/ -I.. -I../utils/logger -I../utils/dllist -I../utils/ipc -I../utils/text -I../services/event_system
METASOURCES = AUTO
bin_PROGRAMS = tmc
noinst_LTLIBRARIES = libtmc_utils.la
tmc_SOURCES = main.c tmc.c tmc_ipc.c tmc_tpc.c tmc_msghnd.c
tmc_LDADD = $(top_builddir)/src/utils/libminutils.la  
tmc_LDADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la  
include_HEADERS = tmc.h tmc_common.h tmc_tpc.h tmc_ipc.h
SUBDIRS = test_module_api

libtmc_utils_la_SOURCES = main.c tmc.c tmc_ipc.c tmc_msghnd.c tmc_tpc.c
libtmc_utils_la_CFLAGS = $(AM_CFLAGS)
