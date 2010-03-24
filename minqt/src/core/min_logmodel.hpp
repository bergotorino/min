/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
 * Contact e-mail: min-support@lists.sourceforge.net
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


#ifndef INCLUDED_MIN_LOGMODEL_HPP
#define INCLUDED_MIN_LOGMODEL_HPP

// System includes
#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>

// Min includes
#include "min_database.hpp"

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class LogModel
     * @brief
     */
    class LogModel: public QAbstractTableModel
    {
    Q_OBJECT
    Q_DISABLE_COPY(LogModel)
    public:
        /** Default C++ Constructor. */
        LogModel(QObject *parent);

        /** Destructor. */
        ~LogModel();
    
    public:

        /**@{ Inherited from QAbstractTableModel */
        int rowCount(const QModelIndex &parent=QModelIndex()) const;

        int columnCount(const QModelIndex &parent=QModelIndex()) const;

        QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

        QVariant headerData(int section,
                            Qt::Orientation orientation,
                            int role=Qt::DisplayRole) const;
        /**@}*/

    public slots:

        /** Makes the model to refetch its data from database and to notify the
         *  view to update itself.
         */
        void updateModelData();

    private:

        /** Handler to database connection for MIN */
        Min::Database &db_;

        /** Data to be displayed fetched from the database */
        mutable QVector<QStringList> data_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_LOGMODEL_HPP 

