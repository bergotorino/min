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
 * @file:     min_mainwidget.cpp
 * @version:  1.00
 * @date:     18.03.2009
 * @author:   
 */

// Module include
#include "min_mainwidget.hpp"

// System includes
#include <QListView>
#include <QToolBox>
#include <QLabel>
#include <QSplitter>
#include <QResizeEvent>
#include <QAbstractListModel>
#include <QStringListModel>
#include <QTableView>
#include <QTabWidget>

// Min includes
#include "min_devicepanel.hpp"
#include "min_casespanel.hpp"

// -----------------------------------------------------------------------------
Min::MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , splitter_(new QSplitter(this))
    , leftPanel_(new Min::DevicePanel(this))
    , mainPanel_(new Min::CasesPanel(this))
{
    // Left panel

    // Main panel

    // Layout items
    splitter_->addWidget(leftPanel_);
    splitter_->addWidget(mainPanel_);
}
// -----------------------------------------------------------------------------
Min::MainWidget::~MainWidget()
{ ; }
// -----------------------------------------------------------------------------
QItemSelectionModel* Min::MainWidget::getSelectedAvailableTestCases() const
{
    return mainPanel_->getSelectionFromAvailableCasesView();
}
// -----------------------------------------------------------------------------
void Min::MainWidget::resizeEvent ( QResizeEvent * event )
{
    splitter_->resize(event->size());
    QWidget::resizeEvent(event);
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
