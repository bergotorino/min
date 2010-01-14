/**
 * @file:     min_logtab.cpp
 * @version:  1.00
 * @date:     07.10.2009
 * @author:   Sampo Saaristo
 */

// Module include
#include "min_logtab.hpp"

// System includes
#include <QTableView>
#include <QTabWidget>
#include <QLabel>
#include <QResizeEvent>
#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QListView>
#include <QHeaderView>
#include <QScrollBar>

// Min includes
#include "min_database.hpp"
#include "min_common.h"
#include "min_logmodel.hpp"

// -----------------------------------------------------------------------------
Min::LogTab::LogTab(QWidget *parent)
    : QWidget(parent)
    , logMsgModel_(new Min::LogModel(this))
    , logView_(new QTabWidget(this))
    , logAll_(new QTableView(this))
    , logErrors_(new QTableView(this))
    , errorProxy_(new QSortFilterProxyModel(this))
    , db_(Min::Database::getInstance())
{
    // Proxies for model
    errorProxy_->setSourceModel(logMsgModel_);
    errorProxy_->setFilterKeyColumn(2);
    errorProxy_->setFilterWildcard("error");

    // Prints

    logAll_->setModel(logMsgModel_);
    logAll_->resizeColumnsToContents();
    //    logAll_->horizontalHeader()->setStretchLastSection(true);
    logErrors_->setModel(errorProxy_);
    logErrors_->resizeColumnsToContents();
    //logErrors_->horizontalHeader()->setStretchLastSection(true);

    // Construct the log cases view

    logView_->addTab(logAll_,"All messages");
    logView_->addTab(logErrors_,"Error messages");

    // Signals and slots
    connect (logMsgModel_,SIGNAL(layoutChanged()),
	     this, SLOT(hideViewColumns()));

    connect (logMsgModel_,SIGNAL(layoutChanged()),
	     logAll_, SLOT(resizeColumnsToContents()));
    connect (logMsgModel_,SIGNAL(layoutChanged()),
	     logErrors_, SLOT(resizeColumnsToContents()));
    connect (&db_,SIGNAL(updated()),
             logMsgModel_,SLOT(updateModelData()));

}
// -----------------------------------------------------------------------------
void Min::LogTab::resizeEvent(QResizeEvent *event)
{
	logView_->resize(event->size());
	QWidget::resizeEvent(event);
}
// -----------------------------------------------------------------------------
void Min::LogTab::hideViewColumns() {
    logAll_->setColumnHidden(0, true);
    logErrors_->setColumnHidden(0, true);
}
// -----------------------------------------------------------------------------
Min::LogTab::~LogTab()
{ ; }
// -----------------------------------------------------------------------------
