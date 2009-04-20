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
 * @file:     min_availablemodel.hpp
 * @version:  1.00
 * @date:     20.04.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_AVAILABLEMODEL_HPP
#define INCLUDED_MIN_AVAILABLEMODEL_HPP

// System includes
#include <QAbstractTableModel>

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class AvailableModel
     * @brief
     */
    class AvailableModel: public QAbstractTableModel
    {
    Q_OBJECT
    public:
        /** Default C++ Constructor. */
        AvailableModel(QObject *parent);

        /** Destructor. */
        ~AvailableModel();
    
    public:

        /**@{ Inherited from QAbstractTableModel */
        int rowCount(const QModelIndex &parent=QModelIndex()) const;

        int columnCount(const QModelIndex &parent=QModelIndex()) const;

        QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

        QVariant headerData(int section,
                            Qt::Orientation orientation,
                            int role=Qt::DisplayRole) const;
        /**@}*/

    private:
        /** Make it noncopyable */
        Q_DISABLE_COPY(AvailableModel)
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_AVAILABLEMODEL_HPP (file created by generator.sh v1.08)

