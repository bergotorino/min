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
 * @file:     min_availablecasestreemodel.cpp
 * @version:  1.00
 * @date:     19.03.2009
 * @author:   
 */

// Module include
#include "min_testruntreemodel.hpp"
#include "min_database.hpp"

// System includes

// Min includes

// -----------------------------------------------------------------------------
Min::TreeItem::TreeItem(const QList<QVariant> &data, Min::TreeItem *parent)
    : data_(data)
    , child_()
    , parent_(parent)
{ }
// -----------------------------------------------------------------------------
Min::TreeItem::~TreeItem()
{
    qDeleteAll(child_);
}
// -----------------------------------------------------------------------------
void Min::TreeItem::appendChild(Min::TreeItem *child)
{
    child_.append(child);
}
// -----------------------------------------------------------------------------
Min::TreeItem* Min::TreeItem::child(int row)
{
    return child_.value(row);
}
// -----------------------------------------------------------------------------
int Min::TreeItem::childCount() const
{
    return child_.count();
}
// -----------------------------------------------------------------------------
int Min::TreeItem::columnCount() const
{
    return data_.count();
}
// -----------------------------------------------------------------------------
QVariant Min::TreeItem::data(int column) const
{
    return data_.value(column);
}
// -----------------------------------------------------------------------------
int Min::TreeItem::row() const
{
    if (parent_) {
        return parent_->child_.indexOf(const_cast<TreeItem*>(this));
    }

    return 0;
}
// -----------------------------------------------------------------------------
void Min::TreeItem::childClean()
{
    child_.clear();
}
// -----------------------------------------------------------------------------
Min::TreeItem* Min::TreeItem::parent() const
{
    return parent_;
}
// -----------------------------------------------------------------------------
Min::TestRunTreeModel::TestRunTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Module Name" << "Test Case Name" << "Description";
    root_ = new Min::TreeItem(rootData);
    setupModelData();
}
// -----------------------------------------------------------------------------
Min::TestRunTreeModel::~TestRunTreeModel()
{
    delete root_;
}
// -----------------------------------------------------------------------------
QVariant Min::TestRunTreeModel::data(const QModelIndex &index,
                                            int role) const
{
    if (!index.isValid()) return QVariant();

    Min::TreeItem *tmp;

    switch (role) {
    case Qt::CheckStateRole:
        if (index.column() == 0 ) return false;
        return QVariant();
    case Qt::DisplayRole:
        tmp = static_cast<Min::TreeItem*>(index.internalPointer());
        return tmp->data(index.column());
        break;
    default:
        return QVariant();
    }
}
// -----------------------------------------------------------------------------
Qt::ItemFlags Min::TestRunTreeModel::flags(const QModelIndex &i) const
{
    if (!i.isValid()) return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}
