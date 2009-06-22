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
#include <QHeaderView>

// Min includes
#include "min_executedmodel.hpp"
#include "min_database.hpp"
#include "min_common.h"
#include "tmc_common.h"
#include "min_descriptionprovider.hpp"
#include "min_executedalldelegate.hpp"

// -----------------------------------------------------------------------------
Min::ExecutedTab::ExecutedTab(QWidget *parent)
    : QWidget(parent)
    , splitter_(new QSplitter(Qt::Horizontal,this))
    , printMsgView_(new QListView(this))
    , printMsgModel_(new QStringListModel(this))
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
    ongoingProxy_->setFilterWildcard(Min::DescriptionProvider::getTestCaseResultDescription(TP_RUNNING));
    passedProxy_->setSourceModel(executedCasesModel_);
    passedProxy_->setFilterKeyColumn(6);
    passedProxy_->setFilterWildcard(Min::DescriptionProvider::getTestCaseResultDescription(TP_PASSED));
    failedProxy_->setSourceModel(executedCasesModel_);
    failedProxy_->setFilterKeyColumn(6);
    failedProxy_->setFilterWildcard(Min::DescriptionProvider::getTestCaseResultDescription(TP_FAILED));
    abortedProxy_->setSourceModel(executedCasesModel_);
    abortedProxy_->setFilterKeyColumn(6);
    //  abortedProxy_->setFilterWildcard(Min::DescriptionProvider::getTestCaseResultDescription(TP_CRASHED));
    abortedProxy_->setFilterRegExp("[nrNR]");

    // Prints
    printMsgModel_->setStringList(db_.getPrintoutView(1));
    printMsgView_->setModel(printMsgModel_);

    // Executed cases view contents
    executedTable_->setModel(executedCasesModel_);
    executedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    executedTable_->resizeColumnsToContents();
    executedTable_->horizontalHeader()->setStretchLastSection(true);
    executedTable_->setItemDelegate(new Min::ExecutedAllDelegate());

    // Ongoing view
    ongoingTable_->setModel(ongoingProxy_);
    ongoingTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ongoingTable_->resizeColumnsToContents();
    ongoingTable_->horizontalHeader()->setStretchLastSection(true);

    // Passed view
    passedTable_->setModel(passedProxy_);
    passedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    passedTable_->resizeColumnsToContents();
    passedTable_->horizontalHeader()->setStretchLastSection(true);

    // Failed view
    failedTable_->setModel(failedProxy_);
    failedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    failedTable_->resizeColumnsToContents();
    failedTable_->horizontalHeader()->setStretchLastSection(true);

    // Aborted view
    abortedTable_->setModel(abortedProxy_);
    abortedTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    abortedTable_->resizeColumnsToContents();
    abortedTable_->horizontalHeader()->setStretchLastSection(true);
   
    // Hide columns from user which he/she does not want to see (Min internal ids)
    hideViewColumns();

    // Construct the executed cases view
    executedCasesView_->addTab(executedTable_,"All cases");
    executedCasesView_->addTab(ongoingTable_,"Ongoing cases");
    executedCasesView_->addTab(passedTable_,"Passed cases");
    executedCasesView_->addTab(failedTable_,"Failed cases");
    executedCasesView_->addTab(abortedTable_,"Aborted/Crashed cases");
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
    connect (executedTable_, SIGNAL(clicked(QModelIndex)),
			    this, SLOT(handleClick(QModelIndex)));
    connect (ongoingTable_, SIGNAL(clicked(QModelIndex)),
			    this, SLOT(handleClick(QModelIndex)));
    connect (passedTable_, SIGNAL(clicked(QModelIndex)),
			    this, SLOT(handleClick(QModelIndex)));
    connect (failedTable_, SIGNAL(clicked(QModelIndex)),
			    this, SLOT(handleClick(QModelIndex)));
    connect (abortedTable_, SIGNAL(clicked(QModelIndex)),
			    this, SLOT(handleClick(QModelIndex)));
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
QItemSelectionModel* Min::ExecutedTab::getSelectionFromOngoingCasesView()
{
    return ongoingTable_->selectionModel();
}
// -----------------------------------------------------------------------------
void Min::ExecutedTab::handleClick(const QModelIndex& index)
{
	const QTableView *tab=static_cast<QTableView*>(executedCasesView_->currentWidget());
	const QItemSelectionModel *selection=tab->selectionModel();
	if(!selection->hasSelection()) return;

	QModelIndexList runs = selection->selectedRows(8);
	if(runs.size()==0) return;

	QStringList printouts;

	for(int i=0; i<runs.count(); i++) {
		printouts << db_.getPrintoutView(runs[i].data().toUInt());
	}

	printMsgModel_->setStringList(printouts);
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
