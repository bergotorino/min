METASOURCES 	= AUTO
INCLUDES	= -I. -I.. -I../.. -I$(top_srcdir)/. \
		-I$(top_srcdir)/conf \
		-I$(top_srcdir)/integration \
		-I$(top_srcdir)/src \
		-I$(top_srcdir)/src/ats_interface \
		-I$(top_srcdir)/src/consoleui \
		-I$(top_srcdir)/src/engine \
		-I$(top_srcdir)/src/engine/data_structures_api \
		-I$(top_srcdir)/src/engine/tec \
		-I$(top_srcdir)/src/engine/treport_log \
		-I$(top_srcdir)/src/services \
		-I$(top_srcdir)/src/services/event_system \
		-I$(top_srcdir)/src/services/scripter_module \
		-I$(top_srcdir)/src/mintfwif \
		-I$(top_srcdir)/src/test_libraries \
		-I$(top_srcdir)/src/tmc \
		-I$(top_srcdir)/src/tmc/test_module_api \
		-I$(top_srcdir)/src/utils \
		-I$(top_srcdir)/src/utils/lib_loader \
		-I$(top_srcdir)/src/utils/lua-5.1.3/src \
		-I$(top_srcdir)/src/utils/dllist \
		-I$(top_srcdir)/src/utils/ipc \
		-I$(top_srcdir)/src/utils/logger \
		-I$(top_srcdir)/src/utils/parser \
		-I$(top_srcdir)/src/utils/text \
		-I$(top_srcdir)/tests 
pkglib_LTLIBRARIES 	= luascripter.la
#check_LIBRARIES 	=
noinst_HEADERS 		= lua_scripter_if.h
luascripter_la_SOURCES	= lua_scripter_if.c
luascripter_la_LDFLAGS 	= -module
luascripter_la_CFLAGS 	= $(AM_CFLAGS)
luascripter_la_LIBADD 	= $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
		 	$(top_builddir)/src/tmc/libtmc_utils.la \
			$(top_builddir)/src/services/event_system/libminevent.la \
			$(top_builddir)/src/utils/libminutils.la -ldl                

install-data-local:
	mkdir -p $(DESTDIR)/usr/share/min
	cp  minbaselib.lua $(DESTDIR)/usr/share/min
