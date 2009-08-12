/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       MINUnitXXX.c
 *  @version    0.1
 *  @brief      This file contains implementation of the MINUnitXXX
 *              test module.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#ifdef __cplusplus
extern "C" {
#endif
#include <test_module_api.h>
#include <min_logger.h>
#include <min_parser.h>
#include <min_test_event_if.h>
#include "MINUnitXXX.h"
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
char *module_date = __DATE__;
char *module_time = __TIME__;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
TTestModuleType module_type     = ESUnit;
unsigned int    module_version  = 200830;
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
#define GET_CASES    0
#define RUN_SETUP    1
#define RUN_CASE     2
#define RUN_TEARDOWN 3

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
LOCAL int       min_unit_wrapper (int __action__, const char *__cfg_file__,
                                   DLList ** __cases__, unsigned int __id__,
                                   TestCaseResult * __result__);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Handles tm_get_test_cases() and tm_run_case() for MINUnit module.
 *  @param __action__ tells wether we are running or getting cases
 *  @param __cfg_file__ not used in MINUnit
 *  @param __cases__ [in/out] list of test cases
 *  @param __id__ test case id
 *  @param __result__ test case result
 *  @return 0 always
 */
LOCAL int min_unit_wrapper (int __action__, const char *__cfg_file__,
                             DLList ** __cases__, unsigned int __id__,
                             TestCaseResult * __result__)
{
        /*
         * Needed variables, please do not edit this section 
         */
        int             __test_case_index__;
#define TEST_VAR_DECLARATIONS
#include CASES_FILE
#undef  TEST_VAR_DECLARATIONS
        if (__action__ == RUN_CASE)
                __action__ = RUN_SETUP;
      min_unit_again:
        __test_case_index__ = 0;
#define TEST_CASES
#include CASES_FILE
#undef  TEST_CASES
        if (__action__ == RUN_SETUP) {
                __action__ = RUN_CASE;
                goto min_unit_again;
        } else if (__action__ == RUN_CASE) {
                __action__ = RUN_TEARDOWN;
                goto min_unit_again;
        }

        if (__action__ != GET_CASES)
                RESULT (__result__, TP_PASSED, "PASSED");
        
        return ENOERR;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Implementation of tm_get_test_cases for MINUnit
 */
int tm_get_test_cases (const char *cfg_file, DLList ** cases)
{

        min_unit_wrapper (GET_CASES, cfg_file, cases, 0, NULL);

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Implementation of tm_run_test_case for MINUnit
 */
int tm_run_test_case (unsigned int id, const char *cfg_file,
                      TestCaseResult * result)
{
        int             retval = 0;

        min_unit_wrapper (RUN_CASE, cfg_file, NULL, id, result);

        return retval;
}
/* ------------------------------------------------------------------------- */
/** return  test module type */
unsigned int get_module_type()
{ return module_type; }
/* ------------------------------------------------------------------------- */
/** return test module template version */
unsigned int get_module_version()
{ return module_version; }
/* ------------------------------------------------------------------------- */
/** return build date */
char* get_module_date()
{ return module_date; }
/* ------------------------------------------------------------------------- */
/** return build time */
char* get_module_time()
{ return module_time; }
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
