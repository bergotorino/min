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
 * @file:     min_casespanel.hpp
 * @version:  1.00
 * @date:     19.03.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_CASESPANEL_HPP
#define INCLUDED_MIN_CASESPANEL_HPP

// System includes
#include <QWidget>

// Forward declarations
class QResizeEvent;
class QToolBox;
class QTableView;
class QTabWidget;
class QAbstractTableModel;
class QAbstractItemModel;
class QTreeView;

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class CasesPanel
     * @brief Occupies main area and displays lists of test cases.
     */
    class CasesPanel: public QWidget
    {
    public:
        /** Default C++ Constructor. */
        CasesPanel(QWidget *parent);

        /** Destructor. */
        ~CasesPanel();
    protected:
        /** Handles resize event that comes from the system. */
        void resizeEvent (QResizeEvent *event);
    private:
        /**@{Declared but not defined by design.*/
        /** Copy Constructor. */
        CasesPanel( const Min::CasesPanel& c );
        /** Assignment operator. */
        Min::CasesPanel& operator =( const Min::CasesPanel& c );
        /**@}*/
    private:
        /** Main area on the window. */
        QToolBox *centralWidget_;

        /** Will display modules and cases we can run. */
        QTreeView *availableCasesView_;

        /** Model that feeds available cases view with the data. */
        QAbstractItemModel *availableCasesModel_;

        /** Will display cases that has been executed {ongoing,completed} */
        QTabWidget *executedCasesView_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_CASESPANEL_HPP (file created by generator.sh v1.08)

