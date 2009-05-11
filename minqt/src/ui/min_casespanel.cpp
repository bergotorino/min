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
#include "min_common.h"
#include "tmc_common.h"
#include "min_descriptionprovider.hpp"
#include "min_executedtab.hpp"

// -----------------------------------------------------------------------------
Min::CasesPanel::CasesPanel(QWidget *parent)
    : QWidget(parent)
    , centralWidget_(new QToolBox(this))
    , availableCasesView_(new QTableView(this))
    , executedTab_(new Min::ExecutedTab(this))
    , availableCasesModel_(new Min::AvailableModel(this))
    , availableProxy_(new QSortFilterProxyModel(this))
    , db_(Min::Database::getInstance())
{
    // Proxies
    availableProxy_->setSourceModel(availableCasesModel_);

    // Available cases view
    availableCasesView_->setShowGrid(false);
    availableCasesView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    availableCasesView_->setModel(availableProxy_);

    // Main pane
    centralWidget_->addItem(availableCasesView_,QString("Available Cases"));
    centralWidget_->addItem(executedTab_,QString("Executed Cases"));
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
QItemSelectionModel* Min::CasesPanel::getSelectionFromOngoingCasesView()
{
    return executedTab_->getSelectionFromOngoingCasesView();
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
