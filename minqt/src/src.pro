TEMPLATE    =   app
TARGET      =   minqt

QT += dbus sql

INCLUDEPATH +=  ./ui ./core

SOURCES     +=  main.cpp                                \
                ui/min_mainwidget.cpp                   \
                ui/min_devicepanel.cpp                  \
                ui/min_casespanel.cpp                   \
                core/min_casesmodel.cpp                 \
                core/min_availablecasestreemodel.cpp    \
                core/min_object.cpp                     \
                core/min_remotecontroll.cpp             \
                ui/min_mainwindow.cpp			\
		core/min_database.cpp

HEADERS     +=  ui/min_mainwidget.hpp                   \
                ui/min_devicepanel.hpp                  \
                ui/min_casespanel.hpp                   \
                core/min_casesmodel.hpp                 \
                core/min_availablecasestreemodel.hpp    \
                core/min_object.hpp.h                   \
                core/min_remotecontroll.hpp             \
                ui/min_mainwindow.hpp			\
		core/min_database.hpp

MOC_DIR     =   ./.moc
OBJECTS_DIR =   ./.moc
