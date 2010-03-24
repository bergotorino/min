/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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

/**
 * @file:     min_casesmodel.hpp
 * @version:  1.00
 * @date:     19.03.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_CASESMODEL_HPP
#define INCLUDED_MIN_CASESMODEL_HPP

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
     * @class CasesModel
     * @brief
     */
    class CasesModel: public QAbstractTableModel
    {
    public:
        /** Default C++ Constructor. */
        CasesModel(QObject *parent);

        /** Destructor. */
        ~CasesModel();

        /**@{Inherited from QAbstractTableModel.*/
        QVariant data (const QModelIndex &i, int r = Qt::DisplayRole) const;

        int columnCount (const QModelIndex &parent = QModelIndex()) const;

        int rowCount (const QModelIndex &parent = QModelIndex()) const;
        /**@}*/

    protected:
    private:
        /**@{Declared but not defined bby design. */
        /** Copy Constructor. */
        CasesModel( const Min::CasesModel& c );
        /** Assignment operator. */
        Min::CasesModel& operator =(const Min::CasesModel& c);
        /**@}*/
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_CASESMODEL_HPP (file created by generator.sh v1.08)

