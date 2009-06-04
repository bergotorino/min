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
#include <QShortcut>

// Min includes
#include "min_mainwidget.hpp"
#include "min_aboutdialog.hpp"
#include "min_remotecontroll.hpp"
#include "min_database.hpp"
#include "min_addmoduledialog.hpp"
#include "min_statusbarprovider.hpp"

// -----------------------------------------------------------------------------
Min::MainWindow::MainWindow()
    : QMainWindow()
    , mainWidget_(new Min::MainWidget(this))
    , toolBar_(NULL)
{
    // Init statusbar provider
    Min::StatusBarProvider &sbp = Min::StatusBarProvider::getInstance();
    sbp.setStatusBar(statusBar());

    // Set-up UI
    setWindowTitle("MIN Test Framework Qt client");
    setGeometry(0,0,700,500);
    statusBar()->show();
    setupMenuBar();
    setupToolBar();

    // Shortcuts
    QShortcut *menuBarShortcut       = new QShortcut(QKeySequence("Ctrl+M"),this);
    QShortcut *runCasesShortcut      = new QShortcut(QKeySequence("Ctrl+S"),this);
    QShortcut *addTestModuleShortcut = new QShortcut(QKeySequence("Ctrl+O"),this);
    QShortcut *pauseTestModuleShortcut = new QShortcut(QKeySequence("Ctrl+P"),this);
    QShortcut *resumeTestModuleShortcut = new QShortcut(QKeySequence("Ctrl+R"),this);
    QShortcut *abortTestModuleShortcut = new QShortcut(QKeySequence("Ctrl+C"),this);

    // Add the real window
    setCentralWidget(mainWidget_);


    // Signals:
    connect (menuBarShortcut,SIGNAL(activated()),
             this,SLOT(toggleToolBar()));
    connect (runCasesShortcut,SIGNAL(activated()),
             this,SLOT(handleRunTestCase()));
    connect (addTestModuleShortcut,SIGNAL(activated()),
             this,SLOT(displayAddModuleDialog()));
    connect (pauseTestModuleShortcut,SIGNAL(activated()),
             this,SLOT(handlePauseTestCase()));
    connect (resumeTestModuleShortcut,SIGNAL(activated()),
             this,SLOT(handleResumeTestCase()));
    connect (abortTestModuleShortcut,SIGNAL(activated()),
             this,SLOT(handleAbortTestCase()));
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
    toolBar_ = addToolBar("Min actions");
    QAction *runaction = toolBar_->addAction(QIcon(":icons/agt_runit.png"),
					     "Run Selected Test Cases");
    QAction *addmoduleaction = toolBar_->addAction(QIcon(":icons/db_add.png"),
                                            "Add Test Module");
    QAction *pausecaseaction = toolBar_->addAction(QIcon(":icons/agt_pause_queue.png"), "Pause Test Case");
    QAction *resumecaseaction = toolBar_->addAction(QIcon(":icons/agt_resume.png"), "Resume Test Case");
    QAction *abortcaseaction = toolBar_->addAction(QIcon(":icons/agt_stop1.png"), "Abort Test Case");

    // Connect buttons with signals
    connect (runaction,SIGNAL(triggered(bool)),
            this,SLOT(handleRunTestCase()));
    connect (addmoduleaction,SIGNAL(triggered(bool)),
            this,SLOT(displayAddModuleDialog()));
    connect (pausecaseaction,SIGNAL(triggered(bool)),
            this,SLOT(handlePauseTestCase()));
    connect (resumecaseaction,SIGNAL(triggered(bool)),
            this,SLOT(handleResumeTestCase()));
    connect (abortcaseaction,SIGNAL(triggered(bool)),
            this,SLOT(handleAbortTestCase()));
}
// -----------------------------------------------------------------------------
void Min::MainWindow::toggleToolBar()
{
        if (toolBar_->isHidden()) toolBar_->show();
        else toolBar_->hide();
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
    //qDebug("Add Module");
    Min::AddModuleDialog dlg(this);
    connect (&dlg,SIGNAL(moduleAndTestCaseFiles(const QStringList&)),
            this,SLOT(handleAddModuleDialogAccepted(const QStringList&)));
    dlg.exec();
}
// -----------------------------------------------------------------------------
void Min::MainWindow::handleAddModuleDialogAccepted(const QStringList &data)
{
    //qDebug("Min::MainWindow::handleAddModuleDialogAccepted");
    if (!data.count()) return;

    Min::RemoteControll &rc = Min::RemoteControll::getInstance();

    rc.setTestCaseFiles(data);

    // Add module
    rc.minAddTestModule(data[0]);
}
// -----------------------------------------------------------------------------
void Min::MainWindow::handleRunTestCase()
{
    // Obtain selection model for available test cases
    const QItemSelectionModel *selection =
                                mainWidget_->getSelectedAvailableTestCases();

    if (!selection->hasSelection()) return;

    // We obtain list of indexes that describe selected rows
    QModelIndexList modules = selection->selectedRows(4);
    QModelIndexList cases   = selection->selectedRows(1);
    Min::Database &db       = Min::Database::getInstance();
    unsigned int groupId = db.getGroup();

    // For each selected, execute it!
    Min::RemoteControll &rc = Min::RemoteControll::getInstance();
    for (int i = 0; i < modules.count(); i++) {
	unsigned int moduleId = modules[i].data().toUInt();
        unsigned int caseId     = db.getTestCaseDbId(moduleId,cases[i].data().toString());

        rc.minStartCase(db.getModuleEngineId(1,moduleId),
                        db.getTestCaseEngineId(moduleId,caseId),
                        groupId); /* groupId to be extracted from database, for now just put 1 */
    }

}
// -----------------------------------------------------------------------------
void Min::MainWindow::handlePauseTestCase()
{
    // Obtain selection model for ongoing test cases
    const QItemSelectionModel *selection =
    mainWidget_->getSelectedOngoingTestCases();
    QModelIndexList ongoingCases   = selection->selectedRows(8);

    qDebug("Pause test case");

    if (!selection->hasSelection()) {
	    qDebug("No selection");
	    return;
    }
    Min::RemoteControll &rc = Min::RemoteControll::getInstance();

    qDebug("Pause test case - has selection");
    Min::Database &db = Min::Database::getInstance();

    for (int i = 0; i < ongoingCases.count(); i++) {
	    unsigned testrundbid = ongoingCases[i].data().toUInt();
	    unsigned testrunid = db.getTestRunEngineId (testrundbid); 
	    qDebug("Test run (db)id:  (%u)%u", testrundbid, testrunid);
	    
	    rc.minPauseCase (testrunid);
    }
    qDebug("Pause test case - 2");
}
// -----------------------------------------------------------------------------
void Min::MainWindow::handleResumeTestCase()
{
    // Obtain selection model for ongoing test cases
    const QItemSelectionModel *selection =
    mainWidget_->getSelectedOngoingTestCases();
    QModelIndexList ongoingCases   = selection->selectedRows(8);

    qDebug("Resume test case");

    if (!selection->hasSelection()) {
	    qDebug("No selection");
	    return;
    }
    Min::RemoteControll &rc = Min::RemoteControll::getInstance();

    qDebug("Resume test case - has selection");
    Min::Database &db = Min::Database::getInstance();

    for (int i = 0; i < ongoingCases.count(); i++) {
	    unsigned testrundbid = ongoingCases[i].data().toUInt();
	    unsigned testrunid = db.getTestRunEngineId (testrundbid); 
	    qDebug("Test run (db)id:  (%u)%u", testrundbid, testrunid);
	    
	    rc.minResumeCase (testrunid);
    }
	
}
// -----------------------------------------------------------------------------
void Min::MainWindow::handleAbortTestCase()
{
    // Obtain selection model for ongoing test cases
    const QItemSelectionModel *selection =
    mainWidget_->getSelectedOngoingTestCases();
    QModelIndexList ongoingCases   = selection->selectedRows(8);

    qDebug("Abort test case");

    if (!selection->hasSelection()) {
	    qDebug("No selection");
	    return;
    }
    Min::RemoteControll &rc = Min::RemoteControll::getInstance();

    qDebug("Abort test case - has selection");
    Min::Database &db = Min::Database::getInstance();

    for (int i = 0; i < ongoingCases.count(); i++) {
	    unsigned testrundbid = ongoingCases[i].data().toUInt();
	    unsigned testrunid = db.getTestRunEngineId (testrundbid); 
	    qDebug("Test run (db)id:  (%u)%u", testrundbid, testrunid);
	    
	    rc.minAbortCase (testrunid);
    }

	
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
