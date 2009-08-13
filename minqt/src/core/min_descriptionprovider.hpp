/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Maciej Jablonski
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


#ifndef INCLUDED_MIN_DESCRIPTIONPROVIDER_HPP
#define INCLUDED_MIN_DESCRIPTIONPROVIDER_HPP

// System includes
#include <QVariant>

// Min includes

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // ---------------------------------------------------------------------
    /** 
     * @class DescriptionProvider
     * @brief Provides test case result and status strings
     */
    class DescriptionProvider: public QObject
    {
    Q_OBJECT
    public:
	/** constructor. */
        DescriptionProvider();
	/** default destructor */
        ~DescriptionProvider();
	/** status description */
	static QString getTestCaseStatusDescription(int tcstatus);
	/** result description */
	static QString getTestCaseResultDescription(int tcresult);

    signals:

    private:
    };
    // -------------------------------------------------------------------------
};
// namespace Min
// -----------------------------------------------------------------------------
#endif  // INCLUDED_MIN_DESCRIPTIONPROVIDER

