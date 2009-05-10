/**
 * @file:     min_executedtab.hpp
 * @version:  1.00
 * @date:     10.05.2009
 * @author:   Konrad Marek Zapalowicz
 */

#ifndef INCLUDED_MIN_EXECUTEDTAB_HPP
#define INCLUDED_MIN_EXECUTEDTAB_HPP

// System includes
#include <QWidget>
#include <QStringListModel>

// Min includes

// Forward declarations
class QResizeEvent;
class QTableView;
class QListView;
class QTabWidget;
class QAbstractTableModel;
class QAbstractItemModel;
class QItemSelectionModel;
class QListView;
class QSplitter;
class QSortFilterProxyModel;

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    class Database;
    // -------------------------------------------------------------------------
    /**
     * @class ExecutedTab
     * @brief
     */
    class ExecutedTab: public QWidget
    {
    Q_OBJECT
    Q_DISABLE_COPY(ExecutedTab)
    public:
        /** Default C++ Constructor. */
        ExecutedTab(QWidget *parent);

        /** Destructor. */
        ~ExecutedTab();

    protected:
        /** Handles resize event that comes from the system. */
        void resizeEvent (QResizeEvent *event);

    private slots:
        /** Updates the view */
        void hideViewColumns();

    private:
        /** Splitter/layout of the widget */
        QSplitter *splitter_;

        /** Will display list of prints from test cases */
        QListView *printMsgView_;

        /** Will feed view with list of prints */
        QStringListModel printMsgModel_;

        /** Model that feeds executed cases view with the data. */
        QAbstractItemModel *executedCasesModel_;

        /** Will display cases that has been executed {ongoing,completed} */
        QTabWidget *executedCasesView_;

        /** Will display all executed cases */
        QTableView *executedTable_;

        /** Will display all ongoing cases */
        QTableView *ongoingTable_;

        /** Will display all passed cases */
        QTableView *passedTable_;

        /** Will display all failed cases */
        QTableView *failedTable_;

        /** Will display all aborted cases */
        QTableView *abortedTable_;

	    /* Proxy model fo filterout ongoing test cases */
	    QSortFilterProxyModel *ongoingProxy_;

	    /* Proxy model fo filterout passed test cases */
	    QSortFilterProxyModel *passedProxy_;

	    /* Proxy model fo filterout failedtest cases */
	    QSortFilterProxyModel *failedProxy_;

	    /* Proxy model fo filterout aborted test cases */
	    QSortFilterProxyModel *abortedProxy_;

        /** Handler to the database */
        Min::Database &db_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_EXECUTEDTAB_HPP (file created by generator.sh v1.08)

