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

// System includes
#include <QSize>
#include <QDateTime>
// Module include
#include "min_executedmodel.hpp"
#include "min_descriptionprovider.hpp"
#include "tmc_common.h"
#include "min_common.h"



// -----------------------------------------------------------------------------
Min::ExecutedModel::ExecutedModel(QObject *parent)
    : QAbstractTableModel(parent)
    , db_(Min::Database::getInstance())
    , data_()
{ ; }
// -----------------------------------------------------------------------------
Min::ExecutedModel::~ExecutedModel()
{ ; }
// -----------------------------------------------------------------------------
int Min::ExecutedModel::rowCount(const QModelIndex &parent) const
{
    return data_.count();
}
// -----------------------------------------------------------------------------
int Min::ExecutedModel::columnCount(const QModelIndex &parent) const
{
    return 10;
}
// -----------------------------------------------------------------------------
QVariant Min::ExecutedModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (role==Qt::DisplayRole) {
	    if (column==0) return data_[row][column];
	    if (column==1) return data_[row][column];
	    if (column==2) return data_[row][column];
	    if (column==3) return Min::DescriptionProvider::getTestCaseStatusDescription(data_[row][column].toInt(), data_[row][6].toInt());
	    if (column==4) return QDateTime::fromTime_t(data_[row][column].toUInt()).toString();
	    if (column==5) return QDateTime::fromTime_t(data_[row][column].toUInt()).toString();
	    if (column==6) return Min::DescriptionProvider::getTestCaseResultDescription(data_[row][column].toInt());
	    if (column==7) return data_[row][column];
	    if (column==8) return data_[row][column];
	    if (column==9) return data_[row][column];
    } else if (role==Qt::UserRole) {
        data_[row][column].toInt();
    }
    return QVariant();
}
// -----------------------------------------------------------------------------
QVariant Min::ExecutedModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const
{
    if (orientation==Qt::Horizontal) {
        if (role==Qt::DisplayRole ) {
            if (section==0) return "Test Case Name";
            if (section==1) return "Test Case Description";
            if (section==2) return "Group ID";
            if (section==3) return "Status";
            if (section==4) return "Start Time";
            if (section==5) return "End Time";
            if (section==6) return "Result";
            if (section==7) return "Result Description";
            if (section==8) return "Test Run DB-ID";
            if (section==9) return "Device DB-ID";
        }
        return QVariant();
    } else {
        if (role!=Qt::DisplayRole) return QVariant();
        return section+1;
    }
}
// -----------------------------------------------------------------------------
void Min::ExecutedModel::updateModelData()
{
    emit layoutAboutToBeChanged();
    data_ = db_.getExecutedView(1);
    emit layoutChanged();
}
// -----------------------------------------------------------------------------
