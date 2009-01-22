METASOURCES=Auto
AM_FLAGS=-fPIC
pkglib_LTLIBRARIES = HardCoded.la

HardCoded_la_LDFLAGS = -shared -module
HardCoded_la_SOURCES = HardCoded.c HardCodedCases.c
HardCoded_la_LIBADD = -lminutils -lmintmapi -lminevent

install-data-local:
	cp .libs/HardCoded.so $(DESTDIR)/usr/share/min-tests

