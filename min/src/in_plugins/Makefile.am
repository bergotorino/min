METASOURCES = AUTO
INCLUDES = -I. -I.. -I../.. -I$(top_srcdir)/. \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/engine/tec \
	-I/usr/include/glib-2.0 \
	-I/usr/lib/glib-2.0/include \
	-I/usr/include/dbus-1.0 \
	-I/usr/lib/dbus-1.0/include

pkglib_LTLIBRARIES = min_dbus.la
noinst_HEADERS = min_dbus_plugin.h
include_HEADERS = min_plugin_interface.h \
		  min_dbus_client_interface.h
min_dbus_la_SOURCES = min_dbus_plugin.c

min_dbus_la_LDFLAGS = -module
min_dbus_la_CFLAGS = $(AM_CFLAGS) -lglib-2.0 -ldbus-glib-1

#min_dbus_la_LIBADD =

CLEANFILES = min_dbus_plugin.h \
	     min_dbus_client_interface.h

min_dbus.la: min_dbus_plugin.h min_dbus_client_interface.h

min_dbus_plugin.h: min_dbus_interface.xml
	dbus-binding-tool --prefix=min_object --mode=glib-server \
	  $< > $@

min_dbus_client_interface.h: min_dbus_interface.xml
	dbus-binding-tool --prefix=min_object --mode=glib-client \
	  $< > $@
