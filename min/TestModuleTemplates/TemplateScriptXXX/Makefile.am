METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = TemplateScriptXXXTestModule.la

TOPDIR=.
DESTDIR=$(PWD)/tmp

TemplateScriptXXXTestModule_la_CFLAGS = -I$(TOPDIR)/src
TemplateScriptXXXTestModule_la_LDFLAGS = -shared -module
TemplateScriptXXXTestModule_la_SOURCES = $(TOPDIR)/src/TemplateScriptXXX.c $(TOPDIR)/src/TemplateScriptXXXCases.c
TemplateScriptXXXTestModule_la_LIBADD =  -lminutils -lmintmapi -lminevent

group_files = `ls -a ./ | grep -v CVS | grep -v debian | grep -v src | grep -v createtestclass | grep -v group | grep -v Makefile.am | grep -v ^\\[.\\]*$$`

install-data-local:
		mkdir -p $(HOME)/.min
		cp ./tmp/usr/lib/TestModuleTemplateScriptXXXTestModule/* $(HOME)/.min


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
