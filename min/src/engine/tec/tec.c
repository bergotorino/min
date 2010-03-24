/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Robert Galkowski
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
 *  @file       tec.c
 *  @version    0.1
 *  @brief      This file contains implementation of test execution 
 *              controller functionality
 */

/* ----------------------------------------------------------------------------
 * INCLUDE FILES
 */
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <regex.h>

#include <min_parser.h>
#include <tec.h>

#include <min_common.h>
#include <consoleui.h>
#include <data_api.h>
#include <tec_events.h>
#include <min_logger.h>
#include <min_engine_api.h>
#include <min_settings.h>
#ifndef MIN_EXTIF
#include <tec_tcp_handling.h>
#endif
/* ----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 */
pthread_mutex_t tec_mutex_ = PTHREAD_MUTEX_INITIALIZER;
char          **envp;
#ifdef MIN_UNIT_TEST
int             unit_test_result;
#endif                          /*MINMIN_UNIT_TEST */
eapiIn_t in_str;                          /** Holder for engine api in
					   *  direction */
eapiIn_t *in = &in_str;                   
DLList   *available_modules = INITPTR;   /** list of available test modules.  */
DLList   *filters = INITPTR;             /** list of test case title filters. */
int       min_return_value = 0;          /** return value for the whole prog. */
/** Mode of operation. 
 *  0 - execute all configured test cases and exit. 
 *  1 - exit when user so requests.
 */     
int             operation_mode_ = 0; 
/** Shared memory segment identifier */
int             sh_mem_id_ = -1;      
EngineDefaults  *engine_defaults;
SettingsSection *engine_section, *logger_section;
struct logger_settings_t *logger_settings; /** Logger setting for engine */

/* ----------------------------------------------------------------------------
 * EXTERNAL DATA STRUCTURES
 */
/* None */
/* ---------------------------------------------------------------------------
 * EXTERNAL FUNCTION PROTOTYPES
 */
/* ---------------------------------------------------------------------------*/
extern int      ec_msg_ms_handler (MsgBuffer * message);
/* ---------------------------------------------------------------------------*/
extern int      ec_msg_sndrcv_handler (MsgBuffer * message);
/* ---------------------------------------------------------------------------*/
extern int      event_system_up (void);
/* ---------------------------------------------------------------------------*/
extern int      event_system_cleanup (void);
/* ----------------------------------------------------------------------------
 * CONSTANTS
 */
/* None */
/* ----------------------------------------------------------------------------
 * LOCAL CONSTANTS AND MACROS
 */
#ifndef MIN_CONF_DIR
# define MIN_CONF_DIR "/etc" /** Default path for min.conf */
#endif

#ifndef MIN_BIN_DIR
# define MIN_BIN_DIR "/usr/bin" /** Default path for tmc executable */
#endif
/* ----------------------------------------------------------------------------
 * MODULE DATA STRUCTURES
 */
/** Test case title filter */
typedef struct {
	Text *filter_;
	int regexp_;
} title_filter;

/* ----------------------------------------------------------------------------
 * LOCAL FUNCTION PROTOTYPES
 */
/* ------------------------------------------------------------------------- */
LOCAL void      wait_for_pid (void *pid);
/* ------------------------------------------------------------------------- */
LOCAL void      handle_sigsegv (int signum);
/* ------------------------------------------------------------------------- */
LOCAL void      handle_sigquit (int signum);
/* ------------------------------------------------------------------------- */
LOCAL void      handle_sigbus (int signum);
/* ------------------------------------------------------------------------- */
LOCAL void      handle_sigterm (int signum);
/* ------------------------------------------------------------------------- */
LOCAL void      handle_sigint (int signum);
/* ------------------------------------------------------------------------- */
LOCAL pid_t     ec_start_tmc (DLListIterator work_module_item);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_read_module_section (MinParser * inifile);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_read_module_confdir (int op_mode);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_read_conf (MinParser * inifile, int operation_mode);
/* ------------------------------------------------------------------------- */
LOCAL void      create_local_confdir ();
/* ------------------------------------------------------------------------- */
LOCAL int       ec_filter_it(char *title);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_start_module_temp (DLListIterator work_case_item);
/* ------------------------------------------------------------------------- */
LOCAL void      ec_check_next_in_group (int group_id);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_remove_module_temp (DLListIterator work_module_item);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_exec_case_temp (DLListIterator work_module_item);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_handle_temp_results (DLListIterator temp_module_item,
					MsgBuffer * message);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_msg_ok_handler (MsgBuffer * message);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_msg_ko_handler (MsgBuffer * message);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_msg_ret_handler (MsgBuffer * message);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_msg_tcd_handler (MsgBuffer * message);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_msg_event_handler (MsgBuffer * message);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_msg_usr_handler (MsgBuffer * message);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_msg_run_id_handler (MsgBuffer * message);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_msg_tcdesc_handler (MsgBuffer * message);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_message_dispatch (MsgBuffer * rcvd_message);
/* ------------------------------------------------------------------------- */
LOCAL void     *ec_message_listener (void *arg);
/* ------------------------------------------------------------------------- */
LOCAL int       ec_init_module_data (DLListItem * work_module_item);
/* ------------------------------------------------------------------------- */

#ifndef MIN_EXTIF
LOCAL int      ec_read_slaves_section (MinParser * inifile);
#endif

/* -------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Waits for process to end - collects the zombie.
 *  @param pid of the process
 */
LOCAL void wait_for_pid (void *arg)
{
        int* pid = (int*)arg;
        waitpid ( *pid, NULL, 0); 

        pthread_exit (0);
}

/* ------------------------------------------------------------------------- */

/** Select test case for execution - passed test case is 
 * copied, and then copied case is added to  "selected" list. As this function 
 * modifies "selected" list, code that calls it should first lock "tec_mutex_",
 * and unlock it after executing the function.
 * @param work_case_item DLList item containing selected case
 * @param group_id id of group to which case belongs
 * @return item from "selected cases" list
 */
DLListIterator ec_select_case (DLListIterator work_case_item, int group_id)
{


        test_case_s    *dest_case = INITPTR;
        test_result_s  *test_result = INITPTR;
        DLListIterator  selected_case = DLListNULLIterator;
        filename_t      config;
        title_string_t  title;
        DLListIterator  work_result_item = DLListNULLIterator;

        tc_get_cfg_filename (work_case_item, config);
        tc_get_title (work_case_item, title);

        dest_case =
            tc_create (tc_get_test_module_ptr (work_case_item), config,
                       title);

        selected_case = tc_add (selected_cases, dest_case);

        tc_set_group_id (selected_case, group_id);
        tc_set_id (selected_case, tc_get_id (work_case_item));
        tc_set_priority (work_case_item, tc_get_priority (work_case_item));
	dest_case->tc_ext_id_ = tc_get_ext_id (work_case_item);

        test_result = tr_create_data_item (selected_case);
        work_result_item =
            tr_add (tc_get_tr_list (selected_case), test_result);
	if (work_result_item == INITPTR)
		MIN_WARN ("Test result list adding failure!");

        return selected_case;
}
/*---------------------------------------------------------------------------*/
/** Create shared memory segment and write settings to it, 
 * so that they can be used by other processes (TMCs). Currently sent settings
 *  are: logger's settings placed in apropriate structure, and search paths 
 * (if there are any search paths defined), concatenated into one 
 *  NULL-terminated string, separated by ":" character. If program fails to 
 * createshared memory segment, it will exit the application.
 */
LOCAL void ec_settings_send ()
{

        int             shared_segm_id = 0;
        void           *sh_mem_handle = NULL;
        int             paths_size = 0, ret;
        char           *work_path = INITPTR;
        DLListIterator  work_path_item = DLListNULLIterator;
        char           *concat_paths = INITPTR;
        void           *temp_ptr;
        work_path_item = dl_list_head (engine_defaults->search_dirs);

        while (work_path_item != DLListNULLIterator) {

                work_path = (char *)dl_list_data (work_path_item);
                paths_size = paths_size + strlen (work_path) + 1;
                work_path_item = dl_list_next (work_path_item);

        }

        if (paths_size > 0) {
                concat_paths = NEW2 (char, paths_size);
                *concat_paths = '\0';
                work_path_item = dl_list_head (engine_defaults->search_dirs);
                while (work_path_item != DLListNULLIterator) {
                        work_path = (char *)dl_list_data (work_path_item);
                        strcat (concat_paths, work_path);
                        work_path_item = dl_list_next (work_path_item);
                        if (work_path_item != DLListNULLIterator)
                                strcat (concat_paths, ":");
                }
        }
        /*create shared memory segment */
	shared_segm_id =
		sm_create ('a', sizeof (struct logger_settings_t) + paths_size);
	/*
	** Try to clean up
	*/

        if (shared_segm_id < 0) {
		MIN_WARN ("failed to create shared memory segment "
			  " - trying to clean up");
		ret = system ("/usr/bin/min-clean.sh");
		if (ret < 0) 
			MIN_WARN ("failed to run clean up script");
	  
	}


        shared_segm_id =
            sm_create ('a', sizeof (struct logger_settings_t) + paths_size);
        if (shared_segm_id < 0) {
                MIN_FATAL ("Failed to create shared memory segment: %s",
                            strerror (errno));
                min_log_close ();
#ifdef MIN_UNIT_TEST
		exit (0);
#endif
                exit (-1);
        }

        sh_mem_id_ = shared_segm_id;
        sh_mem_handle = sm_attach (shared_segm_id);
        sm_write (sh_mem_handle,
                  (void *)(logger_settings),
                  sizeof (struct logger_settings_t));
        temp_ptr = sh_mem_handle + sizeof (struct logger_settings_t);
        if (paths_size > 0) {
                sm_write (temp_ptr, (void *)concat_paths, paths_size);
                DELETE (concat_paths);
        }

        sm_detach (sh_mem_handle);
}

/* ------------------------------------------------------------------------- */
/** Check test case title agains filters.
 * @param test case title string
 * @return 0 if the test case matches filter, 1 if not
 */
LOCAL int 
ec_filter_it(char *title)
{
	DLListIterator it;
	title_filter *filt;
	regex_t   re;
	int val;

	if (filters == INITPTR)
		return 0;

	/*
	** We have filters specified, see if the case should be filtered
	*/
	for (it = dl_list_head(filters);
	     it != DLListNULLIterator;
	     it = dl_list_next(it)) {
		filt = dl_list_data (it);
		if (filt->regexp_ == 0 && !strcmp (title, 
						   tx_share_buf 
						   (filt->filter_)))
			return 0;
		if (filt->regexp_) {
			val = regcomp (&re, tx_share_buf (filt->filter_), 
				       REG_EXTENDED);
			if (val) {
				regfree (&re);
				continue; /* regcomp failed */
			}
			val = regexec (&re, title, 0, NULL, 0);
			regfree (&re);
			if (val == 0)
				return 0; /* Match */
		}
	}

	return 1;
}
/*---------------------------------------------------------------------------*/
/** Used for temporary module instantiation and test case execution, 
 *  used when there is a need to execute two test cases from the same module 
 *  in parallel.
 * @param work_case_item DLListItem containing test case to be executed
 * @return 0 on success -1 on error 
 */
