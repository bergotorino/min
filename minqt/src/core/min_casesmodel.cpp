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
 * @file:     min_casesmodel.cpp
 * @version:  1.00
 * @date:     19.03.2009
 * @author:   
 */

// Module include
#include "min_casesmodel.hpp"

// System includes

// Min includes

// -----------------------------------------------------------------------------
Min::CasesModel::CasesModel(QObject *parent)
    : QAbstractTableModel(parent)
{ ; }
// -----------------------------------------------------------------------------
Min::CasesModel::~CasesModel()
{ ; }
// -----------------------------------------------------------------------------
QVariant Min::CasesModel::data (const QModelIndex &index,
                                        int role) const
{
    if (!index.isValid()) return QVariant();

    // FIXME: stub code
    switch (role) {
    case Qt::DisplayRole:
        if (index.column()==0) return QString("Modulename");
        else return QString("Casename ");
        break;
    default:
        return QVariant();
    }
}
// -----------------------------------------------------------------------------
int Min::CasesModel::columnCount (const QModelIndex &/*parent*/) const
{
    // FIXME: stub code
    return 2;
}
// -----------------------------------------------------------------------------
int Min::CasesModel::rowCount (const QModelIndex &/*parent*/) const
{
    // FIXME: stub code
    return 4;
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
