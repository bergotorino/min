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
 *  @file       testlibrary2.c
 *  @version    0.1
 *  @brief      This file contains implementation of Test Library 2 of MIN
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "../tmc/test_module_api/test_module_api.h"

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
/* ------------------------------------------------------------------------- */
int             test_1 (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
int             test_2 (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
int             test_3 (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
int             test_4 (TestCaseResult * tcr);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
int test_1 (TestCaseResult * tcr)
{
        TL (0);
}

/* ------------------------------------------------------------------------- */
int test_2 (TestCaseResult * tcr)
{
        int             tmp = 6;
        while (tmp--) {
                sleep (1);
        }
        RESULT (tcr, TP_FAILED, "FAILED");
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int tm_get_test_cases (const char *cfg_file, DLList ** cases)
{
        int             retval = 0;
        ENTRY (*cases, "Test Case 1", test_1);
        ENTRY (*cases, "Test Case 2", test_2);
        return retval;
}

/* ------------------------------------------------------------------------- */
int tm_run_test_case (unsigned int id, const char *cfg_file,
                      TestCaseResult * result)
{
        int             retval = 0;
        DLList         *list = dl_list_create ();
        DLListIterator  it = DLListNULLIterator;
        TestCaseInfo   *tci = INITPTR;

        if (list != INITPTR) {
                tm_get_test_cases (cfg_file, &list);

                it = dl_list_find (dl_list_head (list)
                                   , dl_list_tail (list)
                                   , compare_id, &id);

                if (it != DLListNULLIterator) {
                        tci = (TestCaseInfo *) dl_list_data (it);
                        if (tci != INITPTR) {
                                if (tci->test_ != (ptr2test) INITPTR) {
                                        tci->test_ (result);
                                }
                        } else
                                retval = -1;
                } else
                        retval = -1;
        } else
                retval = -1;

        dl_list_free (&list);
        return retval;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
