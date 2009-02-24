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
 *  @file       min_engine_api.c
 *  @version    0.1
 *  @brief      This file contains implementation of test execution 
 *              controller functionality
 */

/* ----------------------------------------------------------------------------
 * INCLUDE FILES
 */
#include <dllist.h>
#include <tec.h>
#include <min_logger.h>
#include <data_api.h>
#include <min_engine_api.h>

/* ----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 */
DLList *modules;
eapiIn_t *in;

/* ----------------------------------------------------------------------------
 * EXTERNAL DATA STRUCTURES
 */
extern pthread_mutex_t tec_mutex_;


/* ---------------------------------------------------------------------------
 * EXTERNAL FUNCTION PROTOTYPES
 */
/* None */

/* ----------------------------------------------------------------------------
 * CONSTANTS
 */
/* None */

/* ----------------------------------------------------------------------------
 * MACROS
 */
/* None */


/* ----------------------------------------------------------------------------
 * LOCAL CONSTANTS AND MACROS
 */
/* None */

/* ----------------------------------------------------------------------------
 * MODULE DATA STRUCTURES
 */
/* None */

/* ----------------------------------------------------------------------------
 * LOCAL FUNCTION PROTOTYPES
 */
/** Searches for test case by pid from the given list
 *  @param list_handle pointer to linked list of test case data
 *  @param pid search key
 *  @return pointer to test case data item,
 *          or returns INITPTR if operation failed.  
 *
 */
LOCAL DLListIterator tc_find_by_pid (DLList * list_handle, long tm_pid);

/* -------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */

LOCAL void eapi_add_test_module (char *modulepath)
{
        test_module_info_s *modinfo = INITPTR;

        modinfo = tm_create (modulepath, INITPTR, 0);
        if (tm_add (modules, modinfo) != INITPTR) {
                if (in->new_module) {
                        in->new_module (modulepath, modinfo->module_id_);
                }
        } else {
                MIN_WARN ("failed to add module");
                if (in->no_module)  in->no_module (modulepath);
        }
                
        return;
}

/* ------------------------------------------------------------------------- */

LOCAL void eapi_add_test_case_file (unsigned module_id, char *testcasefile)
{
        DLListIterator it;
        test_module_info_s *modinfo;
        char *p;

        it = tm_find_by_module_id (modules, module_id);

        if (it == INITPTR) {
                MIN_WARN ("No module with id %u found", module_id);
                return;
        }
        modinfo = dl_list_data (it);
        if (*testcasefile != '\0') {
                p =  NEW2 (char, strlen (testcasefile) + 1);
                STRCPY (p, testcasefile, strlen (testcasefile) + 1);
                dl_list_add (modinfo->cfg_filename_list_, p);
        } else {
                /* we have all the test case files for the module,
                   so we can add the module into engine */
                ec_add_module (modinfo->module_filename_, 
                               modinfo->cfg_filename_list_,
                               modinfo->module_id_);
                
        } 
}

/* ------------------------------------------------------------------------- */

LOCAL void eapi_start_test_case (unsigned module_id, unsigned case_id)
{
        int             result = 0;
	DLListIterator mod_it, case_it;
        test_module_info_s *module;
	
        pthread_mutex_lock (&tec_mutex_);

	mod_it = tm_find_by_module_id (instantiated_modules, module_id);
	if (mod_it == INITPTR) {
		MIN_WARN ("No module by id %d found", module_id);
		return;
	}
	module = (test_module_info_s *)dl_list_data (mod_it);
	case_it = tc_find_by_case_id (module->test_case_list_, case_id);
	if (case_it == INITPTR) {
		MIN_WARN ("No case by id %d found", case_id);
		return;
	}

	/*add to selected cases list */
        case_it = ec_select_case (case_it, 0);

	if (case_it == INITPTR) {
		MIN_WARN ("No case by id %d found", case_id);
		return;
	}

	/*add to selected cases list */
        //case_it = ec_select_case (case_it, 0);

        pthread_mutex_unlock (&tec_mutex_);
        result = ec_exec_case (case_it);

        return;
}

/* ------------------------------------------------------------------------- */

LOCAL void eapi_pause_case (long test_run_id)
{
	DLListIterator it;

	it = tc_find_by_pid (selected_cases, test_run_id);
	if (it != INITPTR)
		ec_pause_test_case (it);
}

/* ------------------------------------------------------------------------- */

LOCAL void eapi_resume_case (long test_run_id)
{
	DLListIterator it;

	it = tc_find_by_pid (selected_cases, test_run_id);
	if (it != INITPTR)
		ec_resume_test_case (it);

}

/* ------------------------------------------------------------------------- */

LOCAL void eapi_error (char *what, char *msg)
{
        MIN_FATAL ("%s - %s",what,msg);
}

/* ------------------------------------------------------------------------- */

LOCAL void eapi_open (char *what, char *msg)
{
        MIN_DEBUG ("Opening");
        MIN_DEBUG ("Open");
}


/* ------------------------------------------------------------------------- */

LOCAL void eapi_close (char *what, char *msg)
{
        MIN_DEBUG ("Closing");
	ec_cleanup();
        MIN_DEBUG ("Closed");
}

/* ------------------------------------------------------------------------- */

LOCAL DLListIterator tc_find_by_pid (DLList * list_handle, long tm_pid)
{
	test_case_s    *tc;
	test_module_info_s *tm;
        DLListIterator  it;

        
        for (it = dl_list_head (list_handle); it != INITPTR;
             it = dl_list_next(it)) {
		tc = (test_case_s *)dl_list_data (it);
		tm = (test_module_info_s *)dl_list_data (tc->tm_data_item_);
		if (tm->process_id_ == tm_pid) {
                        return it;
                }
        }

        
        return INITPTR;

}

/* ======================== FUNCTIONS ====================================== */

void eapi_init (eapiIn_t *inp, eapiOut_t *out)
{
        in = inp;

        modules = dl_list_create();        
        out->add_test_module = eapi_add_test_module;
        out->add_test_case_file = eapi_add_test_case_file;
        out->start_case = eapi_start_test_case;
	out->pause_case = eapi_pause_case;
	out->resume_case = eapi_resume_case;
        out->fatal_error = eapi_error;
	out->min_close = eapi_close;
}


/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
//#include "min_engine_api.tests"
#endif                          /* MIN_UNIT_TEST */

/* End of file */
