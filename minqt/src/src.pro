TEMPLATE    =   app
TARGET      =   minqt

QT += dbus sql

INCLUDEPATH +=  ./ui ./core ../../min/src/tmc ../../min/src ../../min/src/utils/dllist ../../min/src/utils/logger ../../min/src/utils/text ../../min/src/utils/ipc

SOURCES     +=  main.cpp                                \
                ui/min_mainwidget.cpp                   \
                ui/min_devicepanel.cpp                  \
                ui/min_casespanel.cpp                   \
                core/min_casesmodel.cpp                 \
                core/min_availablecasestreemodel.cpp    \
                core/min_object.cpp                     \
                core/min_remotecontroll.cpp             \
                ui/min_mainwindow.cpp			        \
		        core/min_database.cpp                   \
                core/min_availablemodel.cpp             \
                core/min_descriptionprovider.cpp	    \
                core/min_singleton.cpp			        \
		        ui/min_aboutdialog.cpp			        \
                ui/min_executedtab.cpp                  \
		        core/min_executedmodel.cpp

HEADERS     +=  ui/min_mainwidget.hpp                   \
                ui/min_devicepanel.hpp                  \
                ui/min_casespanel.hpp                   \
                core/min_casesmodel.hpp                 \
                core/min_availablecasestreemodel.hpp    \
                core/min_object.h                       \
                core/min_remotecontroll.hpp             \
                ui/min_mainwindow.hpp		            \
		        core/min_database.hpp                   \
                core/min_availablemodel.hpp             \
                core/min_descriptionprovider.hpp	    \
                core/min_singleton.hpp			        \
		        ui/min_aboutdialog.hpp			        \
                ui/min_executedtab.hpp                  \
		        core/min_executedmodel.hpp

MOC_DIR     =   ./.moc
OBJECTS_DIR =   ./.moc
target.path = /usr/bin
INSTALLS = target
