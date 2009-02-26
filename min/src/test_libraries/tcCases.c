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
 *  @file       tcCases.c
 *  @version    0.1
 *  @brief      This file contains implementation of tc
 *              test module test functions.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "tc.h"

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
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
LOCAL int       ExampleTest (MinItemParser * item);
LOCAL int       One (MinItemParser * item);
LOCAL int       Two (MinItemParser * item);
LOCAL int       Three (MinItemParser * item);
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL int ExampleTest (MinItemParser * item)
{
        char           *string = INITPTR;
        int             i = 0;

        while (mip_get_next_string (item, &string) == ENOERR) {

                tm_printf (i, "tc", "Param[%d]: %s", i,
                           string);
                i++;

                DELETE (string);
        }

        return ENOERR;
}

LOCAL int One (MinItemParser * item)
{


        return ENOERR;
}

LOCAL int Two (MinItemParser * item)
{


        return ENOERR;
}

LOCAL int Three (MinItemParser * item)
{


        return ENOERR;
}

LOCAL int FailingOne (MinItemParser * item)
{


        return -1;
}

LOCAL int CheckThreshold (MinItemParser * item)
{
        int threshold;
        long failcount;

        if (mip_get_next_int (item, &threshold) != ENOERR) {
                tm_printf (0, "", "Threshold parameter missing");
                return -1;
        }
        if (GetLocalValueInt ("FAIL_COUNT", &failcount) != 0) {
                tm_printf (0, "", "Failed to get FAIL_COUNT");
                return -1;
        } 
        if (failcount >= threshold) {
                tm_printf (0, "", "Fail count %d exceed threshold %d, setting "
                           "variable bailout", failcount, threshold);
                if (SetLocalValue ("bailout", "true") != 0) {
                        tm_printf (0, "", "Failed to set var bailout");
                        return -1;
                }
        } 
        
        return ENOERR;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int ts_get_test_cases (DLList ** list)
{
        /*
         * Copy this line for every implemented function.
         * * First string is the function name used in TestScripter file.
         * * Second is the actual implementation function. 
         */
        ENTRYTC (*list, "Example", ExampleTest);
        ENTRYTC (*list, "One", One);
        ENTRYTC (*list, "Two", Two);
        ENTRYTC (*list, "Three", Three);
        ENTRYTC (*list, "FailingOne", FailingOne);
        ENTRYTC (*list, "CheckThreshold", CheckThreshold);

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
