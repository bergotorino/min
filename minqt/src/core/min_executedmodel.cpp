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

// Module include
#include "min_executedmodel.hpp"

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
    data_ = db_.getExecutedView(1);
    return data_.count();
}
// -----------------------------------------------------------------------------
int Min::ExecutedModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}
// -----------------------------------------------------------------------------
QVariant Min::ExecutedModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (role==Qt::DisplayRole) {
        if (row < data_.count() && column < 2 ) {
            return data_[row][column];
        } else return "Nothing";
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
            if (section==0) return "Module Name";
            if (section==1) return "Test Case Name";
            if (section==2) return "Description";
            if (section==3) return "Tags";
        }
        return QVariant();
    } else {
        if (role!=Qt::DisplayRole) return QVariant();
        return section+1;
    }
}
// -----------------------------------------------------------------------------
