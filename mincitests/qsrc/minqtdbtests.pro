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
	
TARGET = minqtdbtests.so

MIN_SCRIPTS.files = data/*.cfg
MIN_SCRIPTS.path = /usr/lib/min
INSTALLS += MIN_SCRIPTS

MIN_MODULES.files = *.so*
MIN_MODULES.path = /usr/lib/min
INSTALLS += MIN_MODULES
QMAKE_LFLAGS = $$QMAKE_LFLAGS_SHLIB -module -avoid-version 
QMAKE_CXXFLAGS = $$QMAKE_CXXFLAGS_SHLIB
