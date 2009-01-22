INCLUDES = -I../../ -I../../utils/dllist 
METASOURCES = AUTO
noinst_LTLIBRARIES = libmindsa.la
check_LIBRARIES = libmindsatest.a
libmindsa_la_SOURCES = data_test_case.c data_test_module_info.c \
	data_test_result.c
libmindsatest_a_SOURCES = data_test_case.c data_test_module_info.c \
	data_test_result.c
libmindsatest_a_CFLAGS = -I../../utils.dllist -I../.. -DMIN_UNIT_TEST
noinst_HEADERS = data_api.h data_common.h data_test_case.h \
	data_test_module_info.h data_test_result.h
libmindsa_la_LIBADD = $(top_builddir)/src/utils/libminutils.la
libmindsatest_a_LIBADD = $(top_builddir)/src/utils/libminutils.la
