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
/**
 *  @file       TemplateQtScriptXXXCases.hpp
 *  @version    0.1
 *  @brief      This file contains header file of hte TemplateQtScriptXXX
 *              test module.
 */
#ifndef TEMPLATEQTSCRIPTXXX_H
#define TEMPLATEQTSCRIPTXXX_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
/* ------------------------------------------------------------------------- */
/**
 * @class TestWidget
 * @brief Simple example test class used from ExampleTest().
 */
class TestWidget: public QWidget
{
    Q_OBJECT
	
    public:
	TestWidget(QMainWindow *parent);
			 
    public slots:
	void pass_() {QCoreApplication::exit(0);};
	void fail_() {QCoreApplication::exit(1);};
	void crash_() {int *ip = NULL; *ip = 1;};

    protected:
    private:
	QHBoxLayout *layout_;
	QPushButton *passbutton_;	    
	QPushButton *failbutton_;	    
	QPushButton *crashbutton_;	    

};

#endif /* TEMPLATEQTSCRIPTXXX_H */
