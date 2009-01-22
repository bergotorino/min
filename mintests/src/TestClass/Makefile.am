METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = TestClass.la

TestClass_la_LDFLAGS = -shared -module
TestClass_la_SOURCES = TestClass.c TestClassCases.c
TestClass_la_LIBADD = -lminutils -lmintmapi -lminevent

install-data-local:
	cp .libs/TestClass.so $(DESTDIR)/usr/share/min-tests
