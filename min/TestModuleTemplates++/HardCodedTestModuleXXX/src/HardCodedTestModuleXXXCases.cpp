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
 *  @file       HardCodedTestModuleXXXCases.c
 *  @version    0.1
 *  @brief      This file contains implementation of the test cases for
 *              HardCodedTestModuleXXX.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#ifdef __cplusplus
extern "C" {
#endif
#include <test_module_api.h>
#include <min_logger.h>
#ifdef __cplusplus
}
#endif
/*If You wish to use test interference, uncomment line below*/
/*#include <min_test_interference.h>*/
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

        ENTRY  (*cases, "Test Case 1", test_1);
        ENTRYD (*cases, "Test Case 2", test_2, "<description for Test Case 2>");

        return retval;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
