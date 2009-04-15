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
#include <tmc_common.h>
#include <dllist.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */

extern char    *strcasestr (const char *haystack, const char *needle);
extern eapiIn_t in_str;



/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
typedef struct {
        char            module_name_[128];
        DLList         *test_case_list_;
	unsigned        module_id_;
	int             module_ready_;
} internal_module_info;


/** Structure for storage of executed test case data*/
typedef struct {
        /** Test Case ID */
        unsigned int    case_id_;
        /** Module ID */
        unsigned int    module_id_;
        /** Test run ID */
        unsigned int    test_run_id_;
	/** Test run status */
	MINTPStatus	status_;
	/** Test run group ID */
	unsigned int	group_id_;
} internal_test_run_info;


/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* module list */
DLList *tfwif_modules_ = INITPTR;
DLList *tfwif_test_runs_ = INITPTR;
/* number of ready modules */
unsigned ready_module_count_ = 0;
pthread_mutex_t tfwif_mutex_ = PTHREAD_MUTEX_INITIALIZER;
tfwif_callbacks_s tfwif_callbacks;

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */

int already_executed = 0;
eapiOut_t min_clbk_;
eapiIn_t *in;
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
LOCAL void pl_module_ready (unsigned moduleid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_no_module (char *modulename);
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle);
/* ------------------------------------------------------------------------- */
LOCAL void pl_error_report (char *error);
/* ------------------------------------------------------------------------- */
LOCAL void pl_send_rcp (char* message, int length);
/* ------------------------------------------------------------------------- */
LOCAL int _find_mod_by_id (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
LOCAL int _find_testrun_by_id (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
LOCAL int _find_mod_by_name (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
LOCAL void _del_internal_mod_info (void *data);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
void pl_attach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback);

/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int min_if_set_device_id (int device_id)
{
        own_id = device_id;
        return 0;
}

/* ------------------------------------------------------------------------- */


int min_if_open (min_case_complete_func complete_cb,
		 min_case_print_func print_cb,
		 min_extif_message_cb_ extifsend_cb, char *engine_ini,
		 char *envp[])
{
	int module_count;

        tfwif_callbacks.complete_callback_ = complete_cb;
        tfwif_callbacks.print_callback_ = print_cb;
        tfwif_callbacks.send_extif_msg_ = extifsend_cb;

	if (tfwif_modules_ == INITPTR)
		tfwif_modules_ = dl_list_create();

        if (!already_executed) {
                in = &in_str;
                eapi_init (in, &min_clbk_);
        }
	pl_attach_plugin (&in, &min_clbk_);
	ec_min_init (envp, 1);
	already_executed++;

	module_count = min_clbk_.min_open();
	while (module_count > ready_module_count_) {
		usleep (50000);
	}
	usleep (500000);
	if ((engine_ini != NULL) && (strlen (engine_ini) != 0)) {
                ec_read_settings (engine_ini);

        }

	return 0;
}

/* ------------------------------------------------------------------------- */
int min_if_close ()
{
	if(min_clbk_.min_close) {
		min_clbk_.min_close();
	}
	/*
	** Clear lists 
	*/
	dl_list_foreach (dl_list_head (tfwif_modules_),
			 dl_list_tail (tfwif_modules_),
			 _del_internal_mod_info);
	dl_list_free (&tfwif_modules_);
	dl_list_foreach (dl_list_head (tfwif_test_runs_),
			 dl_list_tail (tfwif_test_runs_),
			 free);
	dl_list_free (&tfwif_test_runs_);

	ready_module_count_ = 0;
	usleep (500000);

        return 0;
}


/* ------------------------------------------------------------------------- */
int min_if_message_received (char *message, int length)
{
	if(min_clbk_.receive_rcp) {
		return min_clbk_.receive_rcp(message, length);
	}
	

        return 1;
}

/* ------------------------------------------------------------------------- */
int min_if_exec_case (char *module, unsigned int id)
{
	internal_module_info *mi;
	internal_test_run_info *tri = INITPTR;

	DLListIterator it;
	int cont = 1;
	pthread_mutex_lock (&tfwif_mutex_);
	it = dl_list_find (dl_list_head (tfwif_modules_),
			   dl_list_tail (tfwif_modules_),
			   _find_mod_by_name,
			   (const void *)module);
	pthread_mutex_unlock (&tfwif_mutex_);

	if (it == INITPTR)
		return 1;

	mi = dl_list_data (it);

	if (!min_clbk_.start_case (mi->module_id_, id, 0)) {
		while (cont) {
			usleep (10000);
			pthread_mutex_lock (&tfwif_mutex_);
			for (it = dl_list_head (tfwif_test_runs_);
			     it != INITPTR;
			     it = dl_list_next (it)) {
				tri = dl_list_data (it);
				if (tri->module_id_ != mi->module_id_)
					continue;
				if (tri->status_ != TP_RUNNING)
					continue;
				if (tri->case_id_ != id)
					continue;
				/* we found the just started test case */
				cont = 0;
				break;
				
			}
			pthread_mutex_unlock (&tfwif_mutex_);

		}
	} else
		return -1;

	MIN_DEBUG("test started with runid=%d", tri->test_run_id_);
	return tri->test_run_id_;
}

/* ------------------------------------------------------------------------- */
int min_if_cancel_case (unsigned int runtime_id)
{
	DLListIterator it;


	pthread_mutex_lock (&tfwif_mutex_);
	it = dl_list_find (dl_list_head (tfwif_test_runs_),
			   dl_list_tail (tfwif_test_runs_),
			   _find_testrun_by_id,
			   (const void *)&runtime_id);
	pthread_mutex_unlock (&tfwif_mutex_);

	if (it == INITPTR)
		return 1;

	return min_clbk_.abort_case (runtime_id);
}

/* ------------------------------------------------------------------------- */
int min_if_pause_case (unsigned int runtime_id)
{
	DLListIterator it;

	pthread_mutex_lock (&tfwif_mutex_);
	it = dl_list_find (dl_list_head (tfwif_test_runs_),
			   dl_list_tail (tfwif_test_runs_),
			   _find_testrun_by_id,
			   (const void *)&runtime_id);
	pthread_mutex_unlock (&tfwif_mutex_);

	if (it == INITPTR)
		return 1;

	return min_clbk_.pause_case (runtime_id);
}

/* ------------------------------------------------------------------------- */

int min_if_get_cases (module_info ** modules_arg)
{

        int             extif_list_size = 0;
        DLListIterator  mod_it = DLListNULLIterator;
        DLListIterator  case_it = DLListNULLIterator;
	int i = 0, j, cases_count;
        module_info    *modules = NULL;
	internal_module_info *mi;
	min_case *mc;

        extif_list_size = dl_list_size(tfwif_modules_);
        modules = NEW2 (module_info, extif_list_size);
				
	pthread_mutex_lock (&tfwif_mutex_);
	for (mod_it = dl_list_head (tfwif_modules_);
	     mod_it != DLListNULLIterator;
	     mod_it = dl_list_next (mod_it)) {
		mi = dl_list_data (mod_it);
		
		STRCPY (modules[i].module_name_, mi->module_name_, 128);
		     
		cases_count = dl_list_size (mi->test_case_list_);
		modules[i].num_test_cases_ = cases_count;
		modules[i].test_cases_ = NEW2 (min_case, cases_count);
		j = 0;
		for (case_it = dl_list_head (mi->test_case_list_);
		     case_it != DLListNULLIterator;
		     case_it = dl_list_next (case_it)) {
			mc = dl_list_data (case_it);
			modules[i].test_cases_[j] = *mc;
			j++;
		     }
		i ++;
        }
	pthread_mutex_unlock (&tfwif_mutex_);

        *modules_arg = modules;
        MIN_WARN ("Number of cases = %d, modules: %x", extif_list_size,
		  modules);
	
        return extif_list_size;
}

/* ------------------------------------------------------------------------- */

int min_if_resume_case (unsigned int runtime_id)
{
	DLListIterator it;

	pthread_mutex_lock (&tfwif_mutex_);
	it = dl_list_find (dl_list_head (tfwif_test_runs_),
			   dl_list_tail (tfwif_test_runs_),
			   _find_testrun_by_id,
			   (const void *)&runtime_id);
	pthread_mutex_unlock (&tfwif_mutex_);
			
	if (it == INITPTR)
		return 1;

	return min_clbk_.resume_case (runtime_id);
}

/* ------------------------------------------------------------------------- */
int min_if_module_add (char *module_name, char *conf_name)
{
	DLListIterator it = INITPTR;
	internal_module_info *mi;

	MIN_DEBUG ("MODULE:%s, CONFIG:%s", module_name, 
		   conf_name != NULL ? conf_name : "<null>");
	if (min_clbk_.add_test_module (module_name)) {
		MIN_WARN ("Returning error");
		return 1;
	}
	while (it == INITPTR) {

		usleep (500000);

		pthread_mutex_lock (&tfwif_mutex_);
		it = dl_list_find (dl_list_head (tfwif_modules_),
				   dl_list_tail (tfwif_modules_),
				   _find_mod_by_name,
				   (const void *)module_name);
		pthread_mutex_unlock (&tfwif_mutex_);

	}

	mi = dl_list_data (it);
	if (conf_name && 
	    min_clbk_.add_test_case_file (mi->module_id_, conf_name)) {
		MIN_WARN ("returning error");
		return 1;
	}
	min_clbk_.add_test_case_file (mi->module_id_, "");

	while (!mi->module_ready_)
		usleep (10000);

	if (dl_list_size (mi->test_case_list_) == 0) {
		/*
		** Module added succesfully, but does not contain any cases,
		** we can remove it now.
		*/
		pthread_mutex_lock (&tfwif_mutex_);
		dl_list_remove_it (it);
		_del_internal_mod_info (mi);
		pthread_mutex_unlock (&tfwif_mutex_);

	}

	return 0;
}


void pl_attach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        /* Binds the callbacks */


       (*out_callback)->case_result            = pl_case_result;
       (*out_callback)->case_started           = pl_case_started;
       (*out_callback)->case_paused            = pl_case_paused;
       (*out_callback)->case_resumed           = pl_case_resumed;
       (*out_callback)->module_prints          = pl_msg_print;
       (*out_callback)->new_module             = pl_new_module;
       (*out_callback)->no_module              = pl_no_module;
       (*out_callback)->module_ready           = pl_module_ready;
       (*out_callback)->new_case               = pl_new_case;
       (*out_callback)->error_report           = pl_error_report;
       (*out_callback)->send_rcp               = pl_send_rcp;
       
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

	
	internal_test_run_info *tri = INITPTR;
	DLListIterator	test_run_item = DLListNULLIterator;

	pthread_mutex_lock (&tfwif_mutex_);
	test_run_item = dl_list_find (dl_list_head (tfwif_test_runs_),
			              dl_list_tail (tfwif_test_runs_),
			              _find_testrun_by_id,
	                              (const void *)&testrunid);
	pthread_mutex_unlock (&tfwif_mutex_);

	if (test_run_item == DLListNULLIterator) {
		MIN_WARN ("no matching test run info found testrun id = %d",
			  testrunid);
		return;
	}
	tri = dl_list_data (test_run_item);
	tri->status_ = TP_ENDED;
	tfwif_callbacks.complete_callback_ (tri->test_run_id_, 1, result, desc);


	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_started (unsigned moduleid,
			    unsigned caseid,
			    long testrunid)
{
	internal_test_run_info *tri;

	if(tfwif_test_runs_==INITPTR){
		tfwif_test_runs_=dl_list_create();
	}

	tri = NEW (internal_test_run_info);
	tri->case_id_ = caseid;
	tri->test_run_id_ = testrunid;
	tri->module_id_ = moduleid;
	tri->status_ = TP_RUNNING;
	tri->group_id_=0;

	dl_list_add (tfwif_test_runs_, tri);

	return;	
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_paused (long testrunid)
{
	internal_test_run_info *tri = INITPTR;
	DLListIterator	test_run_item = DLListNULLIterator;


	pthread_mutex_lock (&tfwif_mutex_);
	test_run_item = dl_list_find (dl_list_head (tfwif_test_runs_),
			              dl_list_tail (tfwif_test_runs_),
			              _find_testrun_by_id,
	                              (const void *)&testrunid);
	pthread_mutex_unlock (&tfwif_mutex_);

	tri = dl_list_data (test_run_item);
	if(tri->status_ == TP_RUNNING &&
	   tri->test_run_id_ == testrunid){
		tri->status_ = TP_PAUSED;
	}

	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_resumed (long testrunid)
{
	internal_test_run_info *tri = INITPTR;
	DLListIterator	test_run_item = DLListNULLIterator;


	pthread_mutex_lock (&tfwif_mutex_);
	test_run_item = dl_list_find (dl_list_head (tfwif_test_runs_),
			              dl_list_tail (tfwif_test_runs_),
			              _find_testrun_by_id,
	                              (const void *)&testrunid);
	pthread_mutex_unlock (&tfwif_mutex_);

	tri = dl_list_data (test_run_item);
	if(tri->status_==TP_PAUSED &&
	   tri->test_run_id_ == testrunid){
		tri->status_ = TP_RUNNING;
	}

	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_msg_print (long testrunid, char *message)
{

	printf ("test module message: %s\n", message);
        tfwif_callbacks.print_callback_ (testrunid, message);

	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_module (char *modulename, unsigned moduleid)
{
	internal_module_info *mi;
	
	mi = NEW (internal_module_info);
	STRCPY(mi->module_name_, modulename, 128);
        mi->test_case_list_ = dl_list_create();
	mi->module_id_ = moduleid;
	mi->module_ready_ = 0;

	pthread_mutex_lock (&tfwif_mutex_);
	dl_list_add (tfwif_modules_, mi);
	pthread_mutex_unlock (&tfwif_mutex_);



	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_module_ready ( unsigned moduleid)
{
	internal_module_info *mi;
	DLListIterator it;
	
	pthread_mutex_lock (&tfwif_mutex_);

	it = dl_list_find (dl_list_head (tfwif_modules_),
			   dl_list_tail (tfwif_modules_),
			   _find_mod_by_id,
			   (const void *)&moduleid);
	pthread_mutex_unlock (&tfwif_mutex_);
	if (it == INITPTR)
		return;

	ready_module_count_ ++;
	mi = dl_list_data (it);
	mi->module_ready_ = 1;

	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_no_module (char *modulename)
{
	return;
};
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle)
{
	internal_module_info *mi;
	min_case *mc;
	DLListIterator it;

	pthread_mutex_lock (&tfwif_mutex_);

	it = dl_list_find (dl_list_head (tfwif_modules_),
			   dl_list_tail (tfwif_modules_),
			   _find_mod_by_id,
			   (const void *)&moduleid);

	if (it == INITPTR) {
		pthread_mutex_unlock (&tfwif_mutex_);
		MIN_WARN ("NO MODULE FOUND WITH ID %s",moduleid);
		return;
	}
	mi = dl_list_data (it);
	mc = NEW(min_case);
	mc->case_id_ = caseid;
	STRCPY (mc->case_name_, casetitle, 256);
	dl_list_add (mi->test_case_list_, mc);
	pthread_mutex_unlock (&tfwif_mutex_);

	return;
};

/* ------------------------------------------------------------------------- */
LOCAL void pl_error_report (char *error)
{
	printf ("%s\n", error);
	return;
};

/* ------------------------------------------------------------------------- */
LOCAL void pl_send_rcp (char* message, int length)
{
	tfwif_callbacks.send_extif_msg_ (message, length);

	return;
};

/* ------------------------------------------------------------------------- */
LOCAL int _find_mod_by_id (const void *a, const void *b)
{
        internal_module_info *tmp1 = (internal_module_info*)a;
        unsigned *tmp2 = (unsigned*)b;

        if (tmp1->module_id_ ==(*tmp2)) return 0;
        else return -1;
}

/* ------------------------------------------------------------------------- */
LOCAL int _find_mod_by_name (const void *a, const void *b)
{
        internal_module_info *tmp1 = (internal_module_info*)a;

	return strncmp (tmp1->module_name_, (const char *)b, 128); 
}

/*---------------------------------------------------------------------------*/
LOCAL int _find_testrun_by_id (const void *a, const void *b)
{
        internal_test_run_info *trp1 = (internal_test_run_info*)a;
        unsigned *tmp2 = (unsigned*)b;

        if (trp1->test_run_id_ ==(*tmp2)) return 0;
        else return -1;
}

/*---------------------------------------------------------------------------*/
LOCAL void _del_internal_mod_info (void *data)
{
        internal_module_info *mi = (internal_module_info*)data;
	dl_list_foreach (dl_list_head (mi->test_case_list_),
			 dl_list_tail (mi->test_case_list_),
			 free);
	dl_list_free (&mi->test_case_list_);
	DELETE (mi);
}

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "mintfwif.tests"

#endif                          /* MIN_UNIT_TEST */

/* End of file */
