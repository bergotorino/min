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
 *  @file       cli.c
 *  @version    0.1
 *  @brief      This file contains implementation of commandline interface
 *              plugin for MIN
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <cli.h>
#include <min_system_logger.h>
#include <min_plugin_interface.h>
#include <min_text.h>
#include <min_logger.h>
#include <tllib.h>

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
/* LOCAL GLOBAL VARIABLES */
DLList   *available_modules = INITPTR;
DLList   *case_list_ = INITPTR;
DLList   *executed_case_list_ = INITPTR;
cli_opts  cliopts;
unsigned  ready_module_count_ = 0;
unsigned  available_case_count_ = 0;
unsigned  case_result_count_ = 0;
unsigned  exit_ = 0;
eapiOut_t min_clbk_;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
struct ExecutedTestCase;

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* ------------------------------------------------------------------------- */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL int _find_case_by_id (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
LOCAL int _find_mod_by_id (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
LOCAL struct ExecutedTestCase *get_executed_tcase_with_runid (long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_module (char *modulename, unsigned moduleid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_no_module (char *modulename);
/* ------------------------------------------------------------------------- */
LOCAL void pl_module_ready (unsigned moduleid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_started (unsigned moduleid,
			    unsigned caseid,
			    long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_result (long testrunid, int result, char *desc,
			   long starttime, long endtime);
/* ------------------------------------------------------------------------- */
LOCAL void pl_msg_print (long testrunid, char *message);
/* ------------------------------------------------------------------------- */
LOCAL void pl_error_report (char *error);
/* ------------------------------------------------------------------------- */
LOCAL int  all_done();
/* ------------------------------------------------------------------------- */
LOCAL char *patch_path (char *p);
/* ------------------------------------------------------------------------- */
LOCAL void test_module_info(char *libname);
/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/** Test module */
typedef struct {
        unsigned moduleid_;
        Text *modulename_;
} CLIModuleData;
/** Test case */
typedef struct {
        unsigned moduleid_;
        unsigned caseid_;
        Text *casetitle_;
        Text *casedesc_;
	CLIModuleData *module_;
} CLICaseData;
/** Executed test case */
struct ExecutedTestCase {
	long runid_;
#define TCASE_STATUS_INVALID   0
#define TCASE_STATUS_ONGOING   1
#define TCASE_STATUS_FINNISHED 2
        unsigned status_;
        int result_;
        Text *resultdesc_;
        long starttime_;
        long endtime_;
        CLICaseData *case_;
};


/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Search a case by id from the case_list_, used with dl_list_find()
 *  @param a pointer to DLListIterator
 *  @param b pointer to search key
 *  @return 0 if found, greater or less than zero if not
 */ 
LOCAL int _find_case_by_id (const void *a, const void *b)
{
        CLICaseData * tmp1 = (CLICaseData*)a;
        unsigned * tmp2 = (unsigned*)b;

        if (tmp1->caseid_==(*tmp2)) return 0;
        else return -1;
}
/* ------------------------------------------------------------------------- */
/** Search a module by id from the available_modules, used with dl_list_find()
 *  @param a pointer to DLListIterator
 *  @param b pointer to search key
 *  @return 0 if found, greater or less than zero if not
 */ 
LOCAL int _find_mod_by_id (const void *a, const void *b)
{
        CLIModuleData * tmp1 = (CLIModuleData*)a;
        unsigned * tmp2 = (unsigned*)b;

        if (tmp1->moduleid_==(*tmp2)) return 0;
        else return -1;
}
/* ------------------------------------------------------------------------- */
/** Get the ExecutedTestCase struture by test run identifier 
 *  @param testrunid search key
 *  @return pointer to ExecutedTestCase matching the id or INITPTR
 */ 
LOCAL struct ExecutedTestCase *get_executed_tcase_with_runid (long testrunid)
{
	DLListIterator it;
	struct ExecutedTestCase *etc;

	for (it = dl_list_head (executed_case_list_);
	     it != INITPTR;
	     it = dl_list_next (it)) {
		etc = (struct ExecutedTestCase *)dl_list_data (it);
		if (etc->runid_ == testrunid)
			return etc;
	}
	
	return INITPTR;
}
/* ------------------------------------------------------------------------- */
/** Engine calls this for each module it is configured with
 *  @param modulename name of the module
 *  @param moduleid module id
 */ 
LOCAL void pl_new_module (char *modulename, unsigned moduleid)
{
	CLIModuleData *cld = INITPTR;
	DLListIterator it;

	if (available_modules == INITPTR) available_modules = dl_list_create();
	
	
	it = dl_list_find (dl_list_head (available_modules),
			   dl_list_tail (available_modules),
			   _find_mod_by_id,
			   (void *)&moduleid);
	if (it != DLListNULLIterator) {
	  /* user may have added a module from commandline that already 
	     exits in min.conf */
	  return;
	}

	/* add new module to list */
        cld = NEW(CLIModuleData);
        cld->moduleid_ = moduleid;
        cld->modulename_ = tx_create(modulename);
        dl_list_add (available_modules, (void*)cld);

	return;
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when module adding fails
 *  @param modulename name of the module
 */ 
LOCAL void pl_no_module (char *modulename)
{

        fprintf (stderr, "Module %s has not been loaded", modulename);        
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when all the cases for module are reported
 *  @param moduleid module id
 */ 
LOCAL void pl_module_ready (unsigned moduleid)
{
	DLListIterator it;
	CLICaseData *c;
	CLIModuleData *m;

	if (cliopts.display_info_) {
		it = dl_list_find (dl_list_head (available_modules),
				   dl_list_tail (available_modules),
				   _find_mod_by_id,
				   (void *)&moduleid);
		if (it == DLListNULLIterator) {
			fprintf (stderr, "No module with id %d",
				 moduleid);
			return;
		}
		m = dl_list_data (it);
		test_module_info (tx_share_buf (m->modulename_));
	}
	for (it = dl_list_head (case_list_); it != INITPTR;
	     it = dl_list_next (it)) {
		c  = (CLICaseData*)dl_list_data(it);
		if (c->moduleid_ == moduleid) {
			if (cliopts.display_info_) {
				printf ("%s\n", 
					tx_share_buf (c->casetitle_));
				if (c->casedesc_ != INITPTR)
					printf ("--%s\n", 
						tx_share_buf(c->casedesc_));
			} else if (cliopts.debug_ &&
				 min_clbk_.debug_case) {
				min_clbk_.debug_case 
					(c->moduleid_,
					 c->caseid_);
			} else if (min_clbk_.start_case) 
				min_clbk_.start_case 
					(c->moduleid_,
					 c->caseid_,
					 !cliopts.parallel_mode_);
		}
	}	

        ready_module_count_ ++;
	
}
/* ------------------------------------------------------------------------- */
/** Engine calls this for each new test case
 *  @param moduleid id of the module this test case belongs to
 *  @param caseid id of the test case
 *  @param casetitle test case title
 */ 
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle)
{
        CLICaseData *ccd = INITPTR;        
	DLListIterator it;

        /* add new case to some list */
        if (case_list_==INITPTR) case_list_ = dl_list_create();

	it = dl_list_find (dl_list_head (available_modules),
			   dl_list_tail (available_modules),
			   _find_mod_by_id,
			   (void *)&moduleid);
	if (it == DLListNULLIterator) {
		fprintf (stderr, "No module with id %d found for case %s\n",
			 moduleid, casetitle);
		return;
	}
	
	available_case_count_++;

        ccd = NEW(CLICaseData);
        ccd->moduleid_ = moduleid;
        ccd->caseid_  = caseid;
        ccd->casetitle_ = tx_create(casetitle);
	ccd->casedesc_ = INITPTR;
	ccd->module_ = dl_list_data (it);
	
        dl_list_add (case_list_,(void*)ccd);
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when description for test case is available
 *  @param moduleid id of the module this test case belongs to
 *  @param caseid id of the test case
 *  @param description test case description
 */ 
LOCAL void pl_case_desc (unsigned moduleid, unsigned caseid, char *description)
{
        CLICaseData *ccd = INITPTR;        
	DLListIterator it, begin;

	
	begin = dl_list_head (case_list_);
        do {
                it = dl_list_find (begin,
				   dl_list_tail (case_list_),
				   _find_case_by_id,
				   (void *)&caseid);
                if (it==DLListNULLIterator) break;
                ccd = (CLICaseData*)dl_list_data(it);
                if (ccd == INITPTR) break;
                if (ccd->moduleid_!=moduleid) begin = dl_list_next(it);
                else break;
        } while (it != DLListNULLIterator);

        if (it == DLListNULLIterator) {
		fprintf (stderr, "pl_case_desc(): error no case "
			 "found with id %d:%d",
			 moduleid, caseid);
		return;
        }

        ccd->casedesc_ = tx_create(description);

}
/* ------------------------------------------------------------------------- */
/** Engine calls this when test case has been started
 *  @param moduleid id of the module this test case belongs to
 *  @param caseid id of the test case
 *  @param testrunid identifier for the test run
 */ 
LOCAL void pl_case_started (unsigned moduleid,
			    unsigned caseid,
			    long testrunid)
{
        struct ExecutedTestCase *tmp = INITPTR;
        DLListIterator it = DLListNULLIterator;
        DLListIterator begin = DLListNULLIterator;
	CLICaseData *ccd;

        /* Case has been started, add it to the executed cases list and set
         *  its status to ongoing.
	 */
        if (executed_case_list_==INITPTR) {
                executed_case_list_ = dl_list_create();
        }

        /* Running test case details, also pointer to test case details should
           be added here.
        */
        tmp = NEW2(struct ExecutedTestCase,1);
        tmp->status_ = 1;
        tmp->result_ = -1;
        tmp->resultdesc_ = INITPTR;
        tmp->runid_ = testrunid;
        tmp->starttime_ = 0;
        tmp->endtime_ = 0;

        begin = dl_list_head (case_list_);
        do {
                it = dl_list_find (begin,
				   dl_list_tail (case_list_),
				   _find_case_by_id,
				   (void *)&caseid);
                if (it==DLListNULLIterator) break;
                ccd = (CLICaseData*)dl_list_data(it);
                if (ccd == INITPTR) break;
                if (ccd->moduleid_!=moduleid) begin = dl_list_next(it);
                else break;
        } while (it != DLListNULLIterator);
        if (it == DLListNULLIterator) {
                DELETE (tmp);
        }
        tmp->case_ = dl_list_data(it);

        /* add to list */
        dl_list_add (executed_case_list_, (void*)tmp);
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when it when test case has finished
 *  @param testrunid identifier for the test run
 *  @param result test case result
 *  @param desc test result description
 *  @param starttime starting timestamp
 *  @param endtime time the test case has finnished
 */ 
LOCAL void pl_case_result (long testrunid, int result, char *desc,
			   long starttime, long endtime){

	struct ExecutedTestCase *etc;
        Text             *txt = tx_create(" ");
        struct tm        *timeinfo = INITPTR;
	char              end_time [20];

	etc = get_executed_tcase_with_runid (testrunid);
	if (etc == INITPTR) {
		fprintf (stderr, "No test found with run id %ld", testrunid);
		tx_destroy (&txt);
		return;
	}
	if (etc->status_ != TCASE_STATUS_ONGOING) {
		fprintf (stderr, "Test result for a case with status "
			 "other than ogoing");
	}
	etc->status_ = TCASE_STATUS_FINNISHED;
	etc->resultdesc_ = tx_create (desc);
        etc->result_ = result;
        etc->starttime_ = starttime;
        etc->endtime_ = endtime;
	
	memset (end_time, 0x0, 20);
	timeinfo = localtime ((time_t *) & (etc->endtime_));
	strftime (end_time, 20, "%I:%M:%S",
		  timeinfo);
	tx_c_prepend (txt, end_time);
	switch (result) {
	case TP_TIMEOUTED:
		tx_c_append (txt, "Timeouted  ");
		break;
	case TP_CRASHED:
		tx_c_append (txt, "Crashed    ");
		break;
	case TP_PASSED:
		tx_c_append (txt, "Passed     ");
		break;
	case TP_FAILED:
		tx_c_append (txt, "Failed     ");
		break;
        case TP_NC:
		tx_c_append (txt, "Incomplete ");
		break;
	case TP_LEAVE:
		tx_c_append (txt, "Leave      ");
		break;
	default:
		tx_c_append (txt, "<Unknown>  ");
		break;
	}
	tx_c_append (txt, tx_share_buf (etc->case_->module_->modulename_));
	if (strchr (tx_share_buf (etc->case_->module_->modulename_), '.') ==
	    NULL)
		tx_c_append (txt, ".so");
	tx_c_append (txt, ":\"");
	tx_c_append (txt, tx_share_buf (etc->case_->casetitle_));
	tx_c_append (txt, "\" - ");
	tx_c_append (txt, desc);
	
	printf ("%s\n", tx_share_buf (txt));
	
	tx_destroy (&txt);
	case_result_count_ ++;
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when it when test case has sent print data
 *  @param testrunid identifier for the test run
 *  @param message the test case message
 */ 
LOCAL void pl_msg_print (long testrunid, char *message)
{
	struct ExecutedTestCase *etc;

	etc = get_executed_tcase_with_runid (testrunid);

	if (etc == INITPTR)
		printf ("         message from unknown test : %s\n", message);
	else
		printf ("         message from \"%s\": %s\n", 
			tx_share_buf (etc->case_->casetitle_),
			message);
	return;
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when it when test case/module sends error message
 *  @param error the message
 */ 
LOCAL void pl_error_report (char *error) {
	fprintf (stderr, "%s\n", error);
}
/* ------------------------------------------------------------------------- */
/** Function that checks if we have ran all the cases and can finnish
 */
LOCAL int all_done()
{
	if (ready_module_count_ == dl_list_size (available_modules) &&
	    (cliopts.display_info_ == 1 ||
	     available_case_count_  == case_result_count_))	    
	{
		return 1;
	}

	return 0;
}
/* ------------------------------------------------------------------------- */
/** If path given in argument is not absolute make it so.
 *  @param p the path to check
 *  @return the patched path
 */
LOCAL char *patch_path (char *p)
{
        Text *path, *cwd;
        char *retval;

        path = tx_create (p);
        if (*p != '/') { /* relative path have to prepend with pwd */
                cwd = tx_create (getenv ("PWD"));
                tx_c_append (cwd, "/");
                tx_append (cwd, path);
                retval = tx_get_buf (cwd);
                tx_destroy (&cwd);
        } else
                retval = tx_get_buf (path);
        tx_destroy (&path);

        return retval;
}
/* ------------------------------------------------------------------------- */
/** Displays information of test module library 
 *  @param libname the test module name coming from commandline
 */
LOCAL void test_module_info (char *libname)
{
        test_libl_t tlibl;
        int ret;
        static int first = 1;
        char *fullpath;

        fullpath = patch_path (libname);

        ret = tl_open (&tlibl, fullpath);

        if( (ret != ENOERR) && (tlibl.test_library_==INITPTR) ) {
                printf("Error when looking for test module %s\n",libname);
                DELETE (fullpath);
                return;
        }
        if (first) {
                printf ("Module Type:\tModule Version:\t\tBuild date:\n");
                first = 0;
        }
        printf ("%-11s\t%4d\t\t\t%s %s\n", 
                tlibl.type_, 
                tlibl.version_, 
                tlibl.date_, 
                tlibl.time_);
	printf ("Cases:\n");
        tl_close (&tlibl);
        DELETE (fullpath);
	
        return;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Plugin attach function, sets the callbacks.
 *  @param out_callback callbacks called by the engine
 *  @param in_callback callbacks towards the engine
 */
void pl_attach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        /* Binds the callbacks */
        memcpy (&min_clbk_,in_callback,sizeof(eapiOut_t));

        (*out_callback)->case_result            = pl_case_result;
        (*out_callback)->case_started           = pl_case_started;
        (*out_callback)->case_paused            = NULL;
        (*out_callback)->case_resumed           = NULL;
        (*out_callback)->module_prints          = pl_msg_print;
        (*out_callback)->new_module             = pl_new_module;
        (*out_callback)->no_module              = pl_no_module;
        (*out_callback)->module_ready           = pl_module_ready;
        (*out_callback)->new_case               = pl_new_case;
        (*out_callback)->case_desc              = pl_case_desc;
        (*out_callback)->error_report           = pl_error_report;

        return;
}
/* ------------------------------------------------------------------------- */
/** Plugin open function 
 *  @param arg not used
 */
void pl_open_plugin (void *opts)
{
	DLListIterator it;
        CLIModuleData *cld;
	CLICaseData *ccd;
	struct ExecutedTestCase *etc;

	memcpy (&cliopts, (cli_opts*)opts, sizeof (cli_opts));
        if (min_clbk_.min_open) min_clbk_.min_open ();
	while (!all_done()) {
		usleep (50000);
	}
        if (min_clbk_.min_close) min_clbk_.min_close ();
	
	for (it = dl_list_head (available_modules);
	     it != DLListNULLIterator;
	     it = dl_list_next (it)) {
		cld = (CLIModuleData *)dl_list_data (it);
		tx_destroy (&cld->modulename_);
		DELETE (cld);
	}
	dl_list_free (&available_modules);

	for (it = dl_list_head (case_list_);
	     it != DLListNULLIterator;
	     it = dl_list_next (it)) {
		ccd = (CLICaseData *)dl_list_data (it);
		tx_destroy (&ccd->casetitle_);
		tx_destroy (&ccd->casedesc_);
		DELETE (ccd);
	}
	dl_list_free (&case_list_);
	
	for (it = dl_list_head (executed_case_list_);
	     it != DLListNULLIterator;
	     it = dl_list_next (it)) {
		etc = (struct ExecutedTestCase *)dl_list_data (it);
		tx_destroy (&etc->resultdesc_);
		DELETE (etc);
	}
	dl_list_free (&executed_case_list_);
	
        return;
}
/* ------------------------------------------------------------------------- */
