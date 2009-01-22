INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/conf -I$(top_srcdir)/integration \
	-I$(top_srcdir)/src -I$(top_srcdir)/src/ext_interface -I$(top_srcdir)/src/consoleui \
	-I$(top_srcdir)/src/engine -I$(top_srcdir)/src/engine/data_structures_api \
	-I$(top_srcdir)/src/engine/tec -I$(top_srcdir)/src/engine/treport_log -I$(top_srcdir)/src/services \
	-I$(top_srcdir)/src/services/event_system -I$(top_srcdir)/src/services/scripter_module \
	-I$(top_srcdir)/src/mintfwif -I$(top_srcdir)/src/test_libraries -I$(top_srcdir)/src/tmc \
	-I$(top_srcdir)/src/utils/lib_loader -I$(top_srcdir)/src/tmc/test_module_api -I$(top_srcdir)/src/utils \
	-I$(top_srcdir)/src/utils/dllist -I$(top_srcdir)/src/utils/ipc -I$(top_srcdir)/src/utils/logger \
	-I$(top_srcdir)/src/utils/parser -I$(top_srcdir)/tests
METASOURCES = AUTO
SUBDIRS = text dllist ipc logger parser lua-5.1.3 lib_loader
lib_LTLIBRARIES = libminutils.la

libminutils_la_SOURCES =
libminutils_la_LDFLAGS = -export-dynamic -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

libminutils_la_LIBADD = $(top_builddir)/src/utils/logger/libminlogger.la \
                         $(top_builddir)/src/utils/ipc/libminipc.la \
                         $(top_builddir)/src/utils/parser/libminparser.la \
                         $(top_builddir)/src/utils/dllist/libmindllist.la \
			 $(top_builddir)/src/utils/text/libmintext.la \
			 $(top_builddir)/src/utils/lib_loader/libmintl.la \
			 $(top_builddir)/src/utils/lua-5.1.3/src/libminlua.la -lm -ldl