LOCAL int ec_start_module_temp (DLListIterator work_case_item)
{
	/* this will be the module that will be used as a source of data for 
	 * instantiating temporary module */
        DLListIterator  source_module_item = DLListNULLIterator;        
	/* this will contain handle to temporaryily instantiated module */
        DLListIterator  temporary_module_item = DLListNULLIterator;    
        /* we will need also the copy of case to be executed */
        DLList         *work_cases_list = dl_list_create ();
        DLList         *cfgs_work_list = dl_list_create ();
        pid_t           temp_mod_pid = 0;
        filename_t      library_name;
        test_module_info_s *temporary_module = INITPTR;

        source_module_item = tc_get_test_module_ptr (work_case_item);

        tm_get_module_filename (source_module_item, library_name);

        temporary_module =
            tm_create (library_name,
                       tm_get_cfg_filenames (source_module_item),
                       tm_get_module_id (source_module_item));
        temporary_module_item =
            tm_add (instantiated_modules, temporary_module);
        tm_set_cfg_filenames (temporary_module_item, cfgs_work_list);
        temp_mod_pid = ec_start_tmc (temporary_module_item);
        if (temp_mod_pid == -1) {
                MIN_WARN ("Failed to create module");
		dl_list_free (&work_cases_list);
		dl_list_free (&cfgs_work_list);
                return -1;

        }
        tm_set_status (temporary_module_item, TEST_MODULE_INITIALIZED_TEMP);
        tm_set_tclist (temporary_module_item, work_cases_list);
        tm_set_pid (temporary_module_item, temp_mod_pid);

        tc_add (work_cases_list, dl_list_data (work_case_item));
        tc_set_test_module_ptr (work_case_item, temporary_module_item);

        return 0;
}
/*----------------------------------------------------------------------------*/
/** Used to check if there is a test case with given group id 
 *  in "selected_cases" list waiting to be executed and runs it if it is the 
 *  case.
 * @param group_id - id og group of test cases in question.
 */
LOCAL void ec_check_next_in_group (int group_id)
{

        DLListIterator  work_case_item = DLListNULLIterator;

        pthread_mutex_lock (&tec_mutex_);

        work_case_item = dl_list_head (selected_cases);

        while (work_case_item != DLListNULLIterator) {

                if ((tc_get_group_id (work_case_item) == group_id) &&
                    (tc_get_status (work_case_item) == TEST_CASE_IDLE))
                        break;
                
                work_case_item = dl_list_next (work_case_item);
        }
        
        pthread_mutex_unlock (&tec_mutex_);
        
        if (work_case_item != DLListNULLIterator) {
                ec_exec_case (work_case_item);
        }

        return;
}
/*---------------------------------------------------------------------------*/
/**Remove temporary instantiated module.
 * @param work_module_item pointer to list item containing module in question.
 * @return 0 always
 */
LOCAL int ec_remove_module_temp (DLListIterator work_module_item)
{

        pthread_t       threadid;
        long            address = tm_get_pid (work_module_item);
        test_module_info_s *work_module =
            (test_module_info_s *) dl_list_data (work_module_item);

        mq_send_message2 (mq_id, address, MSG_END, 0, "Shut down !");
#ifndef MIN_UNIT_TEST
        pthread_create (&threadid, NULL, (void *)&wait_for_pid,
                        (void *)&address);
#endif
        tm_delete (work_module);
        tm_remove (work_module_item);

        return 0;
}
/*---------------------------------------------------------------------------*/
/** Start test case from temporarily instantianted module -
 *  process is slightly different than starting test case from ordinary module.
 * "Temp" module has only one test case in list, so only that has to be passed
 *  to this function is module item.
 * @param work_module_item pointer to dllistitem containg "current" module.
 * @return 0 on success, -1 on error
 */
LOCAL int ec_exec_case_temp (DLListIterator work_module_item)
{
        MsgBuffer       message;
        DLListIterator  work_result_item;
        DLListIterator  work_case_item =
                dl_list_head (tm_get_tclist (work_module_item));
        test_case_s    *work_case = dl_list_data (work_case_item);

        /*let's look for test case in "selected list" */
        pthread_mutex_lock (&tec_mutex_);

        work_case_item = dl_list_head (selected_cases);

        while (work_case_item != DLListNULLIterator) {

                if (dl_list_data (work_case_item) == work_case)
                        break;
                work_case_item = dl_list_next (work_case_item);

        }

        pthread_mutex_unlock (&tec_mutex_);

        if (work_case_item == DLListNULLIterator) {
                MIN_WARN ("Test case not found in selected list");
                return -1;
        }

        tm_set_status (work_module_item, TEST_MODULE_BUSY_TEMP);
        tc_set_status (work_case_item, TEST_CASE_ONGOING);
        work_result_item = dl_list_head (tc_get_tr_list (work_case_item));
        tr_set_start_time (work_result_item, time (NULL));
        tr_set_priontouts_list (work_result_item, dl_list_create ());

        /*create and send message */
	if (work_case->debug_case_)
		message.type_ = MSG_EXE_DLD;
	else
		message.type_ = MSG_EXE;
        message.sender_ = engine_pid;
        message.receiver_ = tm_get_pid (work_module_item);
        message.param_ = tc_get_id (work_case_item);
        message.desc_[0] =  '\0';
        tc_get_cfg_filename (work_case_item, message.message_);
        mq_send_message (mq_id, &message);
	
        return 0;
}
/*---------------------------------------------------------------------------*/
/** Handles test results obtained from "temp" module
 * @param temp_module_item DLListIterator containing module to be worked on
 * @param message pointer to received message buffer
 * @return 0 on success, -1 on error
 */
LOCAL int ec_handle_temp_results (DLListIterator temp_module_item,
                                  MsgBuffer * message)
{
        /* This module has only one test case */
        DLListIterator  work_case_item =
            dl_list_head (tm_get_tclist (temp_module_item));
        int             group_id = tc_get_group_id (work_case_item);
        DLListIterator  orig_case;
        DLListIterator  work_module_item = DLListNULLIterator;
        DLListIterator  temp_result;
        DLListIterator  work_result_item = DLListNULLIterator;
        test_result_s  *work_result = INITPTR;
        filename_t      name;
        filename_t      name2;
        filename_t      work_conf;
        filename_t      orig_conf;
        int             id, id2;
        int             it = 1; /*general purpose loop iterator */

        int             cont_flag = 1;

        work_module_item = dl_list_head (instantiated_modules);
        tm_get_module_filename (temp_module_item, name);

        /* first we will search for module that was the source for "temp", 
	 * using filename */
        while (work_module_item != DLListNULLIterator) {
                tm_get_module_filename (work_module_item, name2);
                it = strcmp (name, name2);

                if (it == 0) {
                        tc_get_cfg_filename (work_case_item, work_conf);
                        orig_case =
                            dl_list_head (tm_get_tclist (work_module_item));
                        if (orig_case == DLListNULLIterator)
                                break;
                        while (orig_case != DLListNULLIterator) {
                                tc_get_cfg_filename (orig_case, orig_conf);
                                if (strstr (work_conf, orig_conf) != NULL) {
                                        cont_flag = 0;
                                        break;
                                } else
                                        orig_case = dl_list_next (orig_case);
                        }
                }

                if (cont_flag == 1)
                        work_module_item = dl_list_next (work_module_item);
                else
                        break;
        }

        if (work_module_item == DLListNULLIterator) {
                MIN_WARN ("MODULE Data fault");
                return -1;
        }

        /*now we will search for original test case, using id and config
	  filename */
        id = tc_get_id (work_case_item);
        tc_get_cfg_filename (work_case_item, work_conf);
        orig_case = dl_list_head (tm_get_tclist (work_module_item));

        while (orig_case != DLListNULLIterator) {
                tc_get_cfg_filename (orig_case, orig_conf);
                id2 = tc_get_id (orig_case);
                if ((id == id2) && (strcmp (work_conf, orig_conf) == 0))
                        break;
                else
                        orig_case = dl_list_next (orig_case);
        }

        if (orig_case == DLListNULLIterator) {
                MIN_WARN ("CASE Data fault");
                return -1;
        }

        work_result = tr_create_data_item (orig_case);
        work_result_item = tr_add (tc_get_tr_list (orig_case), work_result);
        /* since module was started to run just one case once, it will have
	 * only one test result, from which we will now copy data to 
	 * original test case 
	 */
        temp_result = dl_list_head (tc_get_tr_list (work_case_item));   
        tr_set_result_description (work_result_item, message->message_);
        tr_set_start_time (work_result_item, tr_get_start_time (temp_result));
        tr_set_result_description (work_result_item, message->message_);
        tr_set_end_time (work_result_item, time (NULL));

        switch (message->param_) {
        case TP_CRASHED:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_CRASHED);
                break;

        case TP_TIMEOUTED:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_TIMEOUT);
                break;

        case TP_PASSED:
                tr_set_result_type (work_result_item, TEST_RESULT_PASSED);
                break;

        case TP_FAILED:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_FAILED);
                break;

        case TP_NC:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_NOT_RUN);
                break;

        case TP_LEAVE:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_FAILED);
                break;

        default:

                MIN_WARN ("Faulty data received");
                return (-1);

        }

        tr_set_priontouts_list (work_result_item,
                                tr_get_priontouts_list (temp_result));
        ((test_result_s *) dl_list_data (temp_result))->printouts_list_ =
            INITPTR;


	MINAPI_PLUGIN_CALL (case_result,
			    case_result (tc_get_run_id (work_case_item), 
					 message->param_, 
					 message->message_,
					 tr_get_start_time (work_result_item),
					 tr_get_end_time (work_result_item)));

        pthread_mutex_lock (&tec_mutex_);
	
        work_case_item = dl_list_head (selected_cases);
        while (work_case_item != DLListNULLIterator) {
                if (tc_get_test_module_ptr (work_case_item) ==
                    temp_module_item)
                        break;
                work_case_item = dl_list_next (work_case_item);
        }
        pthread_mutex_unlock (&tec_mutex_);
	
        if (work_case_item == DLListNULLIterator) {
                MIN_WARN ("could not find case in list");
                return -1;
        }
        work_module_item =
		tm_get_ptr_by_pid (instantiated_modules, message->sender_);
        tc_set_status (work_case_item, TEST_CASE_TERMINATED);

        pthread_mutex_lock (&tec_mutex_);
        pthread_mutex_unlock (&tec_mutex_);
	
        ec_remove_module_temp (work_module_item);
        ec_check_next_in_group (group_id);

        return 0;
}
/*---------------------------------------------------------------------------*/
/** Handler for MSG_OK. As it is first message sent by module, 
 *  function set's module status to TC_SENDING, initilaizes test cases list
 *  for module and sends message MSG_GTC
 *  @param message pointer to MsgBuffer structure containing message
 *  @return result of message handling, 0 if message handling was succesful, 
 *          -1 otherwise
 */
