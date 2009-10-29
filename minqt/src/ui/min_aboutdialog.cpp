/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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

// Module include
#include "min_aboutdialog.hpp"

// System includes
#include <QLabel>
#include <QResizeEvent>
#include <QDialogButtonBox>
#include <QFont>
#include <QWidget>
#include <QDialog>
#include <QSizePolicy>

// Min incudes
/* MIN Test Framework, (c) Nokia 2008, All rights reserved,
 * licensed under the Gnu General Public License version 2,
 * Contact: Pekka Nuotio, DG.MIN-Support@nokia.com
 *
 */


// -----------------------------------------------------------------------------
Min::AboutDialog::AboutDialog(QWidget * parent) : QDialog(parent)
{
	if(this->objectName().isEmpty()){
		this->setObjectName(QString::fromUtf8("AboutDialog"));
	}
	this->setFixedSize(400,200);
	this->setWindowTitle("About MIN Test Framework");

	this->buttonBox = new QDialogButtonBox(this);
	this->buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
	this->buttonBox->setGeometry(QRect(200, 150, 80, 32));
	this->buttonBox->setStandardButtons(QDialogButtonBox::Ok);

	this->label_1 = new QLabel(this);
	QFont font;
	font.setPointSize(12);
	font.setBold(true);
	font.setWeight(75);
	this->label_1->setFont(font);
	this->label_1->setGeometry(QRect(20, 20, 360, 30));
	this->label_1->setAlignment(Qt::AlignTop|Qt::AlignLeft);
	this->label_1->setText(QString::fromUtf8("About MIN"));

	this->label_2 = new QLabel(this);
	this->label_2->setObjectName(QString::fromUtf8("label_2"));
	this->label_2->setGeometry(QRect(20,50,360,100));
	this->label_2->setWordWrap(true);
	this->label_2->setAlignment(Qt::AlignTop|Qt::AlignLeft);
	this->label_2->setText(
		"MIN Test Framework, (c) Nokia 2008, All rights reserved,\n"
		"licensed under the Gnu General Public License version 2,\n"
		"Contact: Pekka Nuotio, DG.MIN-Support@nokia.com\n"
		"or visit: http://min.sourceforge.net/");

	QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}
// -----------------------------------------------------------------------------
Min::AboutDialog::~AboutDialog()
{ ; }
// -----------------------------------------------------------------------------
#if 0
void Min::AboutDialog::changeEvent(QEvent *event){
	/** EMPTY BODY 
	 * Function can be implemented if needed, currently "About" dialog is 
	 * unresizable, so no bigger changes to make
	 **/

}
#endif
// -----------------------------------------------------------------------------



