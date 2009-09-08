TEMPLATE = lib
VERSION = 0.0.1

CONFIG = dll
CONFIG += qt
CONFIG += link_pkgconfig
CONFIG += build_all
CONFIG += lib_bundle

SOURCES += \
	src/TemplateQtScriptXXXCases.cpp \
	src/TemplateQtScriptXXX.cpp	

HEADERS += \
        src/TemplateQtScriptXXXCases.hpp
	
TARGET = TemplateQtScriptXXX

LIBS += -lminutils -lmintmapi -lminevent

MIN_CONFIG.version = Versions
MIN_CONFIG.files = data/*.min.conf
MIN_CONFIG.path = debian/tmp/etc/min.d
INSTALLS += MIN_CONFIG

MIN_SCRIPTS.files = data/*.cfg
MIN_SCRIPTS.path = debian/tmp/usr/lib/min
INSTALLS += MIN_SCRIPTS

MIN_MODULES.files = *.so*
MIN_MODULES.path = debian/tmp/usr/lib/min
INSTALLS += MIN_MODULES


