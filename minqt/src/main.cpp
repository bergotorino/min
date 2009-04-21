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
#include "core/min_remotecontroll.hpp"
#include "core/min_database.hpp"


// -----------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    // 1. Init the application
    QApplication app(argc,argv);

    Min::Database::getInstance();

    // 2. Create window
    Min::MainWindow mw;
    mw.show();

    // 3. Run the application
    return app.exec();
   
    Min::RemoteControll &rc = Min::RemoteControll::getInstance();
    rc.minAddTestModule("/home/user/.min/Hard.so");

    return app.exec();
}

// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
