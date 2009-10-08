/**
 * @file:     min_logtab.hpp
 * @version:  1.00
 * @date:     07.10.2009
 * @author:   Sampo Saaristo
 */

#ifndef INCLUDED_MIN_LOGTAB_HPP
#define INCLUDED_MIN_LOGTAB_HPP

// System includes
#include <QWidget>
#include <QStringListModel>

// Min includes

// Forward declarations
class QResizeEvent;
class QTableView;
class QTabWidget;
class QAbstractTableModel;
class QSortFilterProxyModel;
class QSplitter;

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
     * @class LogTab
     * @brief
     */
    class LogTab: public QWidget
    {
    Q_OBJECT
    Q_DISABLE_COPY(LogTab)
    public:
        /** Default C++ Constructor. */
        LogTab(QWidget *parent);

        /** Destructor. */
        ~LogTab();

    protected:
        /** Handles resize event that comes from the system. */
	 void resizeEvent (QResizeEvent *event);		 

    private slots:
         void hideViewColumns();

    private:

        /** Feeds view with list of log messages */
        QAbstractItemModel *logMsgModel_;

        /** Tab widget for messages */
        QTabWidget *logView_;

        /** Will display all log messages  */
        QTableView *logAll_;

        /** Will display all ongoing cases */
        QTableView *logErrors_;

        /* Proxy model to filter out error messaged */
        QSortFilterProxyModel *errorProxy_;

        /** Handler to the database */
        Min::Database &db_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_LOGTAB_HPP 

