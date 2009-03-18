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
 *  @file       mintfwif.c
 *  @version    0.1
 *  @brief      This file contains implementation of mintfwif
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <ctype.h>
#include <string.h>
#include <tec.h>
#include <data_api.h>
#include <mintfwif.h>
#include <min_engine_api.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

extern char    *strcasestr (const char *haystack, const char *needle);

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */


/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* List of cases */
DLList *case_list_ = INITPTR;
DLList *executed_case_list_ = INITPTR;
DLList *error_list_ = INITPTR;

/* List of modules */
DLList  *available_modules = INITPTR;
DLList  *test_modules = INITPTR;

/* List of test case files selected for added module */
DLList *found_tcase_files = INITPTR;
DLList *test_case_files = INITPTR;



/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */

eapiIn_t in_clbk_;
eapiOut_t min_clbk_;

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */
/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */

LOCAL void pl_case_result (long testrunid, int result, char *desc,
		                        long starttime, long endtime);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_started (unsigned moduleid,
	                    unsigned caseid,
                            long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_paused (long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_resumed (long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_msg_print (long testrunid, char *message);
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_module (char *modulename, unsigned moduleid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_no_module (char *modulename);
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle);
/* ------------------------------------------------------------------------- */
LOCAL void pl_error_report (char *error);


/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int min_if_set_device_id (int device_id)
{
        own_id = device_id;
        return 0;
}

int min_if_open (min_case_complete_func complete_cb,
                  min_case_print_func print_cb,
                  min_extif_message_cb_ extifsend_cb, char *engine_ini,
                  char *envp[])
{
        int             cont_flag = 0;
        int             status;
        int             i = 0;
	
        DLListIterator  work_module_item = DLListNULLIterator;

	/* create linked list for test cases */
        user_selected_cases = dl_list_create ();

        /* create linked list for test modules */
        test_modules = dl_list_create ();

        /* create linked list for testcase files */
        test_case_files = dl_list_create ();

        while (cont_flag == 0) {
                usleep (500000);
                i++;
                work_module_item = dl_list_head (instantiated_modules);
                cont_flag = 1;
                while (work_module_item != DLListNULLIterator) {
                        status = tm_get_status (work_module_item);
                        if (status == TEST_MODULE_READY)
                                cont_flag = cont_flag | 1;
                        else
                                cont_flag = 0;
                        work_module_item = dl_list_next (work_module_item);
                }
                if (i > 20)
                        exit (1);
        }

        if ((engine_ini != NULL) && (strlen (engine_ini) != 0)) {
                ec_read_settings (engine_ini);

        }
        return 0;
}

/* ------------------------------------------------------------------------- */
int min_if_close ()
{
        if(min_clbk_.min_close)
	{
		min_clbk_.min_close();
	}

        return 0;
}


/* ------------------------------------------------------------------------- */
int min_if_message_received (char *message, int length)
{
        int             result = tec_extif_message_received (message, length);

        return result;
}

/* ------------------------------------------------------------------------- */
int min_if_exec_case (char *module, unsigned int id)
{
        int             retval = -1;
        DLListIterator  work_module_item = INITPTR;
        DLListIterator  work_case_item = INITPTR;
        char           *module_name = NEW2 (char, MaxFileName);
	unsigned 	module_id;

        work_module_item = dl_list_head (instantiated_modules);
        while (work_module_item != DLListNULLIterator) {
                tm_get_module_filename (work_module_item, module_name);
                if ((strcasestr (module, module_name) != NULL) ||
                    (strcasestr (module_name, module) != NULL)) {
			module_id=tm_get_module_id(work_module_item);
                        work_case_item =
                            dl_list_at (tm_get_tclist (work_module_item), id);
                        break;
                }
                work_module_item = dl_list_next (work_module_item);
        }

        retval = ec_exec_test_case (work_case_item);
        if (retval == 0)
                retval = dl_list_size (selected_cases) - 1;
        DELETE (module_name);

        return retval;
}

/* ------------------------------------------------------------------------- */
int min_if_cancel_case (unsigned int runtime_id)
{
        int             result = 0;
        DLListIterator  work_case_item = INITPTR;

        work_case_item = dl_list_at (selected_cases, runtime_id);

        if (work_case_item != INITPTR)
                result = ec_abort_test_case (work_case_item);
        else
                return -3;

        /*translate result of ec_cancel_test_case
           to return value */

        switch (result) {
        case (0):
                return 0;
        case (-1):
                return -1;
        case (-2):
                return -1;
        default:
                break;
        }

        return -4;
}

/* ------------------------------------------------------------------------- */
int min_if_pause_case (unsigned int runtime_id)
{


        DLListIterator  work_case_item = DLListNULLIterator;
        int             result = 0;

        work_case_item = dl_list_at (selected_cases, runtime_id);

        /* work case could not be fetched from the list, invalid arg. passed */
        if (work_case_item == DLListNULLIterator) {
                MIN_WARN ("wrong_argument");
                MIN_WARN ("there is %d selected cases",
                             dl_list_size (selected_cases));
                return -3;

        }

        result = ec_pause_test_case (work_case_item);

        /*translate result of ec_pause_test_case
           to return value */

        switch (result) {
        case (0):
                return 0;
        case (-2):
                return -1;
        case (-3):
                return -2;
        default:
                break;

        }

        return -4;
}

/* ------------------------------------------------------------------------- */

int min_if_get_cases (module_info ** modules_arg)
{

        int             extif_list_size = 0;
        int             mod_counter = 0;
        int             case_counter = 0;
        int             cases_count = 0;
        DLListIterator  work_module_item = DLListNULLIterator;
        DLListIterator  work_case_item = DLListNULLIterator;
        char           *title_string = NEW2 (char, MaxFileName);

        module_info    *modules = NULL;

        extif_list_size = dl_list_size (instantiated_modules);
        modules = NEW2 (module_info, extif_list_size);
        for (mod_counter = 0; mod_counter < extif_list_size; mod_counter++) {
                work_module_item =
                    dl_list_at (instantiated_modules, mod_counter);
                tm_get_module_filename (work_module_item, title_string);
                STRCPY (modules[mod_counter].module_name_, title_string, 128);
                cases_count = dl_list_size (tm_get_tclist (work_module_item));
                modules[mod_counter].num_test_cases_ = cases_count;
                modules[mod_counter].test_cases_ =
                    NEW2 (min_case, cases_count);
                /*write test cases data) */
                for (case_counter = 0; case_counter < cases_count;
                     case_counter++) {
                        work_case_item =
                            dl_list_at (tm_get_tclist (work_module_item),
                                        case_counter);
                        modules[mod_counter].test_cases_[case_counter].
                            case_id_ = case_counter;
                        tc_get_title (work_case_item, title_string);
                        STRCPY (modules[mod_counter].
                                test_cases_[case_counter].case_name_,
                                title_string, 256);
                }
        }
        DELETE (title_string);
        *modules_arg = modules;
        MIN_WARN ("Number of cases = %d, modules: %x", extif_list_size,
                     modules);

        return extif_list_size;
}

/* ------------------------------------------------------------------------- */

int min_if_resume_case (unsigned int runtime_id)
{
        DLListIterator  work_case_item = DLListNULLIterator;
        int             result = 0;

        work_case_item = dl_list_at (selected_cases, runtime_id);
        /*work case could not be fetched from the list, invalid arg. passsed */
        if (work_case_item == DLListNULLIterator) {
                MIN_WARN ("wrong_argument");
                MIN_WARN ("there is %d selected cases",
                           dl_list_size (selected_cases));
                return -3;

        }

        result = ec_resume_test_case (work_case_item);

        switch (result) {
        case (0):
                return 0;
        case (-2):
                return -1;
        case (-3):
                return -2;
        default:
                break;
        }

        return -4;
}

/* ------------------------------------------------------------------------- */
int min_if_module_add (char *module_name, char *conf_name)
{
        int             result = 0;
        DLList         *conf_list = INITPTR;

        TSChar         *tcf_name;
        TSChar         *mod_name;
        int             cont_flag = 0;
        int             i = 0;
        int             old_number_of_modules = 0;
        /*diff between old and new number of instantiated modules
           indicates if new module was successfully added */
        DLListIterator  work_module_item = DLListNULLIterator;
        test_module_status_t status;

        MIN_DEBUG ("arg: modulename=%s : configuration_file=%s", 
                    module_name, conf_name);

        old_number_of_modules = dl_list_size (instantiated_modules);
        conf_list = dl_list_create ();
        if (conf_name != NULL) {
                tcf_name = NEW2 (char, strlen (conf_name) + 1);
                sprintf (tcf_name, "%s", conf_name);
                dl_list_add (conf_list, (void *)tcf_name);
        }
        mod_name = NEW2 (TSChar, strlen (module_name) + 1);
        sprintf (mod_name, "%s", module_name);
        result = ec_add_module (mod_name, conf_list, 0);
        if (result == 0) {
                while (cont_flag == 0) {
                        usleep (500000);
                        i++;
                        work_module_item =
                            dl_list_head (instantiated_modules);
                        cont_flag = 1;
                        while (work_module_item != DLListNULLIterator) {
                                status = tm_get_status (work_module_item);
                                if (status == TEST_MODULE_READY)
                                        cont_flag = cont_flag | 1;
                                else
                                        cont_flag = 0;
                                work_module_item =
                                    dl_list_next (work_module_item);
                        }
                        if (i > 20)
                                break;
                }
        }
        DELETE (mod_name);
        return result;
}


void pl_attach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        /* Binds the callbacks */
        memcpy (&min_clbk_,in_callback,sizeof(eapiOut_t));

        (*out_callback)->case_result            = pl_case_result;
        (*out_callback)->case_started           = pl_case_started;
        (*out_callback)->case_paused            = pl_case_paused;
        (*out_callback)->case_resumed           = pl_case_resumed;
        (*out_callback)->module_prints          = pl_msg_print;
        (*out_callback)->new_module             = pl_new_module;
        (*out_callback)->no_module              = pl_no_module;
        (*out_callback)->module_ready           = NULL;
        (*out_callback)->new_case               = pl_new_case;
        (*out_callback)->error_report           = pl_error_report;

        return;
}
/* ------------------------------------------------------------------------- */
void pl_open_plugin (void *arg)
{
        return;
}
/* ------------------------------------------------------------------------- */
void pl_close_plugin ()
{
        return;
}
/* ------------------------------------------------------------------------- */
void pl_detach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        return;
}
/* ------------------------------------------------------------------------- */


LOCAL void pl_case_result (long testrunid, int result, char *desc,
                        long starttime, long endtime){
	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_started (unsigned moduleid,
                      unsigned caseid,
                      long testrunid){
	return;	
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_paused (long testrunid){
	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_resumed (long testrunid){
	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_msg_print (long testrunid, char *message){
	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_module (char *modulename, unsigned moduleid){
	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_no_module (char *modulename){
	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle){
	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_error_report (char *error){
	return;
};





/*---------------------------------------------------------------------------*/
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "mintfwif.tests"

#endif                          /* MIN_UNIT_TEST */

/* End of file */
