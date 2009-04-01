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
 * @file:     min_remotecontroll.hpp
 * @version:  1.00
 * @date:     27.03.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_REMOTECONTROLL_HPP
#define INCLUDED_MIN_REMOTECONTROLL_HPP

// System includes
#include <QtDBus/QtDBus>
#include <QObject>

// Min includes
#include "min_object.hpp.h"

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class RemoteControll
     * @brief
     */
    class RemoteControll: public QObject
    {
    public:
        /** Default C++ Constructor. */
        RemoteControll();

        /** Destructor. */
        ~RemoteControll();

        /** Gives status of object.
         *  @return true if initialization was success, false otherwise
         */
        bool isValid() const;

    private:
        /**@{Declared but nit defined.*/
        /** Copy Constructor. */
        RemoteControll( const Min::RemoteControll& c );
        /** Assignment operator. */
        Min::RemoteControll& operator =( const Min::RemoteControll& c );
        /**@}*/

        /** Connection to the DBus */
        QDBusConnection bus_;

        /** Generated from DBus interface .xml MinObject */
        MinObject obj_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_REMOTECONTROLL_HPP (file created by generator.sh v1.08)

