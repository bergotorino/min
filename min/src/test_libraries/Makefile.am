METASOURCES = AUTO
INCLUDES = -I../ -I../tmc/test_module_api -I../tmc/ -I../utils/lib_loader\
	   -I../utils/dllist -I../utils/ipc -I../utils/text -I../utils/logger \
	   -I../services/event_system -I../utils/parser
AM_CFLAGS = -fPIC  
#change to pkglib if you want use this
noinst_LTLIBRARIES =  testlib2.la testlib3.la testlib4.la \
	testlibio.la testlibevents.la minunitdllist.la 
pkglib_LTLIBRARIES = minDemoModule.la demoClass.la

demoClass_la_LDFLAGS = -shared -module
demoClass_la_SOURCES = tc.c tcCases.c
demoClass_la_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	              $(top_builddir)/src/utils/libminutils.la \
		      $(top_builddir)/src/services/event_system/libminevent.la

testlib2_la_LDFLAGS = -shared -module
testlib2_la_SOURCES = testlibrary2.c
testlib2_la_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/src/utils/libminutils.la	

testlib3_la_LDFLAGS = -shared -module
testlib3_la_SOURCES = testlibrary3.c
testlib3_la_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/src/utils/libminutils.la	

testlib4_la_LDFLAGS = -shared -module
testlib4_la_SOURCES = testlibrary4.c
testlib4_la_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/src/utils/libminutils.la	

testlibio_la_LDFLAGS = -shared -module
testlibio_la_SOURCES = testlibrary_fileio.c file_operations.c
testlibio_la_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/src/utils/libminutils.la	


minunitdllist_la_LDFLAGS = -shared -module
minunitdllist_la_SOURCES = min_unit_dllist.c  
minunitdllist_la_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	$(top_builddir)/src/utils/libminutils.la	

testlibevents_la_LDFLAGS = -shared -module
testlibevents_la_SOURCES = testlib_event_server.c
testlibevents_la_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
		$(top_builddir)/src/services/event_system/libminevent.la         \
		$(top_builddir)/src/utils/libminutils.la	

minDemoModule_la_LDFLAGS = -module -shared
minDemoModule_la_SOURCES = Demo.c DemoCases.c Demo.h
minDemoModule_la_LIBADD = $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
		$(top_builddir)/src/utils/libminutils.la \
		$(top_builddir)/src/services/event_system/libminevent.la          
