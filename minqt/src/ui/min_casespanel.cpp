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
#include "min_availablemodel.hpp"

// -----------------------------------------------------------------------------
Min::CasesPanel::CasesPanel(QWidget *parent)
    : QWidget(parent)
    , centralWidget_(new QToolBox(this))
    , availableCasesView_(new QTableView(this))
    , availableCasesModel_(new Min::AvailableModel(this))
    , executedCasesView_(new QTabWidget(this))
    , executedTable_(new QTableView(this))
{
    // Available cases view
//    availableCasesView_->setModel(availableCasesModel_);
    availableCasesView_->setShowGrid(false);
    QSortFilterProxyModel *proxyView= new QSortFilterProxyModel(this);
    proxyView->setSourceModel(availableCasesModel_);
    availableCasesView_->setModel(proxyView);
    
    // Executed cases view
    executedCasesView_->addTab(executedTable_,"All");
    executedCasesView_->addTab(new QLabel(),"Ongoing");
    executedCasesView_->addTab(new QLabel(),"Passed");
    executedCasesView_->addTab(new QLabel(),"Failed");
    executedCasesView_->addTab(new QLabel(),"Aborted");

    // Main pane
    centralWidget_->addItem(availableCasesView_,QString("Available Cases"));
    centralWidget_->addItem(executedCasesView_,QString("Executed Cases"));
    centralWidget_->addItem(new QLabel("Pie chart goes here!"),
                                        QString("Summary"));

    // Signals and slots
    connect (availableCasesView_,SIGNAL(clicked(const QModelIndex&)),
            this,SLOT(availableViewSetRowSelection(const QModelIndex&)));
}
// -----------------------------------------------------------------------------
Min::CasesPanel::~CasesPanel()
{ ; }
// -----------------------------------------------------------------------------
void Min::CasesPanel::resizeEvent(QResizeEvent *event)
{
    centralWidget_->resize(event->size());
    QWidget::resizeEvent(event);
}
// -----------------------------------------------------------------------------
void Min::CasesPanel::availableViewSetRowSelection(const QModelIndex &index)
{
    if (!index.isValid()) return;
    availableCasesView_->selectRow(index.row());
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
