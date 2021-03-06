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
 *  @file       TestModuleXXX.c
 *  @version    0.1
 *  @brief      This file contains implementation of the TestModuleXXX.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <test_module_api.h>
#include <min_logger.h>
#include <min_parser.h>
#include <min_test_event_if.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern char *module_date;
extern char *module_time;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
TTestModuleType module_type     = ENormal; /** Test module type */
unsigned int    module_version  = 200830;  /** Test module template version */
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
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** tm_run_test_case() for this test module. Needs to be implemented by user.
  * @param id test case id
  * @param cfg_file test case file
  * @param result [out] test case result
  * @return test result code
  */
int tm_run_test_case (unsigned int id, const char *cfg_file,
                      TestCaseResult * result)
{
        /*
         * Set test case result and description. 
         */
        RESULT (result, TP_PASSED, "Test Passed");
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
