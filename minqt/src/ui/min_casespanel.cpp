/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 * @file:     min_casespanel.cpp
 * @version:  1.00
 * @date:     19.03.2009
 * @author:   
 */

// Module include
#include "min_casespanel.hpp"

// System includes
#include <QToolBox>
#include <QTableView>
#include <QTabWidget>
#include <QLabel>
#include <QResizeEvent>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QTreeView>
#include <QSortFilterProxyModel>

// Min incudes
#include "min_casesmodel.hpp"
#include "min_availablecasestreemodel.hpp"
#include "min_executedmodel.hpp"

#include "min_availablemodel.hpp"
#include "min_database.hpp"

// -----------------------------------------------------------------------------
Min::CasesPanel::CasesPanel(QWidget *parent)
    : QWidget(parent)
    , centralWidget_(new QToolBox(this))
    , availableCasesView_(new QTableView(this))
    , executedCasesModel_(new Min::ExecutedModel(this))
    , availableCasesModel_(new Min::AvailableModel(this))
    , executedCasesView_(new QTabWidget(this))
    , executedTable_(new QTableView(this))
    , ongoingTable_(new QTableView(this))
    , passedTable_(new QTableView(this))
    , failedTable_(new QTableView(this))
    , abortedTable_(new QTableView(this))
    , availableProxy_(new QSortFilterProxyModel(this))
    , ongoingProxy_(new QSortFilterProxyModel(this))
    , passedProxy_(new QSortFilterProxyModel(this))
    , failedProxy_(new QSortFilterProxyModel(this))
    , abortedProxy_(new QSortFilterProxyModel(this))
    , db_(Min::Database::getInstance())
{
    // Available cases view
    availableCasesView_->setShowGrid(false);
    availableCasesView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    availableProxy_->setSourceModel(availableCasesModel_);
    ongoingProxy_->setSourceModel(executedCasesModel_);
    passedProxy_->setSourceModel(executedCasesModel_);
    failedProxy_->setSourceModel(executedCasesModel_);
    abortedProxy_->setSourceModel(executedCasesModel_);

    availableCasesView_->setModel(availableProxy_);

    executedTable_->setModel(executedCasesModel_);
    executedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    executedTable_->setColumnHidden(1, true);
    executedTable_->setColumnHidden(2, true);
    executedTable_->setColumnHidden(4, true);
    executedTable_->setColumnHidden(5, true);
    executedTable_->setColumnHidden(6, true);
    executedTable_->setColumnHidden(7, true);
    executedTable_->setColumnHidden(8, true);
    executedTable_->setColumnHidden(9, true);

    ongoingTable_->setModel(executedCasesModel_);
    ongoingTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ongoingTable_->setColumnHidden(1,true);
    ongoingTable_->setColumnHidden(2,true);
    ongoingTable_->setColumnHidden(5,true);
    ongoingTable_->setColumnHidden(6,true);
    ongoingTable_->setColumnHidden(7,true);
    ongoingTable_->setColumnHidden(8,true);
    ongoingTable_->setColumnHidden(9,true);

    passedTable_->setModel(executedCasesModel_);
    passedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    passedTable_->setColumnHidden(1,true);
    passedTable_->setColumnHidden(2,true);
    passedTable_->setColumnHidden(3,true);
    passedTable_->setColumnHidden(8,true);
    passedTable_->setColumnHidden(9,true);

    failedTable_->setModel(executedCasesModel_);
    failedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    failedTable_->setColumnHidden(1,true);
    failedTable_->setColumnHidden(2,true);
    failedTable_->setColumnHidden(3,true);
    failedTable_->setColumnHidden(8,true);
    failedTable_->setColumnHidden(9,true);

    abortedTable_->setModel(executedCasesModel_);
    abortedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    abortedTable_->setColumnHidden(1,true);
    abortedTable_->setColumnHidden(2,true);
    abortedTable_->setColumnHidden(3,true);
    abortedTable_->setColumnHidden(8,true);
    abortedTable_->setColumnHidden(9,true);
    
    // Executed cases view
    executedCasesView_->addTab(executedTable_,"All");
    executedCasesView_->addTab(ongoingTable_,"Ongoing");
    executedCasesView_->addTab(passedTable_,"Passed");
    executedCasesView_->addTab(failedTable_,"Failed");
    executedCasesView_->addTab(abortedTable_,"Aborted");

    // Main pane
    centralWidget_->addItem(availableCasesView_,QString("Available Cases"));
    centralWidget_->addItem(executedCasesView_,QString("Executed Cases"));
    centralWidget_->addItem(new QLabel("Pie chart goes here!"),
                                        QString("Summary"));

    // Signals and slots
    connect (&db_,SIGNAL(updated()),
             availableCasesModel_,SLOT(updateModelData()));
}
// -----------------------------------------------------------------------------
Min::CasesPanel::~CasesPanel()
{ ; }
// -----------------------------------------------------------------------------
QItemSelectionModel* Min::CasesPanel::getSelectionFromAvailableCasesView()
{
    return availableCasesView_->selectionModel();
}
// -----------------------------------------------------------------------------
void Min::CasesPanel::resizeEvent(QResizeEvent *event)
{
    centralWidget_->resize(event->size());
    QWidget::resizeEvent(event);
}
// -----------------------------------------------------------------------------
void Min::CasesPanel::updateAvailableView()
{
    qDebug("Updated!!!\n");
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
