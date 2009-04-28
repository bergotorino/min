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
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <QCoreApplication>

// Min includes
#include "min_mainwidget.hpp"
#include "min_aboutdialog.hpp"

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
    setupToolBar();

    // Add the real window
    setCentralWidget(mainWidget_);
}
// -----------------------------------------------------------------------------
Min::MainWindow::~MainWindow()
{ ; }
// -----------------------------------------------------------------------------
void Min::MainWindow::setupMenuBar()
{
    QMenu *file_menu = menuBar()->addMenu (tr("&File"));
    QMenu *help_menu = menuBar()->addMenu (tr("&Help"));
    QAction *exit_action = file_menu->addAction(tr("Exit"));
    QAction *about_action = help_menu->addAction(tr("About"));
    connect (about_action,SIGNAL(triggered(bool)),
            this,SLOT(displayAboutDialog()));
    connect (exit_action, SIGNAL(triggered(bool)),
	    QCoreApplication::instance(), SLOT(quit()));
}
// -----------------------------------------------------------------------------
void Min::MainWindow::setupToolBar()
{
    QToolBar *tlb = addToolBar("Min actions");
    QAction *runaction = tlb->addAction(QIcon("/home/user/work/gmo/min/trunk/minqt/icons/clicknrun.png"),
                                        "Run Selected Test Cases");
    QAction *addmoduleaction = tlb->addAction(QIcon("/home/user/work/gmo/min/trunk/minqt/icons/db_add.png"),
                                            "Add Test Module");

    connect (addmoduleaction,SIGNAL(triggered(bool)),
            this,SLOT(displayAddModuleDialog()));
}
// -----------------------------------------------------------------------------
void Min::MainWindow::displayAboutDialog()
{
    AboutDialog *ad=new AboutDialog(this);
    ad->show();
}
// -----------------------------------------------------------------------------
void Min::MainWindow::displayAddModuleDialog()
{
    qDebug("Add Module");
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
