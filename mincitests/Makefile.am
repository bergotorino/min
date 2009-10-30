METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = min_unit_selftest.la dllist_tests.la item_parser_tests.la logger_tests.la
TOPDIR=.
SUBDIRS = data

min_unit_selftest_la_CFLAGS = -DCASES_FILE='"src/min_unit_selftest_cases.c"'
min_unit_selftest_la_LDFLAGS = -shared -module
min_unit_selftest_la_SOURCES = /usr/share/min/minunit.c 
min_unit_selftest_la_LIBADD =  -lminutils -lmintmapi -lminevent  

dllist_tests_la_CFLAGS = -DCASES_FILE='"src/dllist_cases.c"'
dllist_tests_la_LDFLAGS = -shared -module
dllist_tests_la_SOURCES = /usr/share/min/minunit.c src/equal.c
dllist_tests_la_LIBADD = -lminutils -lmintmapi -lminevent  

item_parser_tests_la_CFLAGS = -DCASES_FILE='"src/item_parser_cases.c"'
item_parser_tests_la_LDFLAGS = -shared -module
item_parser_tests_la_SOURCES = /usr/share/min/minunit.c
item_parser_tests_la_LIBADD = -lminutils -lmintmapi -lminevent  

logger_tests_la_CFLAGS = -DCASES_FILE='"src/logger_tests_cases.c"'
logger_tests_la_LDFLAGS = -shared -module
logger_tests_la_SOURCES = /usr/share/min/minunit.c 
logger_tests_la_LIBADD = -lminutils -lmintmapi -lminevent  


