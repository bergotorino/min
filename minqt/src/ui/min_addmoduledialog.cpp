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
 * @file:     min_addmoduledialog.cpp
 * @version:  1.00
 * @date:     27.04.2009
 * @author:   
 */

// Module include
#include "min_addmoduledialog.hpp"

// System includes
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>

// -----------------------------------------------------------------------------
Min::AddModuleDialog::AddModuleDialog(QWidget *parent)
    : QDialog(parent)
    , layout_(new QGridLayout(this))
    , moduleLabel_(new QLabel("Module",this))
    , moduleEdit_(new QLineEdit(this))
    , tcFilesLabel_(new QLabel("Test Case files",this))
    , tcView_(new QListView(this))
    , tcModel_()
    , testCaseFiles_()
    , pickModuleButton_(new QPushButton("Select module",this))
    , addTcFileButton_(new QPushButton("Add test case",this))
    , removeTcFileButton_(new QPushButton("Rm test case",this))
    , okButton_(new QPushButton("Ok",this))
    , koButton_(new QPushButton("Cancel",this))
{
    // Set-up dialog
    setModal(true);

    // Test Case files view
    tcModel_.setStringList(testCaseFiles_);
    tcView_->setModel(&tcModel_);
    tcView_->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Layout stuff
    layout_->addWidget(moduleLabel_,0,0);
    layout_->addWidget(moduleEdit_,0,1);
    layout_->addWidget(pickModuleButton_,0,2);
    layout_->addWidget(tcFilesLabel_,1,0,2,1);
    layout_->addWidget(tcView_,1,1,2,1);
    layout_->addWidget(addTcFileButton_,1,2);
    layout_->addWidget(removeTcFileButton_,2,2);
    layout_->addWidget(okButton_,3,1);
    layout_->addWidget(koButton_,3,2);

    // Signal and slots
    connect (pickModuleButton_,SIGNAL(clicked(bool)),
            this,SLOT(selectModule()));
    connect (addTcFileButton_,SIGNAL(clicked(bool)),
            this,SLOT(addTestCaseFile()));
    connect (removeTcFileButton_,SIGNAL(clicked(bool)),
            this,SLOT(removeTestCaseFile()));
    connect (okButton_,SIGNAL(clicked(bool)),
            this,SLOT(accept()));
    connect (koButton_,SIGNAL(clicked(bool)),
            this,SLOT(reject()));
}
// -----------------------------------------------------------------------------
Min::AddModuleDialog::~AddModuleDialog()
{ ; }
// -----------------------------------------------------------------------------
QString Min::AddModuleDialog::displayFileOpenDialog(const QString &title,
                                                    const QString &filter)
{
    return QFileDialog::getOpenFileName(this,
                                        title,
                                        QDir::homePath()+"/.min",
                                        filter);
}
// -----------------------------------------------------------------------------
void Min::AddModuleDialog::selectModule()
{
    QString moduleName = displayFileOpenDialog("Load test module",
                                                "Modules (*.so)");
    if (moduleName=="") return;
    moduleEdit_->setText(moduleName);
}
// -----------------------------------------------------------------------------
void Min::AddModuleDialog::addTestCaseFile()
{
    QString testCaseFile = displayFileOpenDialog("Add test case file",
                                                "Test case files (*.cfg *.lua)");
    if (testCaseFile=="") return;
    testCaseFiles_ << testCaseFile;
    tcModel_.setStringList(testCaseFiles_);
}
// -----------------------------------------------------------------------------
void Min::AddModuleDialog::removeTestCaseFile()
{
    QItemSelectionModel *selection = tcView_->selectionModel();
    if (!selection->hasSelection()) return;
    QModelIndexList rows = selection->selectedRows();
    for (int i=0;i<rows.count();i++) {
        testCaseFiles_.removeOne(rows[i].data().toString());
    }
    tcModel_.setStringList(testCaseFiles_);
}
// -----------------------------------------------------------------------------
void Min::AddModuleDialog::accept()
{
    testCaseFiles_.prepend(moduleEdit_->text());
    emit moduleAndTestCaseFiles(testCaseFiles_);

    QDialog::accept();
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
