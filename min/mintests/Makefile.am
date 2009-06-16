INCLUDES = -I.  -I../ -I../src -I../src/tmc/ \
	   -I../src/utils/lib_loader \
	   -I../src/utils/dllist -I../src/utils/ipc -I../src/utils/text \
	   -I../src/utils/logger \
           -I../src/utils/parser \
	   -I../src/tmc/test_module_api \
	   -I../src/services/event_system \
	   -I../shared

AM_FLAGS=-fPIC
noinst_HEADERS = equal.h
pkglib_LTLIBRARIES = min_unit_selftest.la dllist_tests.la 
TOPDIR=$(PWD)
DESTDIR=$(PWD)/tmp
TESTS = $(DESTDIR)/usr/lib/min/min_unit_selftest.so \
	$(DESTDIR)/usr/lib/min/dllist_tests.so
TESTS_ENVIRONMENT = min --console --execute
SCRIPTER = /usr/lib/min/scripter.so
SCRIPTERTESTS = scripter_tests.cfg
XFAIL_TESTS = $(DESTDIR)/usr/lib/min/min_unit_selftest.so

min_unit_selftest_la_CFLAGS = -DCASES_FILE='"min_unit_selftest_cases.c"'
min_unit_selftest_la_LDFLAGS = -shared -module
min_unit_selftest_la_SOURCES = ../shared/minunit.c 
min_unit_selftest_la_LIBADD = \
	              $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	              $(top_builddir)/src/utils/libminutils.la \
		      $(top_builddir)/src/services/event_system/libminevent.la

dllist_tests_la_CFLAGS = -DCASES_FILE='"dllist_cases.c"'
dllist_tests_la_LDFLAGS = -shared -module
dllist_tests_la_SOURCES = ../shared/minunit.c equal.c
dllist_tests_la_LIBADD = \
	              $(top_builddir)/src/tmc/test_module_api/libmintmapi.la \
	              $(top_builddir)/src/utils/libminutils.la \
		      $(top_builddir)/src/services/event_system/libminevent.la

check-local: 
	min-clean.sh -f; \
	if [ ! -d ~/.min ]; then \
		mkdir ~/.min; \
	fi; \
	failedtests=0; passedtests=0; numbertests=0;\
	list=' $(SCRIPTERTESTS) '; \
	for tst in $$list; do \
	  	numbertests=`expr $$numbertests + 1`; \
		if $(TESTS_ENVIRONMENT) $(SCRIPTER):$$tst; then \
		  passedtests=`expr $$passedtests + 1`; \
		else \
		  failedtests=`expr $$failedtests + $$?`; \
		fi; \
	done; \
	echo "$$numbertests MIN scripts executed"; \
	echo "Failed $$failedtests Passed $$passedtests"; \
	test "$$failedtests" -eq 0;
