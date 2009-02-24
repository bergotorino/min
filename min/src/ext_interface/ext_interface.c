/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Robert Galkowski
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
 *  @file       ext_interface.c
 *  @version    0.1
 *  @brief      This file contains implementation of min external interface
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <ext_interface.h>
#include <data_api.h>
#include <tec.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern DLList  *instantiated_modules;   /* list of running test modules */
extern DLList  *selected_cases; /*list of test cases selected for execution */

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
/* None */

/* ======================== FUNCTIONS ====================================== */

int ext_if_exec ()
{
        int             cont_flag = 1;
        DLListIterator  work_module_item = INITPTR;
        DLListIterator  work_case_item = INITPTR;
        DLList         *work_cases_list = INITPTR;
	/*this list will hold all available test cases */
        DLList         *all_cases_list = INITPTR;
        all_cases_list = dl_list_create ();
        /*get all available test cases */
        work_module_item = dl_list_head (available_modules);

        while (work_module_item != DLListNULLIterator) {

                /*get all cases from module and put them in
                   selected list */
                work_cases_list = tm_get_tclist (work_module_item);
                work_case_item = dl_list_head (work_cases_list);

                while (work_case_item != DLListNULLIterator) {

                        tc_add (all_cases_list,
                                (test_case_s *)
                                dl_list_data (work_case_item));
                        work_case_item = dl_list_next (work_case_item);

                }

                work_module_item = dl_list_next (work_module_item);

        }

        MIN_DEBUG ("%d cases selected ", dl_list_size (all_cases_list));
        ec_run_cases_seq (all_cases_list);
        dl_list_free (&all_cases_list);


        /*now start waiting for all test cases to finish */


        while (cont_flag != 0) {

                cont_flag = 0;
                work_case_item = dl_list_head (selected_cases);

                while (work_case_item != DLListNULLIterator) {

                        if (tc_get_status (work_case_item) !=
                            TEST_CASE_TERMINATED) {

                                /* at least one case not finished */
                                cont_flag = 1;
                                break;
                        }

                        work_case_item = dl_list_next (work_case_item);

                }

                sleep (1);

        }
	ec_cleanup();
        log_summary ();
        return 0;
}

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "check.h"

/* ------------------------------------------------------------------------- */

int template_tests ()
{
        int             number_failed;

        number_failed = 0;

        return number_failed;
}

/* ------------------------------------------------------------------------- */

#endif                          /* MIN_UNIT_TEST */

/* End of file */
