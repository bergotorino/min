/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
 * Contact e-mail: min-support@lists.sourceforge.net
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
 *  @file       HardCodedCases.c
 *  @version    0.1
 *  @brief      This file contains implementation of the test cases for
 *              HardCoded.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <test_module_api.h>
#include <min_logger.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
char *module_date = __DATE__;
char *module_time = __TIME__;
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
/* ------------------------------------------------------------------------- */
int             test_1 (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
int             test_2 (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
int             test_3 (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
int test_1 (TestCaseResult * tcr)
{
        RESULT (tcr, TP_FAILED, "FAILED");
}
/* ------------------------------------------------------------------------- */
int test_2 (TestCaseResult * tcr)
{
        RESULT (tcr, TP_PASSED, "PASSED");
}
/* ------------------------------------------------------------------------- */
int test_3 (TestCaseResult * tcr)
{
        RESULT (tcr, TP_FAILED, "FAILED");
}
/* ------------------------------------------------------------------------- */
int test_4 (TestCaseResult * tcr)
{
        RESULT (tcr, TP_PASSED, "PASSED");
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/**Function that will be called once during loading of testmodule*/
/*uncomment function below and implement it if you need this 
functionality*/

/*
void tm_initialize()
{
        return;
}
*/
/* ------------------------------------------------------------------------- */
/**Function that will be called once during shutdown/closing of testmodule*/
/*uncomment function below and implement it if you need this 
functionality*/

/*
void tm_finalize()
{
        return;
}
*/
/* ------------------------------------------------------------------------- */
int tm_get_test_cases (const char *cfg_file, DLList ** cases)
{
        int             retval = 0;

        ENTRY (*cases, "One", test_1);
        ENTRY (*cases, "Two", test_2);
        ENTRY (*cases, "Three", test_3);
        ENTRY (*cases, "Four", test_4);

        return retval;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