LOCAL int ec_msg_ok_handler (MsgBuffer * message)
{
        int             result = -1;
        long            sender = message->sender_;
        int             work_module_status;
	int             retry_count;
        DLListIterator  work_module =
            tm_get_ptr_by_pid (instantiated_modules, sender);
        MsgBuffer       message_gtc;

        /*Possible error handling will be inserted here */
	retry_count = 0;
        while (work_module == DLListNULLIterator) {

                MIN_WARN ("Could not fetch module data, pid = %d", sender);
		/*
		** Synchronization issue try again a few times
		*/
		usleep (100000);
		work_module =
			tm_get_ptr_by_pid (instantiated_modules, sender);
		retry_count++;
		if (retry_count > 10)
			return result;
        }

        work_module_status = tm_get_status (work_module);

        switch (work_module_status) {
        case TEST_MODULE_INITIALIZED:
                tm_set_status (work_module, TEST_MODULE_TC_SENDING);
                result = 0;
                message_gtc.type_ = MSG_GTC;
                message_gtc.sender_ = engine_pid;
                message_gtc.receiver_ = message->sender_;
                message_gtc.param_ = 0;
                message_gtc.special_ = ESTrue;  /*TCDs should be sent */
                mq_send_message (mq_id, &message_gtc);
                break;

        case TEST_MODULE_INITIALIZED_TEMP:

                result = 0;
                message_gtc.type_ = MSG_GTC;
                message_gtc.sender_ = engine_pid;
                message_gtc.receiver_ = message->sender_;
                message_gtc.param_ = 0;
                message_gtc.special_ = ESFalse; /*don't care about 
                                                   TCDs */
                mq_send_message (mq_id, &message_gtc);
                break;

        default:

                MIN_WARN ("Unexpected message sequence");
                result = -1;
                break;

        }

#ifdef MIN_UNIT_TEST
        unit_test_result = result;
#endif                          /*MIN_UNIT_TEST */

        return result;
}
/*---------------------------------------------------------------------------*/
/** Handler for MSG_KO message, which means that some trouble occured on
 *  tmc's side. Hadling depends on state of module: if it's 
 *  TEST_MODULE_INITIALIZED, it means that test case extraction is impossible.
 *  Module will be removed from "instantiated" list and terminated.Problems 
 *  will be logged.
 *  @param message - pointer to MsgBuffer containing message
 *  @return 0 on success, -1 on error.
 */
LOCAL int ec_msg_ko_handler (MsgBuffer * message)
{
        int             result = -1;
        int             status = 0;
        DLListIterator  work_module_item = DLListNULLIterator;
        /*work module fetched from list of instantiated modules */

        work_module_item =
            tm_get_ptr_by_pid (instantiated_modules, message->sender_);

        if (work_module_item == DLListNULLIterator) {

                MIN_WARN ("Could not fetch module data");
                return result;
        }

        status = tm_get_status (work_module_item);
        switch (status) {
        case TEST_MODULE_INITIALIZED:
                mq_send_message2 (mq_id, message->sender_, MSG_END, 0,
                                  "Useless module");
                waitpid (message->sender_, NULL, 0);
                dl_list_remove_it (work_module_item);
                MIN_WARN ("Could not fetch test cases");
                result = 0;
                break;

        default:

                result = -1;
                MIN_WARN ("Unexpected message sequence");
                break;

        }

        return result;
}
/*---------------------------------------------------------------------------*/
/** Handler for MSG_RET with test case results
 *  @param message - pointer to MsgBuffer containing message
 *  @return 0 on success, -1 on error.
 */
LOCAL int ec_msg_ret_handler (MsgBuffer * message)
{

        DLListIterator  work_case_item = INITPTR;
        DLListIterator  dest_case_item = INITPTR;
        DLListIterator  work_module_item = INITPTR;
        DLList         *work_result_list = INITPTR;
        DLListIterator  work_result_item = INITPTR;
        int             group_id;
        filename_t      orig_conf;
        filename_t      work_conf;
        test_result_s  *result;
	int		fun_result = 0, res_code;

        const char     *test_result_type_str[] = {
                "not run",
                "passed",
                "failed",
                "crashed",
                "aborted",
                "timeout",
                "all"
        };

        work_module_item =
            tm_get_ptr_by_pid (instantiated_modules, message->sender_);
        if (work_module_item == DLListNULLIterator) {

                MIN_WARN ("Failed to fetch module data");
		fun_result = -1;
		goto EXIT;
        }
        /* if module was instantiated as temporary, handling is different 
	 * since it was created only to run one test case
	 */
        if (tm_get_status (work_module_item) == TEST_MODULE_BUSY_TEMP) {
                ec_handle_temp_results (work_module_item, message);
		fun_result = 1;
		goto EXIT;
        }
        /* in current design, there is only one test case with status : ongoing
	 * belonging to module with given PID, so we search "selected cases" 
	 * list for test case based on that information
	 */
        pthread_mutex_lock (&tec_mutex_);


        work_case_item = dl_list_head (selected_cases);
        while (work_case_item != DLListNULLIterator) {
                if ((tc_get_status (work_case_item) == TEST_CASE_ONGOING ||
		     tc_get_status (work_case_item) == TEST_CASE_PAUSED) &&
		    (tm_get_pid (tc_get_test_module_ptr (work_case_item)) ==
		     message->sender_))
                        /* we found ongoing case with module matching message 
			 * sender's pid, co we can stop
			 * iterating through list 
			 */
                        break;
                work_case_item = dl_list_next (work_case_item);

        }

        pthread_mutex_unlock (&tec_mutex_);

        if (work_case_item == DLListNULLIterator) {
		fun_result = -1;
		goto EXIT;
	}


        work_result_list = tc_get_tr_list (work_case_item);
        if (work_result_list == INITPTR) {
                MIN_WARN ("Fault in test data");
		fun_result = -1;
		goto EXIT;
        }

        work_result_item = dl_list_head (work_result_list);
        tr_set_result_description (work_result_item, message->message_);
        tr_set_end_time (work_result_item, time (NULL));

        switch (message->param_) {

        case TP_CRASHED:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_CRASHED);
                break;

        case TP_TIMEOUTED:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_TIMEOUT);
                break;

        case TP_PASSED:

                tr_set_result_type (work_result_item, TEST_RESULT_PASSED);
                break;

        case TP_FAILED:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_FAILED);
                break;

        case TP_NC:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_ABORTED);
                break;

        case TP_LEAVE:
		min_return_value ++;
                tr_set_result_type (work_result_item, TEST_RESULT_FAILED);
                break;

        default:

                MIN_WARN ("Faulty data received");
		fun_result = -1;
		goto EXIT;
        }
	res_code = tr_get_result_type(work_result_item);
        MIN_DEBUG ("RESULT CODE = %s (%d)",
		   (res_code >= 0) ?
		   test_result_type_str [res_code] : "UNDEFINED",
		   res_code);
	
        tm_set_status (work_module_item, TEST_MODULE_READY);
        tc_set_status (work_case_item, TEST_CASE_TERMINATED);
        group_id = tc_get_group_id (work_case_item);

	MINAPI_PLUGIN_CALL (case_result,
			    case_result (tc_get_run_id (work_case_item), 
					 message->param_, 
					 message->message_,
					 tr_get_start_time
					 (work_result_item),
					 tr_get_end_time
					 (work_result_item)));
	
        /* Now let's link created result item to original test case. 
	 * We will use module link, test case id and test case filename 
	 */
        dest_case_item = dl_list_head (tm_get_tclist (work_module_item));
        tc_get_cfg_filename (work_case_item, work_conf);


        while (dest_case_item != DLListNULLIterator) {
                tc_get_cfg_filename (dest_case_item, orig_conf);
                if ((tc_get_id (dest_case_item) == tc_get_id (work_case_item))
                    && (strcmp (work_conf, orig_conf) == 0)) {
                        break;
                }
                dest_case_item = dl_list_next (dest_case_item);
        }
        result = NEW (test_result_s);
        memcpy (result,
                dl_list_data (work_result_item), sizeof (test_result_s));
        ((test_result_s *) dl_list_data (work_result_item))->printouts_list_ =
            INITPTR;
        work_result_item = tr_add (tc_get_tr_list (dest_case_item), result);

        ((test_result_s *) dl_list_data (work_result_item))->tc_data_item_ =
            dest_case_item;
        /*now add result to global "results" list */
        /*dl_list_add(dl_list_data(work_result_item),results); */

        MIN_DEBUG ("orig_case has %d results now",
                    dl_list_size (tc_get_tr_list (dest_case_item)));
        work_result_item = dl_list_head (tc_get_tr_list (dest_case_item));
        while (work_result_item != DLListNULLIterator) {
		res_code = tr_get_result_type(work_result_item);
                MIN_DEBUG ("RESULT CODE = %s (%d)",
			   (res_code >= 0) ?
			   test_result_type_str [res_code] : 
			   "UNDEFINED",
			   res_code);

                work_result_item = dl_list_next (work_result_item);
        }


        if (group_id != 0)
                ec_check_next_in_group (group_id);
EXIT:
        return fun_result;
}
/* ------------------------------------------------------------------------- */
/** Handler for MSG_TCD message. Creates new test_case_s structure and
 *  inserts it into appropriate module's case list.
 * @param message - pointer to MsgBuffer containing message
 * @return result of message handling, 0 if message was handled, -1 otherwise
 */
