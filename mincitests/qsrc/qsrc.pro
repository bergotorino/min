TEMPLATE = lib
VERSION = 0.0.1

QT += sql 

CONFIG = dll
CONFIG += qt
CONFIG += link_pkgconfig
CONFIG += build_all
CONFIG += lib_bundle

INCLUDEPATH +=  ../../minqt/src/core ../../min/src/tmc

SOURCES += ../../minqt/src/core/min_database.cpp ../../minqt/src/core/min_singleton.cpp minqtdbtestsCases.cpp minqtdbtests.cpp	

HEADERS += \
        minqtdbtestsCases.hpp \
        ../../minqt/src/core/min_database.hpp \
        ../../minqt/src/core/min_singleton.hpp 
	
LIBS += -lminutils -lmintmapi -lminevent

MIN_SCRIPTS.files = data/*.cfg
MIN_SCRIPTS.path = /usr/lib/min
INSTALLS += MIN_SCRIPTS

MIN_MODULES.files = *.so*
MIN_MODULES.path = /usr/lib/min
INSTALLS += MIN_MODULES
