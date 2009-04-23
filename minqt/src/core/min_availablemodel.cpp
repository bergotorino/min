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
 * @file:     min_availablemodel.cpp
 * @version:  1.00
 * @date:     20.04.2009
 * @author:   
 */

// System includes
#include <QSize>

// Module include
#include "min_availablemodel.hpp"

// -----------------------------------------------------------------------------
Min::AvailableModel::AvailableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , db_(Min::Database::getInstance())
{ ; }
// -----------------------------------------------------------------------------
Min::AvailableModel::~AvailableModel()
{ ; }
// -----------------------------------------------------------------------------
int Min::AvailableModel::rowCount(const QModelIndex &parent) const
{
    return 2;
}
// -----------------------------------------------------------------------------
int Min::AvailableModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}
// -----------------------------------------------------------------------------
QVariant Min::AvailableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    QStringList modules = db_.getModules(10);
    int row = index.row();
    int column = index.column();


    if (role==Qt::DisplayRole) {
        unsigned int mdbid = db_.getModuleDbId(10,modules[0]);
        QStringList tests = db_.getTestCases(mdbid);

        if (column==0) return modules[0];
        if (column==1) return "Test Case";
        return "Nothing";
    }
    return QVariant();
/*
    switch (role) {
    case Qt::DisplayRole:

        getModuleDbId(10,modules[0])
        QStringList getTestCases(unsigned int module_dbid);


        return modules[0];
        break;
    default:
        return QVariant();
    }
*/
}
// -----------------------------------------------------------------------------
QVariant Min::AvailableModel::headerData(int section,
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
// file created by generator.sh v1.08