LOCAL int ec_msg_tcd_handler (MsgBuffer * message)
{
        int             result = -1;
        int             work_module_status = 0;
        int             address = 0;
        DLListIterator  work_module_item = DLListNULLIterator;
        DLListIterator  work_case_item = DLListNULLIterator;
        test_case_s    *work_case = INITPTR;
        DLList         *work_tclist = INITPTR;

        work_module_item =
            tm_get_ptr_by_pid (instantiated_modules, message->sender_);
        if (work_module_item == DLListNULLIterator)
		goto EXIT;

        work_module_status = tm_get_status (work_module_item);

        work_tclist = tm_get_tclist (work_module_item);
        if (work_tclist == INITPTR)
		goto EXIT;

        if (message->param_ == 0) {

                switch (work_module_status) {
                case TEST_MODULE_TC_SENDING:
                        MIN_DEBUG ("finished tc gathering");
                        if (dl_list_size (tm_get_tclist (work_module_item)) ==
                            0) {
                                /* something went wrong, module has 0 tcs, 
				 * and can be removed - at least in current 
				 * design 
				 */
				MINAPI_PLUGIN_CALL (module_ready,
						    module_ready
						    (tm_get_module_id 
						     (work_module_item)));
                                address = tm_get_pid (work_module_item);
                                mq_send_message2 (mq_id, address, MSG_END, 0,
                                                  "Shut down !");
                                tm_remove (work_module_item);
                                waitpid (address, NULL, 0);

				result = 0;
				goto EXIT;
                                break;
                        } else {
                                tm_set_status (work_module_item,
                                               TEST_MODULE_READY);
                                result = 0;

                                MINAPI_PLUGIN_CALL (module_ready, 
						    module_ready 
						    (tm_get_module_id 
						     (work_module_item)));
				goto EXIT;
                        }
                        break;
                case TEST_MODULE_INITIALIZED_TEMP:
                        tm_set_status (work_module_item,
                                       TEST_MODULE_BUSY_TEMP);
                        /* first case from list will be fetched and executed,
			 * since temp module has only one case in list 
			 */
                        result = ec_exec_case_temp (work_module_item);
                        goto EXIT;
                        break;
                default:
                        MIN_WARN (" UNEXPECTED MESSAGE SEQUENCE ");
                        result = -1;
			goto EXIT;
                        break;
                }
        }

        if (work_module_status == TEST_MODULE_TC_SENDING &&
	    !ec_filter_it(message->message_)) {
                work_case =
                    tc_create (work_module_item, message->desc_,
                               message->message_);
                work_case_item = tc_add (work_tclist, work_case);
                tc_set_id (work_case_item, message->param_);
		work_case->tc_ext_id_ =
			dl_list_size (work_tclist);
                MINAPI_PLUGIN_CALL (new_case,
				    new_case (tm_get_module_id 
					      (work_module_item),
					      work_case->tc_ext_id_,
					      work_case->title_));
        }
        result = 0;
EXIT:
	return result;
}
/* ------------------------------------------------------------------------- */
/** Handler for MSG_EVENT message.
 * @param message - pointer to MsgBuffer containing message
 * @return result of message handling, 0 if message was handled, -1 otherwise
 */
LOCAL int ec_msg_event_handler (MsgBuffer * message)
{
        minTestEventParam_t param;
        minEventSrc_t  esrc;
	int result = 0;
        int status;

        if (!event_system_up ()) {
                MIN_WARN ("Event System is not initialized, can not handle "
                           "message");
		result = -1;
		goto EXIT;
        }
        param.event.event_type_ = (message->param_ >> 8) & 0xff;
        param.command_ = message->param_ & 0xff;

        param.event.event_name_ = NEW2 (char, strlen (message->message_) + 1);

        STRCPY (param.event.event_name_, message->message_,
                strlen (message->message_) + 1);

        esrc.remote = 0;
        esrc.pid = message->special_;
        switch (param.event.event_type_) {
        case EIndication:
                switch (param.command_) {
                case EReqEvent:
                        MIN_DEBUG ("Indication Event: REQUEST");
                        ind_event_handle_request (&param, &esrc);
                        break;
                case EWaitEvent:
                        MIN_DEBUG ("Indication Event: WAIT");
                        ind_event_handle_wait (&param, &esrc, &status);
                        break;
                case ERelEvent:
                        MIN_DEBUG ("Indication Event: RELEASE");
                        ind_event_handle_release (&param, &esrc);
                        break;
                case ESetEvent:
                        MIN_DEBUG ("Indication Event: SET");
                        ind_event_handle_set (&param, &esrc);
                        break;
                case EUnsetEvent:
                        MIN_DEBUG ("Indication Event: UNSET N/A for"
                                     " Indications");
                        break;
                default:
                        break;
                }
                break;
        case EState:
                switch (param.command_) {
                case EReqEvent:
                        MIN_DEBUG ("State Event: REQUEST");
                        state_event_handle_request (&param, &esrc);
                        break;
                case EWaitEvent:
                        MIN_DEBUG ("State Event: WAIT");
                        state_event_handle_wait (&param, &esrc,
                                                 &status);
                        break;
                case ERelEvent:
                        MIN_DEBUG ("State Event: RELEASE");
                        state_event_handle_release (&param, &esrc);
                        break;
                case ESetEvent:
                        MIN_DEBUG ("State Event: SET");
                        state_event_handle_set (&param, &esrc);
                        break;
                case EUnsetEvent:
                        MIN_DEBUG ("State Event: UNSET");
                        state_event_handle_unset (&param, &esrc);
                        break;
                default:
                        break;
                }
                break;
        default:
                MIN_WARN ("Unsupported event type %d\n",
                           param.event.event_type_);
                break;
        }

        DELETE (param.event.event_name_);
EXIT:
        return result;
}
/* ------------------------------------------------------------------------- */
/** Handler for MSG_USR message. This message is used to pass 
 * "printouts" from test case to user. Handler will write received 
 * data to apropriate place in test result's printouts list according to 
 * priority of printout. Accessing proper
 * test case/result data will be done in a way similar to MSG_RET handler.
 * @param message pointer to MsgBuffer containing read message
 * @return result of message handling, 0 if message was handled, -1 otherwise.
 */
LOCAL int ec_msg_usr_handler (MsgBuffer * message)
{

        DLListIterator  work_module_item = INITPTR;
        DLListIterator  work_case_item = INITPTR;
        DLListIterator  work_printout_item = INITPTR;
        DLListIterator  work_result_item = INITPTR;
        DLList         *work_list = INITPTR;
        test_result_printout_s *print_msg;
        unsigned int    it = 0; /*general purpose iterator */
        int             case_id;
	int 		result = 0;

        work_module_item =
            tm_get_ptr_by_pid (instantiated_modules, message->sender_);
        print_msg = tr_create_printout (message->param_, message->message_);
	
	/* 
	** Messages with descriction __error_console__ have a special handling
	*/
	if (!strcmp (message->desc_, "__error_console__")) {
		MINAPI_PLUGIN_CALL (error_report,
				    error_report (message->message_));
		tr_remove_printout (print_msg);
		goto EXIT;
	}

        if (work_module_item == DLListNULLIterator) {
                MIN_WARN ("Failed to fetch module data");
		result = -1;
		tr_remove_printout (print_msg);
		goto EXIT;
        }

       /* in current design, there is only one test case with status : 
	* ongoing belonging to module with given PID, so we search 
	* "selected cases" list for test case based on that
	* information
	*/
        pthread_mutex_lock (&tec_mutex_);

        case_id = 0;
        work_case_item = dl_list_head (selected_cases);
        while (work_case_item != DLListNULLIterator) {

                if (((tc_get_status (work_case_item)) == TEST_CASE_ONGOING) &&
                    ((tm_get_pid (tc_get_test_module_ptr (work_case_item))) ==
                     message->sender_))
                        /* we found ongoing case with module matching message
			 * sender's pid, co we can stop iterating through list
			 */
                        break;
                case_id++;
                work_case_item = dl_list_next (work_case_item);
        }
	
        pthread_mutex_unlock (&tec_mutex_);

        if (work_case_item == DLListNULLIterator){
		result = -1;
		tr_remove_printout (print_msg);
		goto EXIT;
	}

        work_list = tc_get_tr_list (work_case_item);
        if (work_list == INITPTR) {
                MIN_WARN ("Fault in test data");
		result = -1;
		tr_remove_printout (print_msg);
		goto EXIT;
        }

        work_result_item = dl_list_head (work_list);
        while (work_result_item != DLListNULLIterator) {
                if (tr_get_result_type (work_result_item) ==
                    TEST_RESULT_NOT_RUN)
                        break;
                work_result_item = dl_list_next (work_result_item);
        }
	
        work_list = tr_get_priontouts_list (work_result_item);
        work_printout_item = dl_list_head (work_list);
        while (work_printout_item != DLListNULLIterator) {
                if (((test_result_printout_s *)
                     dl_list_data (work_printout_item))->priority_ >
                    message->param_)
                        break;
                it++;
                work_printout_item = dl_list_next (work_printout_item);
        }
        //dl_list_add_at (work_list, ( void* ) print_msg, it);
        dl_list_add (work_list, (void *)print_msg);
	MINAPI_PLUGIN_CALL(module_prints,
			   module_prints (tc_get_run_id (work_case_item),
					  message->message_));
EXIT:
        return result;
}
/* ------------------------------------------------------------------------- */
/** Handler for MSG_RUN_ID message. 
 * @param message - pointer to MsgBuffer containing message
 * @return result of message handling, 0 if message was handled, -1 otherwise
 */
LOCAL int ec_msg_run_id_handler (MsgBuffer * message)
{
        int             result = -1;
        DLListIterator  work_module_item = DLListNULLIterator;
        DLListIterator  work_case_item = DLListNULLIterator;
        test_case_s    *work_case = INITPTR;
	char            buf [20];
	Text           *cmd;

        work_module_item =
            tm_get_ptr_by_pid (instantiated_modules, message->sender_);
        if (work_module_item == DLListNULLIterator)
		goto EXIT;

	work_case_item = dl_list_head (selected_cases);
        while (work_case_item != DLListNULLIterator) {

                work_case = (test_case_s *) dl_list_data (work_case_item);
                if (((tc_get_status (work_case_item)) == TEST_CASE_ONGOING) &&
                    ((tm_get_pid (tc_get_test_module_ptr (work_case_item))) ==
                     message->sender_))
                        break;
                work_case_item = dl_list_next (work_case_item);

        }
	if (work_case_item == DLListNULLIterator)
		goto EXIT;

	tc_set_run_id (work_case_item, message->param_);

	MINAPI_PLUGIN_CALL(case_started,
			   case_started(tm_get_module_id(work_module_item),
					tc_get_ext_id (work_case_item),
					tc_get_run_id (work_case_item)));
				      
	/*
	** This case is flagged for debugging ->
	** attach a debugger
	*/
	if (work_case->debug_case_) {
		cmd = tx_create(engine_defaults->debugger_);
		tx_c_append (cmd, " tmc ");
		sprintf (buf, "%u", message->param_);
		tx_c_append (cmd, buf);
		result = system (tx_share_buf (cmd));
		tx_destroy (&cmd);
	}
        result = 0;
EXIT:
	return result;
}
/* ------------------------------------------------------------------------- */
/** Handler for MSG_TCDESC message. 
 * @param message - pointer to MsgBuffer containing message
 * @return result of message handling, 0 if message was handled, -1 otherwise
 */
LOCAL int       ec_msg_tcdesc_handler (MsgBuffer * message)
{
	DLListIterator  mod_it, it = DLListNULLIterator;
	DLList         *case_list;
        test_case_s    *tc;

	MIN_INFO ("TCDESC: %s", message->message_);

	mod_it = tm_get_ptr_by_pid (instantiated_modules, message->sender_);
	if (mod_it == DLListNULLIterator)
		goto EXIT;

	case_list = tm_get_tclist (mod_it);
	if (case_list == INITPTR)
		goto EXIT;
	
	for (it = dl_list_head (case_list); it != DLListNULLIterator; 
	     it = dl_list_next (it)) {
		tc = dl_list_data (it);
		if (tc->tc_id_ == message->param_ && !strcmp (message->desc_,
							   tc->tc_cfg_filename_)) {
			MINAPI_PLUGIN_CALL (case_desc,
					    case_desc (tm_get_module_id (mod_it),
						       tc->tc_ext_id_,
						       message->message_));
			
			break;
		}
	}
	return 0;
 EXIT:
	MIN_WARN ("Error handling TCDESC message");
	return -1;
}

