/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
 * Contact e-mail: DG.MIN-Support@nokia.com
 * 
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, version 2 of the License. 
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  Public License for
 * more details. You should have received a copy of the GNU General Public 
 * License along with this program. If not,  see 
 * <http://www.gnu.org/licenses/>.
 */

/**
 * @file:     main.cpp
 * @version:  1.00
 * @date:     18.03.2009
 * @author:   
 */

// System includes
#include <cstdlib>
#include <QApplication>
#include <QtCore/QtCore>
#include <QtDBus/QtDBus>

// Min includes
#include "ui/min_mainwindow.hpp"
#include "core/min_object.hpp.h"

#define MIN_SERVICE_NAME     "org.maemo.MIN"
#define MIN_SERVICE_OBJECT_PATH "/Min"
#define MIN_SERVICE_INTERFACE "org.maemo.MIN"

// -----------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    QApplication app(argc,argv);
    Min::MainWindow mw;
    mw.show();
    return app.exec();
   
//    QCoreApplication app(argc, argv);

    // 1. Connect to DBus
    QDBusConnection bus = QDBusConnection::connectToBus(QDBusConnection::SessionBus,
                                                        "org.maemo.MIN");
    if (!bus.isConnected()) {
        qDebug("Error: %s",bus.lastError().message().toStdString().c_str());
    } else {
        qDebug("Connected to bus: org.Maemo.MIN");
    }

    // 
    MinObject minobj("org.maemo.MIN","/Min",bus);
    if (!minobj.isValid()) {
        qDebug("Error: %s",minobj.lastError().message().toStdString().c_str());
    } else {
        qDebug("MinObject created successfully");
    }
    minobj.min_open();



    // 2. Connect to interface
/*    QDBusConnectionInterface *iface = bus.interface();
    if (!iface->isValid()) {
        qDebug("%s",iface->lastError().message().toStdString().c_str());
    } else {
        qDebug("Connected to interface: org.Maemo.MIN");
    }*/

    // 3. Get registered services from interface


    return app.exec();
}

// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
