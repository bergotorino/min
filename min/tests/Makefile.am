INCLUDES = -I. -I../src -I../src/utils/dllist -I../src/utils/text -I../src/utils/logger -I../src/utils/ipc -I../src/engine/data_structures_api/  -I../src/tmc -I../src/tmc/test_module_api -I../src/utils/lib_loader -I../src/services/event_system -I../src/utils/parser -I../src/mintfwif -I../src/engine/tec -I../src/consoleui -I../src/services/scripter_module
METASOURCES = AUTO

TESTS = check_dsa check_logger check_utils check_tmapi check_tec check_minunit check_services check_parser check_mintfwif check_min_lego_interface check_scripter_dsa check_text check_libloader check_cli.sh
check_PROGRAMS =  check_dsa check_utils check_logger check_tmapi check_tec check_minunit check_services check_parser check_mintfwif check_integration check_min_lego_interface check_scripter_dsa check_text check_libloader 


AM_LDFLAGS=-L/usr/local/lib

check_utils_LDADD =  $(top_builddir)/src/utils/ipc/libminipctest.a \
		     $(top_builddir)/src/utils/dllist/libmindllisttest.a \
		     $(top_builddir)/src/utils/logger/libminloggertest.a \
	             $(top_builddir)/src/utils/text/libmintext.la -lcheck
check_utils_SOURCES = check_utils.c

check_services_LDADD =  $(top_builddir)/src/services/event_system/libminevent_test.a\
			$(top_builddir)/src/utils/libminutils.la -lcheck

check_services_SOURCES = check_services.c

check_logger_SOURCES = check_logger.c check_logger.tests
check_logger_LDADD =  $(top_builddir)/src/utils/logger/libminloggertest.a \
		      $(top_builddir)/src/utils/libminutils.la -lcheck

check_dsa_LDADD =  $(top_builddir)/src/engine/data_structures_api/libmindsa.la \
		   $(top_builddir)/src/utils/libminutils.la -lcheck
check_dsa_SOURCES = check_dsa.c check_dsa.tests

check_tmapi_LDADD =  $(top_builddir)/src/tmc/test_module_api/libmintmapitest.a \
		     $(top_builddir)/src/utils/libminutils.la \
		     -lcheck
check_tmapi_SOURCES = check_tmapi.c

check_tec_LDADD = $(top_builddir)/src/engine/tec/libmintectest.a \
		  $(top_builddir)/src/engine/data_structures_api/libmindsa.la \
		  $(top_builddir)/src/utils/libminutils.la \
		  -lpthread -lcheck

check_tec_SOURCES = check_tec.c

check_minunit_LDADD = -lcheck $(top_builddir)/src/utils/libminutils.la
check_minunit_SOURCES = check_minunit.c check_minunit.tests \
			 ../src/test_libraries/min_unit_selftest.c \
			 ../src/test_libraries/min_unit_selftest_cases.c

check_parser_LDADD = $(top_builddir)/src/utils/parser/libminparsertest.a \
		   $(top_builddir)/src/utils/libminutils.la -lcheck 

check_parser_SOURCES = check_parser.c

check_mintfwif_SOURCES = check_mintfwif.c
check_mintfwif_LDADD = $(top_builddir)/src/mintfwif/libmintfwiftest.a \
	$(top_builddir)/src/engine/tec/libmintecext.la $(top_builddir)/src/utils/libminutils.la \
	-lpthread -lcheck

check_integration_SOURCES = check_integration.c integration.tests
check_integration_LDADD = $(top_builddir)/src/engine/tec/libmintecext.la \
	$(top_builddir)/src/mintfwif/libmintfwif.la  $(top_builddir)/src/utils/libminutils.la \
	 -lpthread -lcheck

check_scripter_dsa_SOURCES = check_scripter_dsa.c scripter_dsa.tests
check_scripter_dsa_LDADD = \
	$(top_builddir)/src/services/scripter_module/libscripter_dsa_tests.a $(top_builddir)/src/utils/libminutils.la \
	-lcheck


check_min_lego_interface_SOURCES = check_min_lego_interface.c
check_min_lego_interface_LDADD = \
	$(top_builddir)/src/services/scripter_module/libminlegointerface_tests.a \
	$(top_builddir)/src/services/event_system/libminevent.la \
	$(top_builddir)/src/services/test_interference/libmininterference.la \
	$(top_builddir)/src/utils/libminutils.la \
	$(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	-ldl -lcheck -lpthread

check_text_LDADD = $(top_builddir)/src/utils/text/libmintexttest.a \
		   -lcheck

check_text_SOURCES = check_text.c

check_libloader_SOURCES = check_libloader.c
check_libloader_LDADD = $(top_builddir)/src/utils/lib_loader/libmintltest.a \
			$(top_builddir)/src/utils/logger/libminlogger.la \
			$(top_builddir)/src/utils/text/libmintext.la \
			$(top_builddir)/src/utils/ipc/libminipc.la \
			$(top_builddir)/src/utils/dllist/libmindllist.la \
		        -lcheck -ldl

integration: check_integration
	./check_integration
