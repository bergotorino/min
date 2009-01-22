METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = LuaTestClass.la

LuaTestClass_la_CFLAGS = -DLUA_USE_POSIX
LuaTestClass_la_LDFLAGS = -shared -module
LuaTestClass_la_SOURCES = LuaTestClass.c LuaTestClassCases.c
LuaTestClass_la_LIBADD = -lminutils -lmintmapi -lminevent

install-data-local:
	cp .libs/LuaTestClass.so $(DESTDIR)/usr/share/min-tests
