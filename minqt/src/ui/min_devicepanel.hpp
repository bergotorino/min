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
 * @file:     min_devicepanel.hpp
 * @version:  1.00
 * @date:     19.03.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_DEVICEPANEL_HPP
#define INCLUDED_MIN_DEVICEPANEL_HPP

// System includes
#include <QWidget>

// Forward declarations
class QListView;
class QStringListModel;
class QResizeEvent;

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class DevicePanel
     * @brief Displays list of available devices.
     *
     * Always shows at least localhost if DBus connection to the service
     * has been succesfull.
     */
    class DevicePanel: public QWidget
    {
    public:
        /** Default C++ Constructor. */
        DevicePanel(QWidget *parent);

        /** Destructor. */
        ~DevicePanel();
    protected:
        /** Handles resize event that comes from the system. */
        void resizeEvent (QResizeEvent *event);
    private:
        /**@{Declared but not defined by design. */
        /** Copy Constructor. */
        DevicePanel( const Min::DevicePanel& c );
        /** Assignment operator. */
        Min::DevicePanel& operator =( const Min::DevicePanel& c );
        /**@}*/
    private:

        /** List of available devices. */
        QListView *devicesList_;

        /** Model for the left panel.
         *
         *  FIXME: for stub purposes it is StringListModel, but it should be
         *  changed to Min::DevicesListModel.
         */
        QStringListModel *devicesModel_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_DEVICEPANEL_HPP (file created by generator.sh v1.08)

