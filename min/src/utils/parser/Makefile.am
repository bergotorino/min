INCLUDES = -I. -I../.. -I../dllist -I../text
METASOURCES = AUTO
noinst_LTLIBRARIES = libminparser.la
check_LIBRARIES = libminparsertest.a
include_HEADERS =  min_file_parser.h min_parser.h min_item_parser.h min_parser_common.h min_section_parser.h
libminparser_la_SOURCES = min_file_parser.c min_parser.c min_item_parser.c min_section_parser.c
libminparsertest_a_SOURCES = $(libminparser_la_SOURCES) parser.test \
	                      min_file_parser.test min_item_parser.tests
libminparsertest_a_CFLAGS = -DMIN_UNIT_TEST

