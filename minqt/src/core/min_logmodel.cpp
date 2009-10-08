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

// System includes
#include <QSize>
#include <QDateTime>
// Module include
#include "min_logmodel.hpp"
#include "min_descriptionprovider.hpp"
#include "tmc_common.h"
#include "min_common.h"



// -----------------------------------------------------------------------------
Min::LogModel::LogModel(QObject *parent)
    : QAbstractTableModel(parent)
    , db_(Min::Database::getInstance())
    , data_()
{ ; }
// -----------------------------------------------------------------------------
Min::LogModel::~LogModel()
{ ; }
// -----------------------------------------------------------------------------
int Min::LogModel::rowCount(const QModelIndex &parent) const
{
    return data_.count();
}
// -----------------------------------------------------------------------------
int Min::LogModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}
// -----------------------------------------------------------------------------
QVariant Min::LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (role==Qt::DisplayRole) {
	    if (column==0) return data_[row][column];
	    if (column==1) return data_[row][column];
	    if (column==2) return data_[row][column];
	    if (column==3) return QDateTime::fromTime_t(data_[row][column].toUInt()).toString();
    } else if (role==Qt::UserRole) {
        data_[row][column].toInt();
    }
    return QVariant();
}
// -----------------------------------------------------------------------------
QVariant Min::LogModel::headerData(int section,
				   Qt::Orientation orientation,
				   int role) const
{
    if (orientation==Qt::Horizontal) {
        if (role==Qt::DisplayRole ) {
            if (section==0) return "Id";
            if (section==1) return "Log Message";
            if (section==2) return "Log Type";
            if (section==3) return "Time Stamp";

        }
        return QVariant();
    } else {
        if (role!=Qt::DisplayRole) return QVariant();
        return section+1;
    }
}
// -----------------------------------------------------------------------------
void Min::LogModel::updateModelData()
{
    emit layoutAboutToBeChanged();
    data_ = db_.getLogMessages();
    emit layoutChanged();
}
// -----------------------------------------------------------------------------