/* ------------------------------------------------------------------------- */
/** Message dispatcher. Recognizes message type and calls appropriate handler.
 *  If received message is not of the type
 *  expected by engine, function logs error information.
 *  @param message - pointer to received MsgBuffer
 *  @return result of message handling, 0 if message was handled, -1 otherwise.
 */
LOCAL int ec_message_dispatch (MsgBuffer * rcvd_message)
{
        int             msg_handling_result = -1;

        MIN_DEBUG ("Received message type = %d", rcvd_message->type_);
        switch (rcvd_message->type_) {
        case MSG_OK:
                msg_handling_result = ec_msg_ok_handler (rcvd_message);
                break;
        case MSG_USR:
                msg_handling_result = ec_msg_usr_handler (rcvd_message);
                break;
        case MSG_RET:
                msg_handling_result = ec_msg_ret_handler (rcvd_message);
                break;
        case MSG_KO:
                msg_handling_result = ec_msg_ko_handler (rcvd_message);
		break;
        case MSG_TCD:
                msg_handling_result = ec_msg_tcd_handler (rcvd_message);
                break;
        case MSG_EVENT:
                msg_handling_result = ec_msg_event_handler (rcvd_message);
                break;
        case MSG_EXTIF:
                msg_handling_result = ec_msg_ms_handler (rcvd_message);
                break;
        case MSG_SNDRCV:
                msg_handling_result = ec_msg_sndrcv_handler (rcvd_message);
                break;
	case MSG_RUN_ID:
                msg_handling_result = ec_msg_run_id_handler (rcvd_message);
		break;
	case MSG_TCDESC:
		msg_handling_result = ec_msg_tcdesc_handler (rcvd_message);
		break;
        default:

                MIN_WARN ("Faulty message received of type: %d", 
			  rcvd_message->type_);
        }

        return msg_handling_result;

}
/* ------------------------------------------------------------------------- */
/** Message listening thread. Contains while 
 * loop that checks messages in queue and calls ec_msg_dispatch to handle any
 * message
 * @param arg some dummy pointer, to keep compiler from complaining
 */
LOCAL void     *ec_message_listener (void *arg)
{

        int             handling_result = 0;
        long            own_address = 0;
        unsigned long   wait_time = 2;  /*time in miliseconds that thread 
                                           spends sleeping during periods
                                           with no IPC traffic */
        MsgBuffer       received_msg;

        pthread_mutex_lock (&tec_mutex_);
        /* Engine's process id was made global, because passing it to message
	 * listener function as a pointer has proven to be unreliable, 
	 * especially on ARM targets. 
	 */
        own_address = engine_pid;  
        pthread_mutex_unlock (&tec_mutex_);

#ifdef MIN_UNIT_TEST
        unit_test_result = 1;
#endif                          /*MIN_UNIT_TEST */
        while (1) {
                handling_result = mq_peek_message (mq_id, own_address);
                if (handling_result != 0) {
                        wait_time = 2;
                        handling_result =
                            mq_read_message (mq_id, own_address,
                                             &received_msg);
                        if (handling_result > 8) {
                                ec_message_dispatch (&received_msg);
                        } else {
                                MIN_WARN ("Message_handling_fault");
                        }
                } else {
                        if (errno == EINVAL) {
                                MIN_INFO ("Message queue was destroyed,"
                                            " exiting listener thread");
                                pthread_exit (NULL);
                        } else {
                                if (wait_time < 200000)
                                        wait_time = wait_time * 10;
                                /*sleep should not be longer than 0.2 sec. */
                                usleep (wait_time);
                        }
                }



#ifdef MIN_UNIT_TEST
                unit_test_result = handling_result;
#endif
        }

        return NULL;
}
/* ------------------------------------------------------------------------- */
/** Test module controller process starting. 
 *  In case of failure to start tmc, information will be logged.
 * @param work_module_item item from available modules list
 * @return pid of newly created process, or -1 in case of failure
 */
