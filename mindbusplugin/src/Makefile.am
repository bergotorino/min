METASOURCES = AUTO
INCLUDES = -I. -I.. -I../.. -I../../min -I../../min/src/in_plugins \
	-I../../min/src/utils/dllist \
	-I../../min/src \
	-I../../min/src/engine/tec 

pkglib_LTLIBRARIES = min_dbus.la
noinst_HEADERS = min_dbus_plugin.h
include_HEADERS = min_dbus_client_interface.h
min_dbus_la_SOURCES = min_dbus_plugin.c

min_dbus_la_LDFLAGS = -module $(GLIB_LIBS) $(DBUS_GLIB_LIBS) $(GOBJECT_LIBS)
min_dbus_la_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(DBUS_GLIB_CFLAGS) $(GOBJECT_CFLAGS)

CLEANFILES = min_dbus_plugin.h \
	     min_dbus_client_interface.h


min_dbus_plugin.c: min_dbus_plugin.h min_dbus_client_interface.h

min_dbus_plugin.h: min_dbus_interface.xml
	dbus-binding-tool --prefix=min_object --mode=glib-server \
	  $< > $@

min_dbus_client_interface.h: min_dbus_interface.xml
	dbus-binding-tool --prefix=min_object --mode=glib-client \
	  $< > $@
