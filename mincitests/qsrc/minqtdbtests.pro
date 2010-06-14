TEMPLATE = lib
VERSION = 0.0.1

QT += sql 

CONFIG = dll
CONFIG += qt
CONFIG += link_pkgconfig
CONFIG += build_all
CONFIG += lib_bundle

INCLUDEPATH +=  ../../minqt/src/core                    \
                ../../min/src                           \
                ../../min/src/tmc                       \
                ../../min/src/tmc/test_module_api       \
                ../../min/src/utils/dllist              \
                ../../min/src/utils/logger              \
                ../../min/src/utils/text                \
                ../../min/src/utils/ipc                 \
                ../../min/src/utils/parser              \
                ../../min/src/utils/lib_loader          \
                ../../min/src/services/event_system 

SOURCES += ../../minqt/src/core/min_database.cpp ../../minqt/src/core/min_singleton.cpp minqtdbtestsCases.cpp minqtdbtests.cpp	

HEADERS += \
        minqtdbtestsCases.hpp \
        ../../minqt/src/core/min_database.hpp \
        ../../minqt/src/core/min_singleton.hpp 
	
TARGET = minqtdbtests

MIN_SCRIPTS.files = data/*.cfg
MIN_SCRIPTS.path = /usr/lib/min
INSTALLS += MIN_SCRIPTS

MIN_MODULES.files = *.so*
MIN_MODULES.path = /usr/lib/min
INSTALLS += MIN_MODULES
unix {
MV_TARGET = cp libminqtdbtests.so.0.0.1 minqtdbtests.so.0.0.1; \
ln -s minqtdbtests.so.0.0.1 minqtdbtests.so;\
ln -s minqtdbtests.so.0.0.1 minqtdbtests.so.0;\
ln -s minqtdbtests.so.0.0.1 minqtdbtests.so.0.0; rm libminqtdbtests*

QMAKE_POST_LINK = $$MV_TARGET
}
