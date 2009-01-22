METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = TestModuleXXXTestModule.la
TESTS_ENVIRONMENT = min --console --execute
TESTS = $(DESTDIR)/usr/lib/TestModuleXXXTestModule/TestModuleXXXTestModule.so
DESTDIR=$(PWD)/tmp
TOPDIR=.

TestModuleXXXTestModule_la_CFLAGS = -I$(TOPDIR)/src
TestModuleXXXTestModule_la_LDFLAGS = -shared -module
TestModuleXXXTestModule_la_SOURCES = $(TOPDIR)/src/TestModuleXXX.c $(TOPDIR)/src/TestModuleXXXCases.c
TestModuleXXXTestModule_la_LIBADD = -lminutils -lmintmapi -lminevent

group_files = `ls -a ./ | grep -v CVS | grep -v debian | grep -v src | grep -v createmodule | grep -v group | grep -v Makefile.am | grep -v ^\\[.\\]*$$`

install-data-local:
		mkdir -p $(HOME)/.min
		cp ./tmp/usr/lib/TestModuleXXXTestModule/* $(HOME)/.min

		exec patchminconf.sh '$(HOME)/.min/min.conf' 'TestModuleXXXTestModule'


oldstyle:
		mkdir group
		make distclean
		mv $(group_files) ./group
		cat Makefile.am | sed s/^TOPDIR=./TOPDIR=../g > ./group/Makefile.am
		rm Makefile.am

newstyle:
		make distclean
		mv $(group_files) ../
		cat Makefile.am | sed s/^TOPDIR=../TOPDIR=./g > ../Makefile.am
		cd .. && rm -rf group
