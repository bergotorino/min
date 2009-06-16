INCLUDES = 	-I. -I../ -I../in_plugins/cli -I../in_plugins/cui \
	        -I../engine/tec -I../utils/dllist  -I../utils/logger \
		-I../utils/lib_loader -I../utils/ipc \
		-I../utils/parser -I../services/event_system \
		-I../engine/data_structures_api -I../ext_interface \
		-I../utils/text -I../tmc -I../src/utils/lua-5.1.3/src
METASOURCES = AUTO
bin_PROGRAMS = min mind
bin_SCRIPTS = min-clean.sh 
min_SOURCES = min_main.c
min_LDADD =	$(top_builddir)/src/engine/tec/libmintec.a \
	$(top_builddir)/src/engine/data_structures_api/libmindsa.la \
	$(top_builddir)/src/engine/treport_log/libmintreport.la \
	$(top_builddir)/src/utils/libminutils.la \
	-lncurses -lmenu -lpthread 

mind_SOURCES = min_daemon.c
mind_LDADD = $(top_builddir)/src/utils/libminutils.la 
