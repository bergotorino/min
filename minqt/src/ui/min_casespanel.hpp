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
#include <QTabWidget>

// Forward declarations
class QResizeEvent;
class QToolBox;
class QTableView;
class QAbstractItemModel;
class QItemSelectionModel;
class QSortFilterProxyModel;
class QTreeView;

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    class Database;
    class ExecutedTab;
    // -------------------------------------------------------------------------
    /**
     * @class CasesPanel
     * @brief Occupies main area and displays lists of test cases.
     */
    class CasesPanel: public QWidget
    {
    Q_OBJECT
    Q_DISABLE_COPY(CasesPanel);
    public:
        /** Default C++ Constructor. */
        CasesPanel(QWidget *parent);

        /** Destructor. */
        ~CasesPanel();
    public:

        /** Gets information about selected test cases from available cases view
         *  @return pointer to the QSelectionItemModel
         */
        QItemSelectionModel* getSelectionFromAvailableCasesView();
        /** Gets information about selected test cases from ongoing cases view
         *  @return pointer to the QSelectionItemModel
         */
        QItemSelectionModel* getSelectionFromOngoingCasesView();

    protected:
        /** Handles resize event that comes from the system. */
        void resizeEvent (QResizeEvent *event);

    private slots:
        /** Updates the view */
        void updateAvailableView();
        /** Hides columns we do not wish to display */
        void hideViewColumns();
    private:
        /** Main area on the window. */
        QTabWidget *centralWidget_;

	/** Will display modules and cases we can run. */
        QTableView *availableCasesView_;

        /** Executed tab. Will display executed cases view and list
         *  of pronts from test cases.
         */
        Min::ExecutedTab *executedTab_;

        /** Model that feeds available cases view with the data. */
        QAbstractItemModel *availableCasesModel_;

        /** Model that feeds available cases view with the data. */
        QAbstractItemModel *testRunsModel_;

        /** Proxy model fo filterout available test cases */
        QSortFilterProxyModel *availableProxy_;

        /** Handler to the database */
        Min::Database &db_;
      
        /* For Summary view 
	    QTreeView *testRunTree_; */
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_CASESPANEL_HPP (file created by generator.sh v1.08)