// -----------------------------------------------------------------------------
QVariant Min::TestRunTreeModel::headerData(int section,
                                                Qt::Orientation orientation,
                                                int role) const
{
    if (orientation==Qt::Horizontal && role==Qt::DisplayRole ) {
        return root_->data(section);
    }
    return QVariant();
}
// -----------------------------------------------------------------------------
QModelIndex Min::TestRunTreeModel::index(int row, int column,
                                                const QModelIndex &parent) const
{
    if (!hasIndex(row,column,parent)) return QModelIndex();
    Min::TreeItem *tmp = NULL;
    if (!parent.isValid()) tmp = root_;
    else tmp = static_cast<Min::TreeItem*>(parent.internalPointer());
    Min::TreeItem *child = tmp->child(row);
    if (child) return createIndex(row,column,child);
    else return QModelIndex();
}
// -----------------------------------------------------------------------------
QModelIndex Min::TestRunTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) return QModelIndex();
    Min::TreeItem *childItem = 
        static_cast<Min::TreeItem*>(index.internalPointer());
    Min::TreeItem *parentItem = childItem->parent();
    if (parentItem==root_) return QModelIndex();
    return createIndex(parentItem->row(),0,parentItem);
}
// -----------------------------------------------------------------------------
int Min::TestRunTreeModel::rowCount(const QModelIndex &parent) const
{
    Min::TreeItem *parentItem;
    if (parent.column() > 0) return 0;
    if (!parent.isValid()) parentItem = root_;
    else parentItem = static_cast<Min::TreeItem*>(parent.internalPointer());
    return parentItem->childCount();
}
// -----------------------------------------------------------------------------
int Min::TestRunTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        Min::TreeItem *tmp;
        tmp = static_cast<Min::TreeItem*>(parent.internalPointer());
        return tmp->columnCount();
    } else return root_->columnCount();
}
// -----------------------------------------------------------------------------
#include <QFile>
#include <QStringList>
void Min::TestRunTreeModel::setupModelData()
{
/*    QStringList list;
    QList<QVariant> data;
    data << "DemoModule";

    for (int k = 0 ; k < 10 ; k++ ) {
        Min::TreeItem *tmp = new Min::TreeItem(data,root_);
        root_->appendChild(tmp);

        QList<QVariant> data2;
        data2 << "Test Case 1";
        data2 << "1";
        data2 << "Selection";
        for (int i = 0 ; i < 5 ; i++ ) {
            Min::TreeItem *tmp2 = new Min::TreeItem(data2,tmp);
            tmp->appendChild(tmp2);
        }
    }

    // These will be modules, we have to keep list of them cause they are
    // childs of root*/
//    QList<Min::TreeItem*> parents_;

    // FIXME: fetch data from SQL!!!
    //
//    QFile file("./default.txt");
//    file.open(QIODevice::ReadOnly);
//    QStringList lines = QString(file.readAll()).split(QString("\n"));
//    file.close();

    
	QList<Min::TreeItem*> parents;
	QList<int> indentations;
	QList<unsigned int> parentData;
	Min::Database &db_ = Min::Database::getInstance();
	root_->childClean();
	parents.clear();
	parents << root_;

	parentData=db_.getTestRunGroups();

	QListIterator<unsigned int> it(parentData);
	while(it.hasNext()){
		unsigned int current = it.next();
		//qDebug("current: %d ", current);
//		if (parents.last()->childCount() > 0) {
//	        	parents << parents.last()->child(parents.last()->childCount()-1);
//	     	}
	     	QVector<QStringList> rows=db_.getTestRunsInGroup(1, current);
		QVectorIterator< QStringList >  iit(rows);
      		while(iit.hasNext()){
			QStringList row = iit.next();
			QList<QVariant> row2;
			foreach(QString s, row){
				//qDebug("%s", s.toStdString().c_str());
				row2 << s;
			}
             		parents.last()->appendChild(new TreeItem(row2, parents.last()));
		}
//		parents.pop_back();


      }

/*     indentations << 0;

     int number = 0;

     while (number < lines.count()) {
         int position = 0;
         while (position < lines[number].length()) {
             if (lines[number].mid(position, 1) != " ")
                 break;
             position++;
         }

         QString lineData = lines[number].mid(position).trimmed();

         if (!lineData.isEmpty()) {
             // Read the column data from the rest of the line.
             QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
             QList<QVariant> columnData;
             for (int column = 0; column < columnStrings.count(); ++column)
                 columnData << columnStrings[column];

             if (position > indentations.last()) {
                 // The last child of the current parent is now the new parent
                 // unless the current parent has no children.

                 if (parents.last()->childCount() > 0) {
                     parents << parents.last()->child(parents.last()->childCount()-1);
                     indentations << position;
                 }
             } else {
                 while (position < indentations.last() && parents.count() > 0) {
                     parents.pop_back();
                     indentations.pop_back();
                 }
             }

             // Append a new item to the current parent's list of children.
             parents.last()->appendChild(new TreeItem(columnData, parents.last()));
         }

         number++;
     }*/
}
// -----------------------------------------------------------------------------
void Min::TestRunTreeModel::updateModelData()
{
    emit layoutAboutToBeChanged();
    setupModelData();
    emit layoutChanged();
}


// file created by generator.sh v1.08
