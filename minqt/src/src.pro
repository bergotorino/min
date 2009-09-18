TEMPLATE    = app
TARGET      = minqt
RESOURCES   = icons.qrc  

QT += dbus sql network
QMAKE_CXXFLAGS += -D$(DEB_HOST_ARCH)

INCLUDEPATH +=  ./ui                                    \
                ./core                                  \
                ../../min/src/tmc                       \
                ../../min/src                           \
                ../../min/src/utils/dllist              \
                ../../min/src/utils/logger              \
                ../../min/src/utils/text                \
                ../../min/src/utils/ipc                 \
                ../../min/src/in_plugins/tcpip

# UI first, then CORE, please obey for sake of clarity!

SOURCES     +=  main.cpp                                \
                ui/min_mainwidget.cpp                   \
                ui/min_devicepanel.cpp                  \
                ui/min_casespanel.cpp                   \
                ui/min_mainwindow.cpp		        \
                ui/min_aboutdialog.cpp			\
                ui/min_executedtab.cpp                  \
                ui/min_addmoduledialog.cpp              \
                ui/min_dbusconnectiondialog.cpp         \
                core/min_executedmodel.cpp              \
                core/min_casesmodel.cpp                 \
                core/min_testruntreemodel.cpp           \
                core/min_object.cpp                     \
                core/min_remotecontroll.cpp             \
                core/min_eapi_client.cpp                \
		core/min_database.cpp                   \
                core/min_availablemodel.cpp             \
                core/min_descriptionprovider.cpp	\
                core/min_singleton.cpp                  \
                core/min_statusbarprovider.cpp          \
                core/min_executedalldelegate.cpp

HEADERS     +=  ui/min_mainwidget.hpp                   \
                ui/min_devicepanel.hpp                  \
                ui/min_casespanel.hpp                   \
                ui/min_mainwindow.hpp	                \
                ui/min_aboutdialog.hpp		        \
                ui/min_executedtab.hpp                  \
                ui/min_addmoduledialog.hpp              \
                ui/min_dbusconnectiondialog.hpp         \
		core/min_executedmodel.hpp              \
                core/min_casesmodel.hpp                 \
                core/min_testruntreemodel.hpp           \
                core/min_object.h                       \
                core/min_remotecontroll.hpp             \
                core/min_eapi_client.hpp                \
		core/min_database.hpp                   \
                core/min_availablemodel.hpp             \
                core/min_descriptionprovider.hpp	\
                core/min_singleton.hpp                  \
                core/min_statusbarprovider.hpp          \
                core/min_executedalldelegate.cpp

PREFIX = /usr
DATADIR = $$PREFIX/share
DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"
desktop.path = $$DATADIR/applications/hildon
desktop.files += minqt.desktop

iconxpm.path = $$DATADIR/pixmap
iconxpm.files += data/maemo/minqt.xpm 

icon26.path = $$DATADIR/icons/hicolor/26x26/apps
icon26.files += data/26x26/minqt.png 

icon40.path = $$DATADIR/icons/hicolor/40x40/apps
icon40.files += data/40x40/minqt.png

icon64.path = $$DATADIR/icons/hicolor/64x64/apps
icon64.files += data/64x64/minqt.png

MOC_DIR     =   ./.moc
OBJECTS_DIR =   ./.moc
target.path = /usr/bin
INSTALLS    = target desktop icon26 icon40 icon64

