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
 * @file:     min_remotecontroll.cpp
 * @version:  1.00
 * @date:     27.03.2009
 * @author:   
 */

#include "min_remotecontroll.hpp"

// -----------------------------------------------------------------------------
Min::RemoteControll::RemoteControll()
    : bus_(QDBusConnection::connectToBus(QDBusConnection::SessionBus,
                                        "org.maemo.MIN"))
    , obj_("org.maemo.MIN","/Min",bus_)
{ }
// -----------------------------------------------------------------------------
bool Min::RemoteControll::isValid() const
{
    if (bus_.isConnected() && obj_.isValid()) return true;
    else return false;
}
// -----------------------------------------------------------------------------
Min::RemoteControll::~RemoteControll()
{ ; }
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