LOCAL pid_t ec_start_tmc (DLListIterator work_module_item)
{
        char          **exec_args = NULL;

        DLListIterator  work_file_item;
        char           *mod_fpath = NULL;
        char           *mod_fpath_new = NULL;
        char           *dir = NULL;
        char           *mod_name = NULL;
        char           *conf_name = NULL;
        char           *conf_path = NULL;

        int             it = 0; /*general purpose loop iterator */
        /*position of directory in search paths list */
        int             path_pos = 0;   
        int             result = -1;
        int             retval = -1;
        pid_t           res_pid = 0;

        DLList         *work_list;

        work_list  = tm_get_cfg_filenames (work_module_item);
        exec_args = NEW2 (char *, dl_list_size (work_list) + 3);
        exec_args[it++] = engine_defaults->tmc_app_path_;

        if (*((test_module_info_s *) dl_list_data (work_module_item))->
            module_filename_ == '/') {
                mod_fpath =
                    ((test_module_info_s *) dl_list_data (work_module_item))->
                    module_filename_;
                result = access (mod_fpath, F_OK);
                if (result != 0) {
                        MIN_WARN ("Module not found %s",mod_fpath);
                        retval = -1;
                        goto out;
                }
        } else {
                mod_name =
                    NEW2 (char,
                          strlen (((test_module_info_s *)
                                   dl_list_data (work_module_item))->
                                  module_filename_) + 4);
                sprintf (mod_name, "%s.so",
                         ((test_module_info_s *)
                          dl_list_data (work_module_item))->module_filename_);
                path_pos = ec_search_lib (mod_name);

                if (path_pos != -1) {
                        dir =
                            (char *)
                            dl_list_data (dl_list_at
                                          (engine_defaults->search_dirs,
                                           path_pos));
                        mod_fpath_new =
                            NEW2 (char, strlen (dir) + strlen (mod_name) + 2);
                        mod_fpath = mod_fpath_new;
                        sprintf (mod_fpath, "%s/%s", dir, mod_name);
                }
        }

        if (mod_fpath == NULL) {
                MIN_WARN ("Module not found");
                retval = -1;
                goto out;
        }
        exec_args[it++] = mod_fpath;
        work_file_item = dl_list_head (work_list);
        while (work_file_item != DLListNULLIterator) {
                conf_name = (char *)dl_list_data (work_file_item);
                work_file_item = dl_list_next (work_file_item);
                if (*conf_name == '/') {
                        /*config file was defined as full path */
                        conf_path = NEW2 (char, strlen (conf_name) + 1);
                        sprintf (conf_path, "%s", conf_name);
                        exec_args[it++] = conf_path;
                        continue;
                }

                conf_path = NEW2 (char, strlen (conf_name) +
                                  strlen ((char *)
                                          dl_list_data (dl_list_at
                                                        (engine_defaults->
                                                         search_dirs,
                                                         path_pos))) + 2);
                sprintf (conf_path, "%s/%s",
                         (char *)
                         dl_list_data (dl_list_at
                                       (engine_defaults->search_dirs, path_pos)),
                         conf_name);
                /* now check if config exists in the same directory as 
                   module */
                result = access (conf_path, F_OK);
                if (result == 0) {
                        exec_args[it++] = conf_path;
                        continue;
                }
                /*config not found so far, let's look for it in search
                   paths */
                path_pos = ec_search_lib (conf_name);
                if (path_pos != -1) {
                        dir =
                            (char *)
                            dl_list_data (dl_list_at
                                          (engine_defaults->search_dirs,
                                           path_pos));
			DELETE (conf_path);
                        conf_path =
                            NEW2 (char,
                                  strlen (dir) + strlen (conf_name) + 2);
                        sprintf (conf_path, "%s/%s", dir, conf_name);
                        exec_args[it++] = conf_path;
                        continue;
                }
                MIN_WARN ("specified config not found : %s", conf_name);
                retval = -1;
                goto out;

        }
        exec_args[it++] = NULL;
        res_pid = fork ();
        switch (res_pid) {
        case -1:
                MIN_FATAL ("Failed to create process !");
                break;
        case 0:
                sched_yield ();
                execve (exec_args[0], exec_args, envp);
                MIN_FATAL ("Failed to load test module controller app :%s",
                             strerror (errno));
                exit (-1);
                break;
        default:
                break;
        };
        tm_set_pid (work_module_item, res_pid);
        retval = res_pid;
      out:
        DELETE (exec_args);
        DELETE (mod_fpath_new);
        DELETE (mod_name);
        DELETE (conf_path);
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Initializes data for instantiated module.
 * @param work_module_item list item with module
 * @param proc_id PID of associated TMC process
 * @return always 0
 */
LOCAL int ec_init_module_data (DLListItem * work_module_item)
{

        int             result = 0;
        DLList         *cases_list;

        tm_set_status (work_module_item, TEST_MODULE_INITIALIZED);
        cases_list = dl_list_create ();
        tm_set_tclist (work_module_item, cases_list);

        return result;
}
/* ------------------------------------------------------------------------- */
/** Parses the module configuration section(s) from the min configuraion file 
 *  @param inifile the min configuration file 
 *  @return 0, always
 */
LOCAL int ec_read_module_section (MinParser * inifile)
{
        test_module_info_s     *module     = INITPTR;
        DLListIterator          module_item = DLListNULLIterator;
        MinSectionParser       *module_def = INITPTR;
        MinItemParser          *line_item  = INITPTR; 
        char           *bin_path = INITPTR, *tc_file_path = NULL;
        int            it = 1;
        DLList         *work_list;


        module_def = mp_section (inifile, "[New_Module]", "[End_Module]", it);

        while (module_def != INITPTR) {


                line_item = mmp_get_item_line(module_def,
                                             "ModuleName",
                                             ESTag);

                if (line_item == INITPTR){
                        MIN_WARN ("Data fault!");
                        break;
                }

                mip_get_string(line_item,
                              "ModuleName",
                              &bin_path);


                if (bin_path == INITPTR) {
                        MIN_WARN ("Could not read module definition");
			mip_destroy (&line_item);
                        break;
                }
                /* make dummy list of cfgs for now */

                work_list = dl_list_create ();
                module = tm_create (bin_path, work_list, 0);
                module_item = tm_add (available_modules, module);
#ifdef MIN_EXTIF
		MINAPI_PLUGIN_CALL(new_module,
				   new_module (bin_path, module->module_id_));
#endif
                if (module_item == DLListNULLIterator) {
                        MIN_WARN ("Could not insert %s into list",
                                   bin_path);
			DELETE (bin_path);
			DELETE (module);
                        break;
                }

                /* Now let's read test case files, tag : TestCaseFile= */
                do {
                        DELETE (bin_path);
                        mip_destroy(&line_item);

                        line_item = mmp_get_next_item_line(module_def);

                        mip_get_string(line_item,
                                      "TestCaseFile",
                                      &bin_path);

                        if (bin_path != INITPTR) {
                                tc_file_path = NEW2 (char,
                                                     strlen (bin_path) + 2);
                                STRCPY (tc_file_path, bin_path,
                                        strlen (bin_path) + 1);
                                dl_list_add (work_list, (void *)tc_file_path);
                        }

                } while (bin_path != INITPTR);;

		mip_destroy(&line_item);
                mmp_destroy (&module_def);
                it++;

                module_def = mp_section (inifile,
                                         "[New_Module]", "[End_Module]", it);

        }

	mip_destroy(&line_item);
	mmp_destroy (&module_def);

        return 0;
}

#ifndef MIN_EXTIF
/* ------------------------------------------------------------------------- */
/** Read the IP slaves section from configuration file 
 *  @param inifile the min configuration file 
 *  @return 0, always
 */
LOCAL int ec_read_slaves_section (MinParser * inifile)
{
	char *hostname = NULL;
	char *slavetype = NULL;
	MinSectionParser *slave_def;
        MinItemParser    *line = INITPTR; 
	struct addrinfo hints, *result = NULL;
	int i = 1, ret;

        slave_def = mp_section (inifile, "[Slaves]", "[End_Slaves]", i);
        while (slave_def != INITPTR) {
		
		line = mmp_get_item_line (slave_def, "", ESNoTag);

		while (line != INITPTR) {
			ret = mip_get_string (line, "", &hostname);
			if (ret != ENOERR) {
				MIN_WARN ("error parsing slaves section:"
					  "no hostname specified");
				goto next_line;
			}

			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_family = AF_INET;    
			hints.ai_socktype = SOCK_STREAM; 
			hints.ai_flags = 0;
			hints.ai_protocol = 0;          
			
			ret = getaddrinfo(hostname, "51551", &hints, &result);
			if (ret != 0) {
				MIN_WARN ("failed to resolve host %s: %s",
					  strerror (h_errno));
				return 1;
			}
		
			mip_get_next_string (line, &slavetype);

			tec_add_ip_slave_to_pool (&result, 
						  slavetype ? 
						  slavetype : "phone");
			result = NULL;
			DELETE (slavetype);
			DELETE (hostname);
		next_line:
			mip_destroy(&line);
			
			line = mmp_get_next_item_line (slave_def);
		}
		mmp_destroy (&slave_def);
		i ++;
		slave_def = mp_section (inifile, "[Slaves]", "[End_Slaves]", i);
	}

	mip_destroy (&line);
        return 0;
}
#endif /* ndef MIN_EXTIF */
/* ------------------------------------------------------------------------- */
/** Looks for files in directory /etc/min.d and tries to find module
 *  configurations from them
 *  @return 1 if the directory /etc/min.d does not exists or an error occurs,
 *          0 otherwise
 */
LOCAL int ec_read_module_confdir (int op_mode)
{

        DIR            *dir;
        struct dirent  *dirent;
        MinParser     *modfile = INITPTR;

        dir = opendir ("/etc/min.d");

        if (dir == NULL) {
                MIN_WARN ("directory /etc/min.d missing");
                return 1;
        }

        dirent = readdir (dir);
        while (dirent != NULL) {
                if (dirent->d_type == DT_LNK || dirent->d_type == DT_REG) {
                        modfile = mp_create ("/etc/min.d/",
                                             dirent->d_name, ENoComments);
			if (op_mode == 0)
				ec_read_module_section (modfile);
#ifndef MIN_EXTIF
			ec_read_slaves_section (modfile);
#endif
			mp_destroy (&modfile);

                }
                dirent = readdir (dir);
        }

        closedir (dir);

        return 0;
}

LOCAL int ec_read_conf (MinParser * inifile, int operation_mode)
{
	int ret = 0;

	ret += settings_read (engine_section, inifile);
	ret += settings_read (logger_section, inifile);

        if (operation_mode == 0) {
                /*we should read module definitions only
		  if we don't use external controller */
                ec_read_module_section (inifile);
        }
#ifndef MIN_EXTIF
	ec_read_slaves_section (inifile);
#endif

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Checks if the user configuration dir exists and creates it if not
 */
LOCAL void create_local_confdir ()
{
        Text *confpath;
        char *home = getenv ("HOME");
        
        if (home == NULL)
                return;

        confpath = tx_create (home);
        tx_c_append (confpath, "/.min");

	/* directory does not exist, try to create */
	if (mkdir (tx_share_buf(confpath), S_IRWXU)) {
		if (errno != EEXIST) {
			MIN_FATAL ("Failed to create %s: %s\n"
				   "Exiting ...\n",tx_share_buf(confpath),
				   strerror (errno));
			goto err_exit;
		}
	}


        tx_destroy (&confpath);
        return; 
       
err_exit:
        tx_destroy (&confpath);
        exit (-1);
        return;
}
/* ------------------------------------------------------------------------- */
/** Signal handler to signal SIGSEGV
 * FATAL. Segmentation violation
 * @param signum signal number
 */
LOCAL void handle_sigsegv (int signum)
{
        raise (SIGQUIT);
}
/* ------------------------------------------------------------------------- */
/** Signal handler to signal SIGQUIT
 * FATAL. Process should dump core on this signal.
 * @param signum signal number
 */
LOCAL void handle_sigquit (int signum)
{
        ec_cleanup ();
        exit (-1);
}
/* ------------------------------------------------------------------------- */
/** Signal handler to signal SIGBUS
 * FATAL. Bus error.
 * @param signum signal number
 */
LOCAL void handle_sigbus (int signum)
{
        raise (SIGQUIT);
}
/* ------------------------------------------------------------------------- */
/** Signal handler to signal SIGTERM
 * Signal generated by (kill <processnumber>)
 * @param signum signal number
 */
LOCAL void handle_sigterm (int signum)
{
        ec_cleanup ();
        exit (0);
}
/* ------------------------------------------------------------------------- */
/** Signal handler to signal SIGINT
 * Signal generated by CTRL-C
 * @param signum signal number
 */
LOCAL void handle_sigint (int signum)
{
        ec_cleanup ();
        exit (0);
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Initializes all engine's global variables, creates message queues 
 * and threads. In case of problems during operations vital to test execution,
 * function exits process  - since if any of those operations fail - MIN will
 * not be able to work anyway
 * @param completecallbk pointer to function called when test case completes
 * @param printcallbk pointer to function called when test case tries to print
 *         output
 * @param extifsendcallbk pointer to function sending external controller 
 *        messages
 * @param envp pointer to enviroment settings var
 * @param operation_mode - describes what is the operation mode of min
 *         instance - 0 means console interface, 
 *                    1 - external controller interface.
 */
void ec_min_init (char *envp_[], int operation_mode)
{
        int             thread_creation_result, ret;
        pthread_t       listener_thread;
#ifndef MIN_EXTIF
        pthread_t       socket_thread;
#endif
	long            tmp = 0;
  
        operation_mode_ = operation_mode;

        envp = envp_;
        if (operation_mode == 0)
                create_local_confdir();

        /*
         ** Global data initialization
         */
        available_modules = dl_list_create ();
        instantiated_modules = dl_list_create ();
        selected_cases = dl_list_create ();

        pthread_mutex_lock (&tec_mutex_);
	rcp_handling_init ();

	/* 
	** Initialize settings system 
	*/
	settings_init();

	engine_section = settings_get_section ("Engine_Defaults");
	engine_defaults = settings_get ("Engine_Defaults");

	logger_section = settings_get_section ("Logger_Defaults");
	logger_settings = settings_get ("Logger_Defaults");
	

        engine_pid = getpid ();
        pthread_mutex_unlock (&tec_mutex_);


#ifndef MIN_EXTIF
	in->send_rcp = socket_send_rcp;
#endif /* MIN_EXTIF */

	MIN_DEBUG ("available_modules %x", available_modules);
        /* results = dl_list_create ( ); */
        if ((available_modules == INITPTR)
            || (instantiated_modules == INITPTR)
            || (selected_cases == INITPTR) || (results == INITPTR)) {

                MIN_FATAL ("Not enough memory to create list");
                min_log_close ();
                exit (-1);

        }

        mq_id = mq_open_queue ('a');
	/*
	** Try to clean up
	*/
        if (mq_id < 0) {
		MIN_WARN ("failed to create message queue "
			  " - trying to clean up");
		ret = system ("/usr/bin/min-clean.sh");
		if (ret < 0)
			MIN_WARN ("failed to run clean up script");
	}

	mq_id = mq_open_queue ('a');
        if (mq_id < 0) {
                MIN_WARN ("Failed to create message queue: %s",
                           strerror (errno));
                min_log_close ();
                exit (-1);
        }
        event_system_init ();

        /* Listener thread(s) creation */
        thread_creation_result =
            pthread_create (&listener_thread, NULL, ec_message_listener,
                            (void *)tmp);
        if (thread_creation_result != 0) {
                MIN_WARN ("Failed to create new thread");
                char           *fault_text = "unspecified problem";
                switch (thread_creation_result) {
                case EAGAIN:
                        fault_text =
                            "Limit of threads in the system exceeded";
                        break;
                case EPERM:
                        fault_text =
                            "Insufficient user rights to create thread";
                        break;
                }
                MIN_FATAL ("%s", fault_text);
                min_log_close ();
                exit (-1);
        }
#ifndef MIN_EXTIF
        thread_creation_result =
            pthread_create (&socket_thread, NULL, ec_poll_sockets,
                            (void *)tmp);
        if (thread_creation_result != 0) {
                MIN_WARN ("Failed to create new thread");
                char           *fault_text = "unspecified problem";
                switch (thread_creation_result) {
                case EAGAIN:
                        fault_text =
                            "Limit of threads in the system exceeded";
                        break;
                case EPERM:
                        fault_text =
                            "Insufficient user rights to create thread";
                        break;
                }
                MIN_FATAL ("%s", fault_text);
                min_log_close ();
                exit (-1);
        }

#endif
        sl_set_sighandler (SIGSEGV, handle_sigsegv);
        sl_set_sighandler (SIGTERM, handle_sigterm);
        sl_set_sighandler (SIGBUS, handle_sigbus);
        sl_set_sighandler (SIGQUIT, handle_sigquit);
        sl_set_sighandler (SIGINT, handle_sigint);
}
/* ------------------------------------------------------------------------- */
/** Instantiates all modules from available modules list.
 *  @return number of started modules
 */
int ec_start_modules ()
{

        int             result = 0;
        pid_t           module_started = 0;
        test_module_info_s *work_module;
        DLListIterator  work_list_item = dl_list_head (available_modules);
        DLListIterator  result_list_item;
        int             opresult = 0;
        
        MIN_DEBUG ("%d available modules",
                     dl_list_size (available_modules));

        work_module = (test_module_info_s *) dl_list_data (work_list_item);
        while (work_list_item != DLListNULLIterator) {
                module_started = ec_start_tmc (work_list_item);

                if (module_started > 0) {
                        result_list_item =
                            tm_add (instantiated_modules, work_module);
                        opresult = ec_init_module_data (result_list_item);
                        if (opresult < 0)
                                MIN_WARN ("Failed to initialize "
                                           "data for module");
                        /* To be decided if additional handling of that 
			 * fault is needed 
			 */
			result++;
#ifndef MIN_EXTIF

			MINAPI_PLUGIN_CALL(new_module,
					   new_module (work_module->module_filename_, 
						       work_module->module_id_));
#endif
                }

                work_list_item = dl_list_next (work_list_item);

                if (work_list_item != DLListNULLIterator) {
                        work_module = dl_list_data (work_list_item);
                }
        }

        return result;
}
/* ------------------------------------------------------------------------- */
/** Called to execute one selected test case.
 * It adds test case to selected test cases list, sets its status to ongoing
 * and group id to 0, since it does not belong to group. Also, it adds
 * new item to test case's results list.Function Sets module status to busy, 
 * then creates and sends MSG_EXE using data fetched from test case and 
 * module info
 * @param work_case_item pointer to list itemwith case to be executed
 * @return 0 on ok, -1 on error.
 */
int ec_exec_test_case (DLListIterator work_case_item)
{
        int             result = 0;
        pthread_mutex_lock (&tec_mutex_);
	/*add to selected cases list */
        work_case_item = ec_select_case (work_case_item, 0);

        pthread_mutex_unlock (&tec_mutex_);
        result = ec_exec_case (work_case_item);

        return result;
}
/*-----------------------------------------------------------------------*/
/** Execute one test case. Prequisite: before calling this
 *  function, case should be put in "selected_cases" list, and that's from 
 *  where argument should be taken.
 *  @param work_case_item pointer to item from "selected_cases" containing 
 *        test case to be executed.
 *  @return -1 on error, otherwise 0
 */
int ec_exec_case (DLListIterator work_case_item)
{

        DLList         *work_result_list = tc_get_tr_list (work_case_item);
        DLListIterator  test_result_item;
        int             work_module_status;
        test_case_s    *work_case = INITPTR;
        MsgBuffer       message;
        int             res = -1;
        DLListIterator  work_module_item =
            tc_get_test_module_ptr (work_case_item);

        if (work_module_item == INITPTR) {
                MIN_DEBUG (" exec case %x", work_case_item);
                MIN_WARN ("Faulty test case data");
                goto EXIT;

        }

        work_case = dl_list_data (work_case_item);

        if (work_case == DLListNULLIterator) {

                MIN_WARN ("Faulty test case data (2)");
                goto EXIT;

        }
        work_module_status = tm_get_status (work_module_item);

        switch (work_module_status) {

        case TEST_MODULE_READY:
                tm_set_status (work_module_item, TEST_MODULE_BUSY);
                tc_set_status (work_case_item, TEST_CASE_ONGOING);
                test_result_item = dl_list_head (work_result_list);
                /* it is enough to get "head of the list, since item in 
		 * selected list is just a copy of test case containing one 
		 * result in the list */
                tr_set_start_time (test_result_item, time (NULL));
                tr_set_priontouts_list (test_result_item, dl_list_create ());
                message.type_ = MSG_EXE;
                message.sender_ = engine_pid;
                message.receiver_ = tm_get_pid (work_module_item);
                message.param_ = tc_get_id (work_case_item);
                message.desc_[0] =  '\0';
                tc_get_cfg_filename (work_case_item, message.message_);
                res = mq_send_message (mq_id, &message);
                break;

        case TEST_MODULE_BUSY:
                res = ec_start_module_temp (work_case_item);
                break;

        default:
                MIN_WARN ("module status fault");
                res = -1;

        }
EXIT:
        return res;
}
/*----------------------------------------------------------------------------*/
/** Called to debug one test case. Prequisite: before calling this
 *  function, case should be put in "selected_cases" list, and that's from 
 *  where argument should be taken.
 *  @param work_case_item pointer to item from "selected_cases" containing 
 *        test case to be executed.
 *  @return -1 on error, otherwise 0
 */
int ec_debug_case (DLListIterator work_case_item)
{

        DLList         *work_result_list = tc_get_tr_list (work_case_item);
        DLListIterator  test_result_item;
        int             work_module_status;
        test_case_s    *work_case = INITPTR;
        MsgBuffer       message;
        int             res = -1;
        DLListIterator  work_module_item =
            tc_get_test_module_ptr (work_case_item);

        if (work_module_item == INITPTR) {
                MIN_DEBUG ("debug case %x", work_case_item);
                MIN_WARN ("Faulty test case data");
                goto EXIT;

        }

        work_case = dl_list_data (work_case_item);
	work_case->debug_case_ = 1;
        if (work_case == DLListNULLIterator) {

                MIN_WARN ("Faulty test case data (2)");
                goto EXIT;

        }
        work_module_status = tm_get_status (work_module_item);

        switch (work_module_status) {

        case TEST_MODULE_READY:
                tm_set_status (work_module_item, TEST_MODULE_BUSY);
                tc_set_status (work_case_item, TEST_CASE_ONGOING);
                test_result_item = dl_list_head (work_result_list);
                /* it is enough to get "head of the list, since item in 
		 * selected list is just a copy of test case containing one 
		 * result in the list */
                tr_set_start_time (test_result_item, time (NULL));
                tr_set_priontouts_list (test_result_item, dl_list_create ());
                message.type_ = MSG_EXE_DLD;
                message.sender_ = engine_pid;
                message.receiver_ = tm_get_pid (work_module_item);
                message.param_ = tc_get_id (work_case_item);
                message.desc_[0] =  '\0';
                tc_get_cfg_filename (work_case_item, message.message_);
                res = mq_send_message (mq_id, &message);
                break;

        case TEST_MODULE_BUSY:
                res = ec_start_module_temp (work_case_item);
                break;

        default:
                MIN_WARN ("module status fault");
                res = -1;

        }
EXIT:
        return res;
}

/* ------------------------------------------------------------------------- */
/** Called by UI to pause execution of selected test case.
 * @param work_case_item pointer to dl_list_item containig selected test case.
 * @return result of operation :0 if message was sent successfully, -1 in case
 *        of data or message queue error, -2 if case is already paused,
 *        -3 if case is not running
 */
int ec_pause_test_case (DLListIterator work_case_item)
{
        DLListIterator  work_module_item;
        int             result = 0;
        int             case_status = tc_get_status (work_case_item);
        long            addr = 0;

        switch (case_status) {
        case TEST_CASE_ONGOING:
                work_module_item = tc_get_test_module_ptr (work_case_item);
                addr = tm_get_pid (work_module_item);
                result = mq_send_message2 (mq_id, addr, MSG_PAUSE, 0, "\0");
                if (result == 0)
                        tc_set_status (work_case_item, TEST_CASE_PAUSED);
                MINAPI_PLUGIN_CALL (case_paused,case_paused 
				    (tc_get_run_id (work_case_item)));
                break;
        case TEST_CASE_PAUSED:
                result = -2;
                break;
        default:
                result = -3;
        }

        return result;
}
/* ------------------------------------------------------------------------- */
/** Called by UI to resume execution of previously paused case:
 * @param work_case_item pointer to dl_list_item containing selected test case
 * @return : result of operation: 0 if message was sent successfully, 
 *                               -1 in case of data or message queue error,
 *                               -2 if case was not paused, 
 *                               -3 if case is not running (not started or
 *                                  already finished).
 */
int ec_resume_test_case (DLListIterator work_case_item)
{
        DLListIterator  work_module_item;
        int             result = 0;
        int             case_status = tc_get_status (work_case_item);
        long            addr = 0;

        switch (case_status) {
        case TEST_CASE_PAUSED:
                work_module_item = tc_get_test_module_ptr (work_case_item);
                addr = tm_get_pid (work_module_item);
                result = mq_send_message2 (mq_id, addr, MSG_RESUME, 0, "\0");
                if (result == 0) {
                        tc_set_status (work_case_item, TEST_CASE_ONGOING);
                }
		MINAPI_PLUGIN_CALL(case_resumed,
				   case_resumed (tc_get_run_id 
						 (work_case_item)));
                break;
        case TEST_CASE_ONGOING:
                result = -2;
                break;
        default:
                result = -3;
                break;
        }

        return result;
}

/* ------------------------------------------------------------------------- */
/** Called by UI to stop test case execution.
 * @param work_case_item pointer to dl_list_item containing selected test case
 * @return result of operation: 0 if message was sent successfully, -1 in case
 * of data or message queue error, -2 if case has already finished execution.
 */
int ec_abort_test_case (DLListIterator work_case_item)
{
        DLListIterator  work_module_item;
        int             result = 0;
        int             case_status = tc_get_status (work_case_item);
        long            addr = 0;

        switch (case_status) {
        case TEST_CASE_PAUSED:
        case TEST_CASE_ONGOING:
                work_module_item = tc_get_test_module_ptr (work_case_item);
                addr = tm_get_pid (work_module_item);
                result = mq_send_message2 (mq_id, addr, MSG_STOP, 0, "\0");
                break;
        case TEST_CASE_TERMINATED:
                result = -2;
                break;
        default:
                result = -1;
        }

        return result;
}
/* ------------------------------------------------------------------------- */
/** Function for freeing the "session" 
 */
void ec_reinit()
{
        long            address;
        DLListIterator  work_module_item;
        DLListIterator  work_module_item2;
        DLListIterator  work_case_item;
        DLListIterator  work_case_item2;
	DLListIterator  filter_item;
        test_case_s    *work_case = INITPTR;
        DLList         *work_list;
	title_filter   *filter;

        work_module_item = dl_list_head (instantiated_modules);
	/*shutdown all running tmcs and free list*/
        while (work_module_item != DLListNULLIterator) {
                address = tm_get_pid (work_module_item);
                mq_send_message2 (mq_id, address, MSG_END, 0, "Shut down !");
                waitpid (address, NULL, 0);
                /*we suppose that sending this message to TMC will ensure
                   that it shuts down properly */
                work_module_item2 = dl_list_next (work_module_item);
		tm_remove (work_module_item);
                work_module_item = work_module_item2;
        }

        work_module_item = dl_list_head (available_modules);
        while (work_module_item != DLListNULLIterator) {
                work_module_item2 = dl_list_next (work_module_item);
                work_list = tm_get_tclist (work_module_item);
                work_case_item = dl_list_head (work_list);
                while (work_case_item != DLListNULLIterator) {
                        work_case_item2 = dl_list_next (work_case_item);
                        tc_delete ((test_case_s *)
                                   dl_list_data (work_case_item));
                        tc_remove (work_case_item);
                        work_case_item = work_case_item2;
                }
                tm_delete ((test_module_info_s *)
                           dl_list_data (work_module_item));
                tm_remove (work_module_item);
                work_module_item = work_module_item2;
        }
        
	work_case_item = dl_list_head (selected_cases);
        while (work_case_item != DLListNULLIterator) {
                work_case = (test_case_s *) dl_list_data (work_case_item);
                tc_remove (work_case_item);
                tc_delete (work_case);
                work_case_item = dl_list_head (selected_cases);
        }

	filter_item = dl_list_head (filters);
	while (filter_item != DLListNULLIterator) {
		filter = (title_filter *) dl_list_data (filter_item);
		tx_destroy (&filter->filter_);
		DELETE (filter);
		dl_list_remove_it (filter_item);
		filter_item = dl_list_head (filters);
	}
        
}
/* ------------------------------------------------------------------------- */
/** Takes care of removal of all created objects, freeing
 * memory and so on.
 */
void ec_cleanup ()
{
        event_system_cleanup ();
	rcp_handling_cleanup ();
	ec_reinit();
	settings_destroy();
        dl_list_free (&instantiated_modules);
        dl_list_free (&available_modules);
        dl_list_free (&selected_cases);
	dl_list_free (&filters);

        /* This sleep period allows for handling of all sent messages before
	 * destroying message queue 
	 */

        usleep (50000);
        sm_destroy (sh_mem_id_);
        mq_close_queue (mq_id);
        min_log_close();
        usleep (30000);
}
/* ------------------------------------------------------------------------- */
/** Called by user to execute a sequence of test cases
 * @param work_cases_list list of test cases to be executed - user has to build
 * list of test cases by himself.
 */
int ec_run_cases_seq (DLList * work_cases_list)
{
        DLListIterator  work_case_item = dl_list_head (work_cases_list);
        DLListIterator  exec_case = DLListNULLIterator;
        int             group_id = random ();   /* generate unique group id */

        if (work_case_item == DLListNULLIterator) {
                MIN_WARN ("Invalid list of test cases passed !");
                return -1;
        }
        pthread_mutex_lock (&tec_mutex_);

        /*we mark first case of the group to be executed */
        exec_case = ec_select_case (work_case_item, group_id);
        work_case_item = dl_list_next (work_case_item);
        /*now we will iterate through the remaining cases in received list */
        while (work_case_item != DLListNULLIterator) {
                ec_select_case (work_case_item, group_id);
                work_case_item = dl_list_next (work_case_item);
        }
        pthread_mutex_unlock (&tec_mutex_);

        /*now we need to clear passed list, so we don't leave any garbage
           behind. The list itself cannot be destroyed,because it is
           console's global variable. This can change, though. */
        work_case_item = dl_list_head (work_cases_list);
        while (work_case_item != DLListNULLIterator) {
                dl_list_remove_it (work_case_item);
                work_case_item = dl_list_head (work_cases_list);
        }
        /*all done, now start execution of group with first case */
        ec_exec_case (exec_case);
        return 0;
}
/* ------------------------------------------------------------------------- */
/** Called by user to execute test cases simultaneouslu.
 * @param work_cases_list list of test cases to be executed - user has to build
 * list of test cases by himself.
 */
int ec_run_cases_par (DLList * work_cases_list)
{
        DLListIterator  work_case_item = dl_list_head (work_cases_list);
        DLListIterator  exec_case = DLListNULLIterator;

        if (work_case_item == DLListNULLIterator) {
                MIN_WARN ("Invalid list of test cases passed !!");
                return -1;
        }
        pthread_mutex_lock (&tec_mutex_);

        /*"select" all cases in received list */
        while (work_case_item != DLListNULLIterator) {
                exec_case = ec_select_case (work_case_item, 0);
                ec_exec_case (exec_case);
                work_case_item = dl_list_next (work_case_item);
        }
        pthread_mutex_unlock (&tec_mutex_);

        /*now we need to clear passed list, so that we don't leave any garbage
           behind. The list itself cannot be destroyed,because it is
           console's global variable. This can change, though. */
        work_case_item = dl_list_head (work_cases_list);
        while (work_case_item != DLListNULLIterator) {
                dl_list_remove_it (work_case_item);
                work_case_item = dl_list_head (work_cases_list);
        }

        return 0;
}
/* ------------------------------------------------------------------------- */
/** Used to add test module to MIN framework at any time.
 *  @param mod_name name of module to be added
 *  @param testcase_files list containing names of test case files associated
 *         with the module.
 *  @param id test module identifier
 *  @param report flag stating if the module adding should be reported through
 *         MIN engine API plugin.
 *  @return 0 on success, -1 on error.
 *
 *  WARNING: function fails if there is problem 
 *  with initializing data for module entity, possibly some memory problem, or
 *  if any of specified files does not exist. Information, if module was 
 *  started successfully, (returned test cases) is not available. Indication of
 *  problems of that kind would  be failure to start case from that 
 *  module later.
 */
int ec_add_module (TSChar * mod_name, DLList * testcase_files, 
                   test_module_id_t  id, int report)
{
        DLListIterator  work_module_item = DLListNULLIterator;
        test_module_info_s *work_module = INITPTR;
        pid_t           result = 0;
        int             retval = -1;

        work_module = tm_create (mod_name, testcase_files, id);
        if (work_module == INITPTR)
                goto FAULT;
        work_module_item = tm_add (available_modules, work_module);
        result = ec_start_tmc (work_module_item);
        ec_init_module_data (work_module_item);
        if (result > 0) {
                pthread_mutex_lock (&tec_mutex_);
                tm_add (instantiated_modules, work_module);
                pthread_mutex_unlock (&tec_mutex_);
                retval = 0;
		if (report) {
			MINAPI_PLUGIN_CALL(new_module,
					   new_module (mod_name, 
						       work_module->module_id_)
					   );

		}
        } else {
                retval = -1;
        }

      FAULT:
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Searches defined library search paths for library with given name
 *  @param mod_name name of file to be found
 *  @return position of correct path on search paths list, or -1 if file is
 *          not found.
 */
int ec_search_lib (char *mod_name)
{
        char           *full_path = NULL;
        DLListIterator  work_path_item = DLListNULLIterator;
        int             result = 666;
        int             pos = 0;
        char           *work_path = NULL;
        int             retval = -1;

        for (pos = 0; pos < dl_list_size (engine_defaults->search_dirs); pos++) {
                work_path_item = dl_list_at (engine_defaults->search_dirs, pos);
                work_path = (char *)dl_list_data (work_path_item);
                full_path =
                    NEW2 (char, strlen (work_path) + strlen (mod_name) + 2);
                sprintf (full_path, "%s/%s", work_path, mod_name);
                result = access (full_path, F_OK);
                DELETE (full_path);
                if (result == 0) {
                        retval = pos;
                        break;
                }
        }

        return retval;
}
/* ------------------------------------------------------------------------- */
/** Reads config files in following order : MIN_CONF_DIR/min.conf, 
 *  $HOME/.min/min.conf, ./min.conf. Settings with single value are 
 *  overwritten, module and search paths are appended.
 *  @return 0 on success, negative on error
 */
int ec_configure ()
{
        MinParser      *inifile = INITPTR;
        char           *home_d = NULL;
        char           *curr_d = NULL;
        char           *def_d = MIN_CONF_DIR;
        char           *min_d;
        int             op_mode = operation_mode_;
	int             retval = 0;
	Text           *tx;

	tx = tx_create (getenv("HOME"));
	home_d = tx_get_buf (tx);
	tx_c_copy (tx, getenv ("PWD"));
	curr_d = tx_get_buf (tx);
	tx_destroy (&tx);


        /*Read settings from global .conf */
        if (curr_d == NULL || strcmp (MIN_CONF_DIR, curr_d) != 0) {
                inifile = mp_create (def_d, "min.conf", ENoComments);

                if (inifile == INITPTR) {
                        MIN_WARN ("Could not open %s/min.conf %s\n",
                                   MIN_CONF_DIR, strerror (errno));
                } else {
                        retval = ec_read_conf (inifile, op_mode);
                        mp_destroy (&inifile);
                }
        }
        /*Read $HOME/.min/min.conf */
        if (home_d != NULL) {
                min_d = NEW2 (char, strlen (home_d) + 7);
                sprintf (min_d, "%s/.min", home_d);
                if (curr_d != NULL && strcmp (min_d, curr_d) != 0) {
                        inifile = mp_create (min_d, "min.conf", ENoComments);
                        
                        if (inifile == INITPTR) {
                                MIN_WARN ("Could not open "
                                             "%s/min.conf %s\n",
                                             min_d, strerror (errno));
                        } else {
                                retval += ec_read_conf (inifile, op_mode);
                                mp_destroy (&inifile);
                        }
                }
                DELETE (min_d);
        } else {
                MIN_WARN ("$HOME == NULL, not reading "
                           "min.conf from ~/.min");
        }
        /*try to read ./min.conf */
        if (curr_d != NULL) {
                inifile = mp_create (curr_d, "min.conf", ENoComments);
                
                if (inifile == INITPTR) {
                        MIN_DEBUG ("Could not open %s/min.conf %s\n", 
                                    curr_d,
                                    strerror (errno));
                } else {
                        retval += ec_read_conf (inifile, op_mode);
                        mp_destroy (&inifile);
                }
        } else {
                MIN_WARN ("$PWD == NULL, not reading "
                           "min.conf from .");
        }

	/* 
	** Default to gdb
	*/
	if (engine_defaults->debugger_ == NULL) {
		engine_defaults->debugger_ = "gdb";
	}
        /*
         ** Read module definitions from /etc/min.d/
         */
	ec_read_module_confdir (op_mode);
        ec_settings_send ();

        min_log_open ("MIN", 3);
	DELETE (home_d);
	DELETE (curr_d);

        return retval;
}
/* ------------------------------------------------------------------------- */
void
ec_add_title_filter (char *filter_str, int regexp)
{
	title_filter *filter;
	filter = NEW (title_filter);

	filter->filter_ = tx_create (filter_str);
	filter->regexp_ = regexp;

	if (filters == INITPTR) filters = dl_list_create();

	dl_list_add (filters, filter);

	return;
}
/* ------------------------------------------------------------------------- */
int ec_read_settings (char *engine_ini)
{
        MinParser     *passed_config = INITPTR;
        char           *out = NULL;
        char           *fname = NULL;
        char           *dir = NULL;
        int             cont_flag = 0;
        int             status = 0;
        int             result = 0;
        int             i = 0;
        DLListIterator  work_module_item = DLListNULLIterator;
        char           *temp = engine_ini;

        do {
                out = &temp[1];
                temp = strchr (out, '/');
        } while (temp != NULL);

        fname = out;
        dir = NEW2 (char, strlen (engine_ini) - strlen (fname));
        strncpy (dir, engine_ini, strlen (engine_ini) - strlen (fname) - 1);
        dir[strlen (engine_ini) - strlen (fname) - 1] = '\0';
        printf (" read : %s | %s \n", dir, fname);
        passed_config = mp_create (dir, fname, ENoComments);
	DELETE (dir);
        ec_read_conf (passed_config, 0);
	mp_destroy (&passed_config);

        /*in case settings shared memory segment exists already, destroy it */
        sm_destroy (sh_mem_id_);
        /*now send new settings */
        ec_settings_send ();

        ec_start_modules ();
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
                if (i > 20) {
                        result = -1;
                        break;
                };
        }

        return result;
}


/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "tec.tests"
#endif                          /* MIN_UNIT_TEST */

/* End of file */
