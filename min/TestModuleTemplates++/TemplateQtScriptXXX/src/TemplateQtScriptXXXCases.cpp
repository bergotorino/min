/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       TemplateQtScriptXXXCases.c
 *  @version    0.1
 *  @brief      This file contains implementation of TemplateQtScriptXXX
 *              test module test functions.
 */
/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "TemplateQtScriptXXX.h"
#include "TemplateQtScriptXXXCases.hpp"

#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
const char *module_date = __DATE__;
const char *module_time = __TIME__;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL int       ExampleTest (MinItemParser * item);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
/* None */
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
TestWidget::TestWidget(QMainWindow *parent)
	: layout_(new QHBoxLayout (this)),
	  passbutton_(new QPushButton ("PASS", this)),
	  failbutton_(new QPushButton ("FAIL", this)),
	  crashbutton_ (new QPushButton ("CRASH", this))
	  
{
	layout_->addWidget (passbutton_);
	layout_->addWidget (failbutton_);
	layout_->addWidget (crashbutton_);

	connect(passbutton_, SIGNAL(clicked()), this, SLOT(pass_()));
	connect(failbutton_, SIGNAL(clicked()), this, SLOT(fail_()));
	connect(crashbutton_, SIGNAL(clicked()), this, SLOT(crash_()));
	
}
/* ------------------------------------------------------------------------- */
LOCAL int ExampleApp (MinItemParser * item)
{
	int ret, argc = 1;
	char *string;
	char *argv;
	QString s = " ";

        while (mip_get_next_string (item, &string) == ENOERR) {
		s.append(string);
		s.append(" ");
		string = (char *)INITPTR;
	} 
	argv = NEW2(char, s.size());
	sprintf (argv, (char *)s.toStdString().c_str(), s.size());

	QApplication a(argc, &argv, QT_VERSION);
	QMainWindow  window;
	QWidget     *w          = new TestWidget (&window);
	
	window.setCentralWidget(w);


	window.show();
 	ret = a.exec();
	MIN_DEBUG ("RET: %d", ret);
	free ((void *)argv);

        return ret;
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int ts_get_test_cases (DLList ** list)
{
	/*
         * Copy this line for every implemented function.
         * * First string is the function name used in TestScripter file.
         * * Second is the actual implementation function. 
         */
        ENTRYTC (*list, "ExampleApp", ExampleApp);
	/*
         * ADD NEW ENTRY HERE 
         */
}
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */

