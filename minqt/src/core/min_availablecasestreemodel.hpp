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
 * @file:     min_availablecasestreemodel.hpp
 * @version:  1.00
 * @date:     19.03.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_AVAILABLECASESTREEMODEL_HPP
#define INCLUDED_MIN_AVAILABLECASESTREEMODEL_HPP

// System includes
#include <QAbstractItemModel>
#include <QList>
#include <QVariant>

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class AvailableCasesTreeModel
     * @brief
     */
    class TreeItem
    {
    public:
        /** C++ Constructor. */
        TreeItem(const QList<QVariant> &data, Min::TreeItem *parent = 0);

        /** Destructor. */
        ~TreeItem();

        /** Appends child item to particular item.
         *  @param child item to be appended as child
         */
        void appendChild(Min::TreeItem *child);

        /** Gets child item from given row.
         *  @param row for which child is returned.
         *  @return pointer to child item
         */
        Min::TreeItem* child(int row);

        /** Counts child items.
         *  @return number of child items.
         */
        int childCount() const;

        /** Counts columns.
         *  @return number of columns with data
         */
        int columnCount() const;

        /** Gets data for given column.
         *  @param column for which data will be returned.
         *  @retun data for specified column
         */
        QVariant data(int column) const;

        /** Gives current row.
         *  @return row number for current item.
         */
        int row() const;

        /** Gets parent for particular item.
         *  @return pointer to parent item.
         */
        Min::TreeItem* parent() const;

    private:
        /** Data kept by item */
        QList<QVariant> data_;

        /** Child items to particular item. */
        QList<Min::TreeItem*> child_;

        /** Parent item. */
        Min::TreeItem *parent_;
    };
    // -------------------------------------------------------------------------
    /**
     * @class AvailableCasesTreeModel
     * @brief Tree model that will supply the view with module/cases tree
     * structure
     */
    class AvailableCasesTreeModel: public QAbstractItemModel
    {
    Q_OBJECT
    public:
        /** Default C++ Constructor. */
        AvailableCasesTreeModel(QObject *parent);

        /** Destructor. */
        ~AvailableCasesTreeModel();

        /**@{Inherited from QAbstractImetModel */
        QVariant data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;

        QModelIndex index(int row, int column,
                        const QModelIndex &parent = QModelIndex()) const;

        QModelIndex parent(const QModelIndex &index) const;

        int rowCount(const QModelIndex &parent = QModelIndex()) const;

        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        /**@}*/

    protected:
    private:
        /**@{Declared but not defined by design. */
        /** Copy Constructor. */
        AvailableCasesTreeModel( const Min::AvailableCasesTreeModel& c );
        /** Assignment operator. */
        Min::AvailableCasesTreeModel& operator =(const Min::AvailableCasesTreeModel& c);
        /**@}*/

        /** Creates the tree. */
        void setupModelData();

        /** Root of the tree. */
        Min::TreeItem *root_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_AVAILABLECASESTREEMODEL_HPP (file created by generator.sh v1.08)

