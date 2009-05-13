/**
 * @file:     min_dbusconnectiondialog.cpp
 * @version:  1.00
 * @date:     13.05.2009
 * @author:   
 */

// Module include
#include "min_dbusconnectiondialog.hpp"

// System includes
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>

// -----------------------------------------------------------------------------
Min::DBusConnectionDialog::DBusConnectionDialog(QWidget *parent)
    : QDialog(parent)
    , layout_(new QGridLayout(this))
    , comboBox_(new QComboBox(this))
    , selectText_(new QLabel("Choose connection:",this))
    , okButton_(new QPushButton("Connect",this))
    , cancelButton_(new QPushButton("Cancel",this))
{
    // Set-up
    comboBox_->setEditable(true);
    
    // Layout
    layout_->addWidget(selectText_,0,0,1,2);
    layout_->addWidget(comboBox_,1,0,1,2);
    layout_->addWidget(okButton_,2,0);
    layout_->addWidget(cancelButton_,2,1);

}
// -----------------------------------------------------------------------------
Min::DBusConnectionDialog::~DBusConnectionDialog()
{ ; }
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
