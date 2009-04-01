/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 * @file:     min_mainwindow.hpp
 * @version:  1.00
 * @date:     18.03.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_MAINWINDOW_HPP
#define INCLUDED_MIN_MAINWINDOW_HPP

// System includes
#include <QMainWindow>


// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief MIN Test Framework napespace
 */
namespace Min
{
    // Forward decaration
    class MainWidget;
    // -------------------------------------------------------------------------
    /**
     * @class MainWindow
     * @brief MIN Qt client main window class
     */
    class MainWindow: public QMainWindow
    {
    Q_OBJECT
    public:
        /** Default C++ Constructor. */
        MainWindow();

        /** Destructor. */
        ~MainWindow();
    private slots:
        /** Displays "About" information. */
        void displayAboutDialog ();
    private:
        /**@{ Declared but not defined by design. */
        /** Copy Constructor. */
        MainWindow( const Min::MainWindow& c );
        /** Assignment operator. */
        Min::MainWindow& operator =( const Min::MainWindow& c );
        /**@}*/

        /** Initializes the menu bar. */
        void setupMenuBar();
    private:
        /** MIN Qt client main widget. */
        Min::MainWidget *mainWidget_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_MAINWINDOW_HPP (file created by generator.sh v1.08)

