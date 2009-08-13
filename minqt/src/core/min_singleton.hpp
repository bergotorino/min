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
 * @file:     min_singleton.hpp
 * @version:  1.00
 * @date:     20.04.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_SINGLETON_HPP
#define INCLUDED_MIN_SINGLETON_HPP

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class Singleton
     * @brief Template singleton class
     */
    template<class T>
    class Singleton
    {
    public:

        /** C++ Default Constructor */
        Singleton()
        { ; }

        /** C++ Destructor */
        virtual ~Singleton()
        { ; }

        /** Creates instance of T class
         *  return reference to static variable of type T.
         */
        static T& getInstance()
        {
            static T t;
            return t;
        }

    private:
        /** Copy Constructor. */
        Singleton( const Min::Singleton<T>& c );
        /** Assignment operator. */
        Min::Singleton<T>& operator =( const Min::Singleton<T>& c );
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_SINGLETON_HPP (file created by generator.sh v1.08)

