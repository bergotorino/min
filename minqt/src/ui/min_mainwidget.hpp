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
 * @file:     min_mainwidget.hpp
 * @version:  1.00
 * @date:     18.03.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_MAINWIDGET_HPP
#define INCLUDED_MIN_MAINWIDGET_HPP

// System includes
#include <QWidget>

// Min includes

// Forward declarations
class QListView;
class QToolBox;
class QResizeEvent;
class QSplitter;
class QStringListModel;
class QTableView;
class QTabWidget;
class QItemSelectionModel;

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // Forward declarations
    class DevicePanel;
    class CasesPanel;
    // -------------------------------------------------------------------------
    /**
     * @class MainWidget
     * @brief MainWidget of MIN Qt client
     *
     * Since we have to add main widget class to the QMainWindow it is a need to
     * have this class.
     */
    class MainWidget: public QWidget
    {
    public:
        /** C++ Constructor. */
        MainWidget(QWidget *parent);

        /** Destructor. */
        ~MainWidget();

    public:

        /** Gets selection model for available cases view (those to be executed)
         *  @return pointer to selection model
         */
        QItemSelectionModel* getSelectedAvailableTestCases() const;

        /** Gets selection model for ongoing cases view 
         *  @return pointer to selection model
         */
        QItemSelectionModel* getSelectedOngoingTestCases() const;

    protected:
        /** Handles resize event that comes from the system. */
        void resizeEvent (QResizeEvent *event);
    private:
        /**@{Declared but not defined by design. */
        /** Copy Constructor. */
        MainWidget( const Min::MainWidget& c );
        /** Assignment operator. */
        Min::MainWidget& operator =( const Min::MainWidget& c );
        /**@}*/
    private:
        /** Widget splitter. */
        QSplitter *splitter_;

        /** Panel with list of available devices */
        Min::DevicePanel *leftPanel_;

        /** Panel that shows test cases from selected device */
        Min::CasesPanel *mainPanel_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_MAINWIDGET_HPP (file created by generator.sh v1.08)

