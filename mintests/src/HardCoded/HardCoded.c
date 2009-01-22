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
 *  @file       HardCoded.c
 *  @version    0.1
 *  @brief      This file contains implementation of the HardCoded
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
TTestModuleType module_type     = EHardcoded;
unsigned int    module_version  = 200830;
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
int tm_run_test_case (unsigned int id, const char *cfg_file,
                      TestCaseResult * result)
{
        /*
         * Needed variables, please do not edit this section 
         */
        int             retval;
        DLList         *list;
        DLListIterator  it;
        TestCaseInfo   *tci;

        retval = 0;
        list = dl_list_create ();

        /*
         * Check if list was initialized properly 
         */
        if (list != INITPTR) {

                /*
                 * Get test cases connected with this test file 
                 */
                tm_get_test_cases (cfg_file, &list);

                /*
                 * Find test case of a given id 
                 */
                it = dl_list_find (dl_list_head (list)
                                   , dl_list_tail (list)
                                   , compare_id, &id);

                /*
                 * Check if there is a test case of a given id 
                 */
                if (it != DLListNULLIterator) {

                        /*
                         * Get the test case detailed information 
                         */
                        tci = (TestCaseInfo *) dl_list_data (it);

                        if (tci != INITPTR) {

                                /*
                                 * If test function is specified run a test 
                                 * * case.
                                 * * For hardcoded test library we assume that 
                                 * * test functions are specified.
                                 */
                                if (tci->test_ != (ptr2test) INITPTR) {
                                        tci->test_ (result);
                                }
                        }
                }

        } else
                retval = -1;

        /*
         * Cleanup 
         */
        it = dl_list_head (list);
        while (it != DLListNULLIterator) {
                free (dl_list_data (it));
                dl_list_remove_it (it);
                it = dl_list_head (list);
        }
        dl_list_free (&list);

        return retval;
}
/* ------------------------------------------------------------------------- */
unsigned int get_module_type()
{ return module_type; }
/* ------------------------------------------------------------------------- */
unsigned int get_module_version()
{ return module_version; }
/* ------------------------------------------------------------------------- */
char* get_module_date()
{ return module_date; }
/* ------------------------------------------------------------------------- */
char* get_module_time()
{ return module_time; }
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
