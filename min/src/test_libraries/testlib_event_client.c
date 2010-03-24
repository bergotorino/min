/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       testlib_event_client.c
 *  @version    0.1
 *  @brief      Test module acting as client for event statetest
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <test_module_api.h>
#include <min_test_event_if.h>
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
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */

int statetest_register (TestCaseResult * tcr)
{
        minTestEventParam_t e;
        event           e;

        e.event.event_type_ = EState;
        strcpy (e.event.event_name_, "statetest");
        /* 
         ** Register for event 
         */
        e.command_ = EReqEvent;
        min_event (&e);
        MIN_DEBUG3 ("REGISTERED FOR SLEEP60 NOW WAIT\n");
        /*
         ** Wait for the event
         */
        e.command_ = EWaitEvent;
        min_event (&e);

        RESULT (tcr, TP_PASSED, "PASSED");
}


/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int tm_get_test_cases (const char *cfg_file, DLList ** cases)
{
        int             retval = 0;
        ENTRY (*cases, "wait4sleep", wait4sleep);
        return retval;
}

/* ------------------------------------------------------------------------- */
int tm_run_test_case (unsigned int id, const char *cfg_file,
                      TestCaseResult * result)
{
        /* Needed variables, please do not edit this section */
        int             retval = 0;
        DLList         *list = dl_list_create ();
        DLListIterator  it = DLListNULLIterator;
        TestCaseInfo   *tci = INITPTR;

        /* Check if list was initialized properly */
        if (list != INITPTR) {

                /* Get test cases connected with this test file */
                tm_get_test_cases (cfg_file, &list);

                /* Find test case of a given id */
                it = dl_list_find (dl_list_head (list)
                                   , dl_list_tail (list)
                                   , compare_id, &id);

                /* Check if there is a test case of a given id */
                if (it != DLListNULLIterator) {

                        /* Get the test case detailed information */
                        tci = (TestCaseInfo *) dl_list_data (it);

                        if (tci != INITPTR) {

                                /* If test function is specified run a tc
                                 * For hardcoded test library we assume that 
                                 * test functions are specified.
                                 */
                                if (tci->test_ != (ptr2test) INITPTR) {
                                        tci->test_ (result);
                                }
                        }
                }

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
