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





#include "min_descriptionprovider.hpp"
#include "tmc_common.h"
#include "min_common.h"

// ----------------------------------------------------------------------------
Min::DescriptionProvider::DescriptionProvider()
{
	/*EMPTY BODY */
};

// ----------------------------------------------------------------------------
Min::DescriptionProvider::~DescriptionProvider()
{
	/* EMPTY BODY */
};
// ----------------------------------------------------------------------------
QString Min::DescriptionProvider::getTestCaseStatusDescription(int tcstatus)
{
	switch(tcstatus){
		case TP_NONE:
			return QString("None");
			break;
		case TP_RUNNING:
			return QString("Test Case is Running");
			break;
		case TP_PAUSED:
			return QString("Test Case is currently Paused");
			break;
		case TP_ENDED:
			return QString("Test Case Ended normally");
			break;
		case TP_TIMEOUT:
			return QString("Timeout");
			break;
		case TP_ABORTED:
			return QString("Test Case has been Aborted");
			break;
		case TP_CANCELED:
			return QString("Canceled");
			break;
		default:
			return QString("Unknown");
	}
};
// ----------------------------------------------------------------------------
QString Min::DescriptionProvider::getTestCaseResultDescription(int tcresult)
{
	switch(tcresult){
		case TP_CRASHED:
			return QString("Test Case has Crashed");
			break;
		case TP_TIMEOUTED:
			return QString("Test Case has been Timeouted");
			break;
		case TP_PASSED:
			return QString("Test Case is Passed");
			break;
		case TP_FAILED:
			return QString("Test Case is Failed");
			break;
		case TP_NC:
			return QString("Test Case is Not completed");
			break;
		case TP_LEAVE:
			return QString("Leaved");
			break;
		default:
			return QString("Unknown");
	}

};
// ----------------------------------------------------------------------------
