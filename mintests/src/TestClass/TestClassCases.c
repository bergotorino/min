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
 *  @file       TestClassCases.c
 *  @version    0.1
 *  @brief      This file contains implementation of TestClass
 *              test module test functions.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "TestClass.h"

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
char *module_date = __DATE__;
char *module_time = __TIME__;
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
LOCAL int       Test1 (MinItemParser * item);
/* ------------------------------------------------------------------------- */
LOCAL int       Test2 (MinItemParser * item);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
/* None */
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL int Test1 (MinItemParser * item)
{
        return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int Test2 (MinItemParser * item)
{
        return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int Test3 (MinItemParser * item)
{
        return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int Test4 (MinItemParser * item)
{
        return -1;
}
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int ts_get_test_cases (DLList ** list)
{
        /*
         * Copy this line for every implemented function.
         * * First string is the function name used in TestScripter file.
         * * Second is the actual implementation function. 
         */
        ENTRYTC (*list, "One", Test1);
        ENTRYTC (*list, "Two", Test2);
        ENTRYTC (*list, "Three", Test3);
        ENTRYTC (*list, "Four", Test4);
        /*
         * ADD NEW ENTRY HERE 
         */
	return 0;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
