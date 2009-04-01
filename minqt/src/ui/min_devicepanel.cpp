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
 * @file:     min_devicepanel.cpp
 * @version:  1.00
 * @date:     19.03.2009
 * @author:   
 */

// Module include
#include "min_devicepanel.hpp"

// System includes
#include <QListView>
#include <QStringListModel>
#include <QStringList>
#include <QResizeEvent>

// -----------------------------------------------------------------------------
Min::DevicePanel::DevicePanel(QWidget *parent)
        : QWidget(parent)
        , devicesList_(new QListView(this))
        , devicesModel_(new QStringListModel())
{
    setMinimumWidth(100);
    setMaximumWidth(100);

    // Model
    QStringList list;
    list << "localhost";
    devicesModel_->setStringList(list);

    // View
    //devicesList_->setMinimumWidth(100);
    //devicesList_->setMaximumWidth(150);
    devicesList_->setModel(devicesModel_);
}
// -----------------------------------------------------------------------------
Min::DevicePanel::~DevicePanel()
{ ; }
// -----------------------------------------------------------------------------
void Min::DevicePanel::resizeEvent (QResizeEvent *event)
{
    devicesList_->resize(event->size());
    QWidget::resizeEvent(event);
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
