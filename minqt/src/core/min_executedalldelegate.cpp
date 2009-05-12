/**
 * @file:     min_executedalldelegate.cpp
 * @version:  1.00
 * @date:     11.05.2009
 * @author:   Konrad Marek Zapalowicz
 */

// Module include
#include "min_executedalldelegate.hpp"

// System includes
#include <QBrush>
#include <QPainter>

// Min includes
#include "min_common.h"

// -----------------------------------------------------------------------------
Min::ExecutedAllDelegate::ExecutedAllDelegate()
    : QItemDelegate()
{ ; }
// -----------------------------------------------------------------------------
Min::ExecutedAllDelegate::~ExecutedAllDelegate()
{ ; }
// -----------------------------------------------------------------------------
void Min::ExecutedAllDelegate::paint(QPainter *painter,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    // Qt::UserRole is the first one we can use for application
    // specific data
    // This is mesy way to get column 6 data which describes the result. For
    // UserRole it returnts test case result enum value
    int tmp = index.model()->data(index.sibling(index.row(),6),Qt::UserRole).toInt();
    QString f = index.model()->data(index.sibling(index.row(),6),Qt::DisplayRole).toString();

//    qDebug("LALA: %s",f.toStdString().c_str());

/*
        TP_CRASHED = -2,        
        TP_TIMEOUTED = -1,      
        TP_PASSED = 0,          
        TP_FAILED = 1,         
        TP_NC = 2,            
        TP_LEAVE = 3
*/

    if (f.contains("Passed")) {
        if (option.state &QStyle::State_Selected) {
            painter->fillRect(option.rect,QBrush(QColor("green")));
        } else {
            painter->fillRect(option.rect,QBrush(QColor("lime")));
        }
        painter->drawText(option.rect,Qt::AlignVCenter,QString(" ")+index.data().toString());
    } else if (f.contains("Failed")||f.contains("Crashed")) {
        if (option.state &QStyle::State_Selected) {
            painter->fillRect(option.rect,QBrush(QColor("darksalmon")));
        } else {
            painter->fillRect(option.rect,QBrush(QColor("salmon")));
        }
        painter->drawText(option.rect,Qt::AlignVCenter,QString(" ")+index.data().toString());
    } else {
        QItemDelegate::paint(painter,option,index);
    }

//    QItemDelegate::paint(painter,option,index);
}
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
