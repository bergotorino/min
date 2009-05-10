/**
 * @file:     min_executedtab.cpp
 * @version:  1.00
 * @date:     10.05.2009
 * @author:   Konrad Marek Zapalowicz
 */

// Module include
#include "min_executedtab.hpp"

// System includes
#include <QToolBox>
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

// Min includes
#include "min_executedmodel.hpp"
#include "min_database.hpp"
#include "min_common.h"
#include "tmc_common.h"
#include "min_descriptionprovider.hpp"

// -----------------------------------------------------------------------------
Min::ExecutedTab::ExecutedTab(QWidget *parent)
    : QWidget(parent)
    , splitter_(new QSplitter(Qt::Horizontal,this))
    , printMsgView_(new QListView(this))
    , printMsgModel_()
    , executedCasesModel_(new Min::ExecutedModel(this))
    , executedCasesView_(new QTabWidget(this))
    , executedTable_(new QTableView(this))
    , ongoingTable_(new QTableView(this))
    , passedTable_(new QTableView(this))
    , failedTable_(new QTableView(this))
    , abortedTable_(new QTableView(this))
    , ongoingProxy_(new QSortFilterProxyModel(this))
    , passedProxy_(new QSortFilterProxyModel(this))
    , failedProxy_(new QSortFilterProxyModel(this))
    , abortedProxy_(new QSortFilterProxyModel(this))
    , db_(Min::Database::getInstance())
{
    // Proxies for model
    ongoingProxy_->setSourceModel(executedCasesModel_);
    ongoingProxy_->setFilterKeyColumn(6);
    ongoingProxy_->setFilterWildcard(Min::DescriptionProvider::getTestCaseResultDescription(TP_NC));
    passedProxy_->setSourceModel(executedCasesModel_);
    passedProxy_->setFilterKeyColumn(6);
    passedProxy_->setFilterWildcard(Min::DescriptionProvider::getTestCaseResultDescription(TP_PASSED));
    failedProxy_->setSourceModel(executedCasesModel_);
    failedProxy_->setFilterKeyColumn(6);
    failedProxy_->setFilterWildcard(Min::DescriptionProvider::getTestCaseResultDescription(TP_FAILED));
    abortedProxy_->setSourceModel(executedCasesModel_);
    abortedProxy_->setFilterKeyColumn(6);
    abortedProxy_->setFilterWildcard(Min::DescriptionProvider::getTestCaseResultDescription(TP_CRASHED));

    // Prints
    printMsgView_->setModel(&printMsgModel_);

    // Executed cases view contents
    executedTable_->setModel(executedCasesModel_);
    executedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ongoingTable_->setModel(ongoingProxy_);
    ongoingTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    passedTable_->setModel(passedProxy_);
    passedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    failedTable_->setModel(failedProxy_);
    failedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    abortedTable_->setModel(abortedProxy_);
    abortedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
   
    // Hide columns from user which he/she does not want to see (Min internal ids)
    hideViewColumns();

    // Construct the executed cases view
    executedCasesView_->addTab(executedTable_,"All");
    executedCasesView_->addTab(ongoingTable_,"Ongoing");
    executedCasesView_->addTab(passedTable_,"Passed");
    executedCasesView_->addTab(failedTable_,"Failed");
    executedCasesView_->addTab(abortedTable_,"Aborted");
    executedCasesView_->setMinimumWidth(100);

    // Set-up Main pane
    splitter_->addWidget(executedCasesView_);
    splitter_->addWidget(printMsgView_);
    splitter_->setStretchFactor(0,4);

    // Signals and slots
    connect (&db_,SIGNAL(updated()),
             executedCasesModel_,SLOT(updateModelData()));
    connect (executedCasesModel_,SIGNAL(layoutChanged()),
	        this, SLOT(hideViewColumns()));
}
// -----------------------------------------------------------------------------
Min::ExecutedTab::~ExecutedTab()
{ ; }
// -----------------------------------------------------------------------------
void Min::ExecutedTab::resizeEvent(QResizeEvent *event)
{
    splitter_->resize(event->size());
    QWidget::resizeEvent(event);
}
// -----------------------------------------------------------------------------
void Min::ExecutedTab::hideViewColumns()
{
    executedTable_->setColumnHidden(1, true);
    executedTable_->setColumnHidden(2, true);
    executedTable_->setColumnHidden(4, true);
    executedTable_->setColumnHidden(5, true);
    executedTable_->setColumnHidden(6, true);
    executedTable_->setColumnHidden(7, true);
    executedTable_->setColumnHidden(8, true);
    executedTable_->setColumnHidden(9, true);
    ongoingTable_->setColumnHidden(1,true);
    ongoingTable_->setColumnHidden(2,true);
    ongoingTable_->setColumnHidden(5,true);
    ongoingTable_->setColumnHidden(6,true);
    ongoingTable_->setColumnHidden(7,true);
    ongoingTable_->setColumnHidden(8,true);
    ongoingTable_->setColumnHidden(9,true);
    passedTable_->setColumnHidden(1,true);
    passedTable_->setColumnHidden(2,true);
    passedTable_->setColumnHidden(3,true);
    passedTable_->setColumnHidden(8,true);
    passedTable_->setColumnHidden(9,true);
    failedTable_->setColumnHidden(1,true);
    failedTable_->setColumnHidden(2,true);
    failedTable_->setColumnHidden(3,true);
    failedTable_->setColumnHidden(8,true);
    failedTable_->setColumnHidden(9,true);
    abortedTable_->setColumnHidden(1,true);
    abortedTable_->setColumnHidden(2,true);
    abortedTable_->setColumnHidden(3,true);
    abortedTable_->setColumnHidden(8,true);
    abortedTable_->setColumnHidden(9,true);
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
