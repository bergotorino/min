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
 * @file:     min_mainwindow.cpp
 * @version:  1.00
 * @date:     18.03.2009
 * @author:   
 */

// Module includes
#include "min_mainwindow.hpp"

// System includes
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QGridLayout>
#include <QListView>

// Min includes
#include "min_mainwidget.hpp"

// -----------------------------------------------------------------------------
Min::MainWindow::MainWindow()
    : QMainWindow()
    , mainWidget_(new Min::MainWidget(this))
{
    // Set-up UI
    setWindowTitle("MIN Test Framework Qt client");
    setGeometry(0,0,500,500);
    statusBar()->show();
    setupMenuBar();

    // Add the real window
    setCentralWidget(mainWidget_);
}
// -----------------------------------------------------------------------------
Min::MainWindow::~MainWindow()
{ ; }
// -----------------------------------------------------------------------------
void Min::MainWindow::setupMenuBar()
{
    QMenu *menu = menuBar()->addMenu (tr("&File"));
    menu = menuBar()->addMenu (tr("&Help"));
    QAction *action = menu->addAction(tr("About"));
    connect (action,SIGNAL(triggered(bool)),
            this,SLOT(displayAboutDialog()));
}
// -----------------------------------------------------------------------------
void Min::MainWindow::displayAboutDialog()
{
    // FIXME: shoud really display about dialog
    qDebug("About");
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
