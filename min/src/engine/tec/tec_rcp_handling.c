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
 *  @file       tec_rcp_handling.c
 *  @version    0.1
 *  @brief      This file contains implementation of min RCP message handling
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <tec.h>
#include <tec_events.h>
#include <data_api.h>
#include <string.h>
#include <scripter_keyword.h>
#include <mintfwif.h>
#include <min_engine_api.h>
#ifndef MIN_EXTIF
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tec_tcp_handling.h>
#endif

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
#ifdef MIN_EXTIF
extern tfwif_callbacks_s tfwif_callbacks;
#endif 
extern eapiIn_t *in;

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern pthread_mutex_t tec_mutex_;

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
extern char    *strcasestr (__const char *__haystack, __const char *__needle);

/* GLOBAL VARIABLES */
/** List containing association data for master - slave system
(slave_info structures)*/
DLList         *ms_assoc;
DLList         *EXTIF_received_data;
#ifdef MIN_EXTIF
min_case_complete_func original_complete_callback;
#endif
TSBool          ok_to_break;    /*flag stating if it is ok to revert back to
                                 * normal external controller communication 
                                 * instead of slave mode */
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
LOCAL int       min_if_dispatch_extif_msg (MinItemParser * extif_message);
/* ------------------------------------------------------------------------- */
LOCAL int       extif_msg_handle_reserve (MinItemParser * extif_message);
/* ------------------------------------------------------------------------- */
LOCAL int       extif_msg_handle_release (MinItemParser * extif_message);
/* ------------------------------------------------------------------------- */
LOCAL int       extif_msg_handle_response (MinItemParser * extif_message);
/* ------------------------------------------------------------------------- */
LOCAL int       extif_msg_handle_command (MinItemParser * extif_message);
/* ------------------------------------------------------------------------- */
LOCAL int       handle_remote_run (MinItemParser * extif_message);
/* ------------------------------------------------------------------------- */
LOCAL int       handle_remote_pause (MinItemParser * extif_message,
                                     int case_id);
/* ------------------------------------------------------------------------- */
LOCAL int       handle_remote_cancel (MinItemParser * extif_message,
                                      int case_id);
/* ------------------------------------------------------------------------- */
LOCAL int       splithex (char *hex, int *dev_id, int *case_id);
/* ------------------------------------------------------------------------- */
LOCAL int       handle_remote_sendreceive (MinItemParser * extif_message,
                                           int dev_id);
/* ------------------------------------------------------------------------- */
LOCAL slave_info *find_slave_by_he (struct hostent *he, DLListIterator *itp);
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
void            master_report (int run_id, int execution_result,
                               int test_result, char *desc);
/* ==================== LOCAL FUNCTIONS ==================================== */
/** Function used to split string looking like name=value into 2 parts. 
 *  It was necessary, because parser has only functionality to extract value 
 *  only if name is known. Remember to deallocate "name" and "value" after
 *  they are not needed.
 * @param input [in] string to be split
 * @param name [out] points to string containing part before "=" sign
 * @param var [out] points to string containing part after "=" sign
 * @return result of operation (0 if ok, -1 in case of problems); "name"
 * and "value" will be NULL if splitting fails.
*/
LOCAL int split_string_eq (char *input, char **name, char **var)
{

        char           *delimiter;
        int             name_size;
        int             var_size;

        delimiter = strchr (input, '=');
        if (delimiter == NULL) {
                name = NULL;
                var = NULL;
                return -1;
        }

        var_size = strlen (delimiter + 1) + 1;
        name_size = strlen (input) - strlen (delimiter) + 1;

        *name = NEW2 (char, name_size);
        snprintf (*name, name_size, "%s", input);
        *var = NEW2 (char, var_size);
        sprintf (*var, "%s", delimiter + 1);
        MIN_DEBUG ("name = %s, var = %s", *name, *var);

        return 0;

}

LOCAL int
extif_send_ipc_sendrcv (long receipent, char *data_name, char *data_value)
{
        MsgBuffer       data_message;
        data_message.type_ = MSG_SNDRCV;
        data_message.sender_ = getpid ();
        data_message.receiver_ = receipent;
        sprintf (data_message.message_, "%s", data_name);
        sprintf (data_message.desc_, "%s", data_value);

        return mq_send_message (mq_id, &data_message);
}

LOCAL int get_id_from_slavename (char *slavename)
{
        DLListIterator  work_slave_item = DLListNULLIterator;
        slave_info     *work_slave_entry;
        work_slave_item = dl_list_head (ms_assoc);
        int             retval = -1;
        if (strcmp (slavename, "master") == 0)
                retval = 0xdead;
        else {
                while (work_slave_item != DLListNULLIterator) {
                        work_slave_entry =
                            (slave_info *) dl_list_data (work_slave_item);
                        if (strcasecmp
                            (tx_share_buf (work_slave_entry->slave_name_),
			     slavename) == 0) {
                                retval = work_slave_entry->slave_id_;
                                break;
                        }
                        work_slave_item = dl_list_next (work_slave_item);
                }
        }
        return retval;
}

LOCAL int extif_msg_handle_release (MinItemParser * extif_message)
{
        /*restore original "complete" callback if allowed, otherwise mark that 
           it's allowed */
        send_to_master (0, "release 0");

        if (ok_to_break == ESTrue) {
#ifdef MIN_EXTIF
                tfwif_callbacks.complete_callback_ = original_complete_callback;
#else
		;
#endif
        } else
                ok_to_break = ESTrue;
	
        return 0;
}

/* ------------------------------------------------------------------------- */
LOCAL void
tec_ms_handle_data_request (int slave_id, char *varname, long requester)
{
        DLListIterator  work_data_item = DLListNULLIterator;
        received_data  *work_data_entry;
        MsgBuffer       ipc_message;
        pthread_mutex_lock (&tec_mutex_);

        work_data_item = dl_list_head (EXTIF_received_data);
        while (work_data_item != DLListNULLIterator) {
                work_data_entry =
                    (received_data *) dl_list_data (work_data_item);
                MIN_DEBUG (" > %d : %d < ", work_data_entry->sender_id_,
                             slave_id);
                MIN_DEBUG (" > %s : %s < ", work_data_entry->data_name_,
                             varname);

                if ((work_data_entry->sender_id_ == slave_id) &&
                    (strcmp (work_data_entry->data_name_, varname) == 0)) {
                        /*data entry exists already - data was received */
                        MIN_DEBUG ("FOUND SOME DATA ENTRY");
                        break;
                }
                work_data_item = dl_list_next (work_data_item);
        }
        if (work_data_item != DLListNULLIterator) {
                work_data_entry->requested_by_ = requester;
                ipc_message.type_ = MSG_SNDRCV;
                ipc_message.sender_ = getpid ();
                ipc_message.receiver_ = work_data_entry->requested_by_;
                sprintf (ipc_message.message_, "%s",
                         work_data_entry->data_name_);
                sprintf (ipc_message.desc_, "%s", work_data_entry->data_);
                MIN_DEBUG ("returning data");
                mq_send_message (mq_id, &ipc_message);
        } else {
                work_data_entry = NEW (received_data);
                work_data_entry->sender_id_ = slave_id;
                sprintf (work_data_entry->data_name_, "%s", varname);
                work_data_entry->requested_by_ = requester;
                MIN_DEBUG ("Caching request:");
                MIN_DEBUG ("sender_id_ = %d", work_data_entry->sender_id_);
                MIN_DEBUG ("varname = %s", work_data_entry->data_name_);
                dl_list_add (EXTIF_received_data, (void *)work_data_entry);
        }
        pthread_mutex_unlock (&tec_mutex_);
}

/* ------------------------------------------------------------------------- */
LOCAL int extif_msg_handle_command (MinItemParser * extif_message)
{

        char           *srcaddr = NULL;
        char           *destaddr = NULL;
        char           *token = NULL;
        int             dev_id = 0;
        int             case_id = -1;
	int		retval = 0;
        TScripterKeyword kw;

        mip_get_next_string (extif_message, &srcaddr);
        mip_get_next_string (extif_message, &destaddr);
        splithex (destaddr, &dev_id, &case_id);
        MIN_DEBUG (">>destaddr = %s<<", destaddr);
        MIN_DEBUG (">> dev_id = %d, case_id = %d<<", dev_id, case_id);
        mip_get_next_string (extif_message, &token);

        MIN_DEBUG ("%s: Keyword %s", __FUNCTION__,
                    token != INITPTR ? token : "NULL");

        kw = get_keyword (token);
        switch (kw) {
        case EKeywordRequest:
        case EKeywordRelease:
        case EKeywordSet:
        case EKeywordUnset:
                retval = handle_remote_event (kw, extif_message);
		goto EXIT;
                break;
        case EKeywordRun:
                retval = handle_remote_run (extif_message);
		goto EXIT;
                break;
        case EKeywordResume:
                break;
        case EKeywordCancel:
                retval = handle_remote_cancel (extif_message, case_id);
		goto EXIT;
                break;
        case EKeywordPause:
                retval = handle_remote_pause (extif_message, case_id);
		goto EXIT;
                break;
        case EKeywordCancelIfError:
                break;
        case EKeywordSendreceive:
                splithex (srcaddr, &dev_id, &case_id);
                MIN_DEBUG (">>> %s : %d <<<", srcaddr, dev_id);
                retval = handle_remote_sendreceive (extif_message, dev_id);
		goto EXIT;
                break;
        default:
        case EKeywordUnknown:
                MIN_WARN ("%s: unhandled keyword %s", __FUNCTION__, token);
                break;
        }
EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/** Function handles RCP message containing sendreceive command
*/
LOCAL int handle_remote_sendreceive (MinItemParser * extif_message, int dev_id)
{

        char           *data = NULL;
        char           *data_value = NULL;
        char           *data_name = NULL;
        int             retval = 0;
        DLListIterator  work_data_item;
        received_data  *work_data_entry;

        /*check if data entry exists in list, i. e. it was
           already requested */
        mip_get_next_string (extif_message, &data);

        retval = split_string_eq (data, &data_name, &data_value);
        if (retval != 0) {
                MIN_WARN ("Faulty message format");
                return -1;
        }

        pthread_mutex_lock (&tec_mutex_);
        work_data_item = dl_list_head (EXTIF_received_data);
        while (work_data_item != DLListNULLIterator) {
                work_data_entry =
                    (received_data *) dl_list_data (work_data_item);
                if ((work_data_entry->sender_id_ == dev_id)
                    && ((strcasecmp (work_data_entry->data_name_, data_name))
                        == 0)) {
                        MIN_DEBUG (" > %d : %d < ",
                                    work_data_entry->sender_id_, dev_id);
                        MIN_DEBUG (" > %s : %s < ",
                                    work_data_entry->data_name_, data_name);
                        break;
                }
                work_data_item = dl_list_next (work_data_item);
        }

        if (work_data_item == DLListNULLIterator) {
                /*no-one mentioned this data yet, create an entry */
                work_data_entry = NEW (received_data);
                work_data_entry->sender_id_ = dev_id;
                sprintf (work_data_entry->data_name_, "%s", data_name);
                sprintf (work_data_entry->data_, "%s", data_value);
                work_data_entry->requested_by_ = 0;
                dl_list_add (EXTIF_received_data, (void *)work_data_entry);
                MIN_DEBUG ("sender_id_ = %d", work_data_entry->sender_id_);
                MIN_DEBUG ("varname = %s", work_data_entry->data_name_);
                MIN_DEBUG ("EXTIF received data has now %d entries",
                             dl_list_size (EXTIF_received_data));
        } else {
                MIN_DEBUG ("data entry found");
                sprintf (work_data_entry->data_, "%s", data_value);
                extif_send_ipc_sendrcv (work_data_entry->requested_by_,
                                      data_name, data_value);
		/* removing used variable from buffer */
		DELETE ( work_data_entry );
		dl_list_remove_it(work_data_item);
        }
        pthread_mutex_unlock (&tec_mutex_);
        return 0;
}



/* ------------------------------------------------------------------------- */
/**Function handles RCP message containing "remote run" command.
 * @param extif_message - pointer to item parser containing received message. 
 * Assume that get_next_string was called several times, until "run" command
 * was extracted - so next get_string will give first parameter to run keyword.
 * @return result of operation - 0 if ok.
*/
LOCAL int handle_remote_run (MinItemParser * extif_message)
{
        char           *module = NULL;
        filename_t      module_filename;
        char           *casefile = NULL;
        char           *casefile_con = NULL;    /*test case's config */
        char           *temp_string = NULL;
        int             caseid = 0;
        int             result = 0;
        int             cont_flag = 0;
        int             check_result = 0;
        int             i = 0;  /*general purpose loop iterator */
        int             mod_found = 0, conf_found = 0;
        DLListIterator  work_module_item = DLListNULLIterator, it;
        test_module_info_s *work_module = INITPTR;
	test_case_s    *work_case;
        DLListIterator  work_case_item = DLListNULLIterator;
        char           *message;
        int             error_code = 0;
        DLList         *conf_list = dl_list_create ();  /* helper list for 
                                                         * adding module */
        title_string_t  tc_title;
        TParsingType    parsing = ENormalParsing;
        char           *casetitle = NULL;
        filename_t      name;

        result = mip_get_string (extif_message, "module=", &module);
        if (result != 0) {
                MIN_WARN ("remote module not specified");
                goto FAULT;
        }

        result = mip_get_string (extif_message, "testcasefile=", &casefile);
        if (result != 0) {
                MIN_WARN ("test case file not specified");
                error_code = -1;
                casefile = NULL;
        }
        result = mip_get_int (extif_message, "testcasenum=", &caseid);
        if (result != 0) {
                MIN_WARN ("Faulty command: %s", 
                             extif_message->item_line_section_);
                MIN_WARN ("test case id not specified");
                error_code = -1;
                goto FAULT;
        }

        /*
        ** Check if we have the module already instantiated
        */
        if (!strcmp (casefile, "dummy.cfg"))
                conf_found = 1;
        for (work_module_item = dl_list_head (instantiated_modules);
             work_module_item != DLListNULLIterator;
             work_module_item = dl_list_next (work_module_item))
        {
                tm_get_module_filename (work_module_item, name);
                if (strcmp (module, name) == 0) {
                        mod_found = 1;
                        /*
                        ** The module is already instantiated.
                        ** Next check the configuration file.
                        */
                        for (it = dl_list_head (tm_get_cfg_filenames 
                                                (work_module_item));
                             it != DLListNULLIterator;
                             it = dl_list_next (it)) {
                                if (strcmp (dl_list_data (it),
                                            casefile)  == 0) {
                                        conf_found = 1;
                                }
                        }
                        if (conf_found && mod_found)
                                goto MODULE_PRESENT;
                }
                
                
        }

        /*adding module specified in run command here */
        temp_string = NEW2 (char, strlen (casefile) + 1);
        sprintf (temp_string, "%s", casefile);
        /*add specified testcase file only if it is 
        specified as absolute path or exists in lib directories*/
        if (((*temp_string)=='/')||

             (ec_search_lib(temp_string)) != -1){
                dl_list_add (conf_list, (void *)temp_string);
        } else {
                MIN_WARN ("test case file %s not found", temp_string);
        }
#ifdef MIN_EXTIF
	min_if_module_add (module, !strcmp (casefile, "dummy.cfg") ?
			   "" : temp_string);
#else
        ec_add_module (module, conf_list, 0);
#endif
        /*wait for module to return testcases */
MODULE_PRESENT:
        while (cont_flag == 0) {
                usleep (500000);
                i++;
                work_module_item = dl_list_head (instantiated_modules);
                cont_flag = 1;
                while (work_module_item != DLListNULLIterator) {
                        check_result = tm_get_status (work_module_item);
                        if (check_result == TEST_MODULE_READY)
                                cont_flag = cont_flag | 1;
                        else
                                cont_flag = 0;
                        work_module_item = dl_list_next (work_module_item);
                }
                if (i > 20) {
                        MIN_WARN ("timeout on module instantiation");
                        goto FAULT;
                }
        }
        /*Now fetch correct testcase */
        work_module_item = dl_list_head (instantiated_modules);
        while (work_module_item != DLListNULLIterator) {
                work_module =
                    (test_module_info_s *) dl_list_data (work_module_item);
                tm_get_module_filename (work_module_item, module_filename);
                if (strcmp (module_filename, module) == 0)
                        break;
                work_module_item = dl_list_next (work_module_item);
        }

        if (work_module_item == DLListNULLIterator) {
                MIN_WARN ("Specified module not configured");
                error_code = -2;
                goto FAULT;
        }

	/* test case title may contain whitespaces, that's why parsing type
	 * is changed to QuoteStyle. if passing this optional value, it
	 * must be quoted */
	parsing=mip_get_parsing_type(extif_message);
	mip_set_parsing_type (extif_message, EQuoteStyleParsing);
	result = mip_get_string (extif_message, "testcasetitle=", &casetitle);
	mip_set_parsing_type (extif_message, parsing);
        if (result == 0) {
            MIN_INFO ("test case title specified, omitting id");
            
	    work_case_item = dl_list_head (tm_get_tclist (work_module_item));
            while (work_case_item != DLListNULLIterator) {
		    tc_get_title (work_case_item, tc_title);
                    if (strcmp(tc_title, casetitle) == 0) {
                            if (dl_list_size
                                (tm_get_cfg_filenames (work_module_item)) < 2)
                                    break;
                            /*if module has 0 or 1 config file, 
                             * we have already found case */
                            else {
                                    tc_get_cfg_filename (work_case_item,
                                                         casefile_con);
                                    check_result =
                                        strcmp (casefile_con, casefile);
                                    DELETE (casefile_con);
                                    if (check_result == 0)
                                            break;
                                    /*id and testcase file match */
                        	}
                    }
                    work_case_item = dl_list_next (work_case_item);
            }

        } else {
	    MIN_INFO ("test case title not specified, using id");
	    work_case_item = dl_list_head (tm_get_tclist (work_module_item));
            while (work_case_item != DLListNULLIterator) {
                    if (tc_get_id (work_case_item) == caseid) {
                            if (dl_list_size
                                (tm_get_cfg_filenames (work_module_item)) < 2)
                                    break;
                        /*if module has 0 or 1 config file, 
                         * we have already found case */
                            else {
                                    tc_get_cfg_filename (work_case_item,
                                                         casefile_con);
                                    check_result =
                                        strcmp (casefile_con, casefile);
                                    DELETE (casefile_con);
                                    if (check_result == 0)
                                            break;
                                    /*id and testcase file match */
                        	}
                    }
                    work_case_item = dl_list_next (work_case_item);
            }
	}

	if (work_case_item == DLListNULLIterator) {
                MIN_WARN ("Wrong case id");
                error_code = -2;
                goto FAULT;
        }

        result = ec_exec_test_case (work_case_item);
        caseid = dl_list_size (selected_cases);
#ifndef MIN_EXTIF
	work_case_item = dl_list_tail (selected_cases);
	work_case = dl_list_data (work_case_item);
	work_case->ip_slave_case_ = 1;
#endif

        if (result != 0) {
                error_code = -2;
                MIN_WARN ("Error in test case execution");
                goto FAULT;
        }
        ok_to_break = ESFalse;
        send_to_master (caseid, "remote run started");
        return 0;

      FAULT:
        /*test case could not be started for some reason */
        message = NEW2 (char, 30);
        sprintf (message, "remote run error result=%d", error_code);
        send_to_master (0, message);
        DELETE (module);
        DELETE (message);
        DELETE (casefile);
        return 0;
}

/* ------------------------------------------------------------------------- */
/**Function implemens handling of "remote pause" command 
 * @param extif_message - pointer to item parser containing received message.
 *  Assume that get_next_string was called several times, until "pause" 
 * command was extracted
 * @param case_id - dev id extracted from external controller address field
 * @return result of operation - 0 if ok.
 */

LOCAL int handle_remote_pause (MinItemParser * extif_message, int case_id)
{
        return 0;
}

/* ------------------------------------------------------------------------- */
/**Function implemens handling of "remote cancel" command 
 * @param extif_message - pointer to item parser containing received message. 
 * Assume   that get_next_string was called several times, until "cancel" 
 * command was extracted
 * @param case_id - dev id extracted from external controller address field
 * @return result of operation - 0 if ok.
*/
LOCAL int handle_remote_cancel (MinItemParser * extif_message, int case_id)
{
        DLListIterator  work_case_item = DLListNULLIterator;
        int             result = 0;

        work_case_item = dl_list_at (selected_cases, case_id - 1);

        if (work_case_item != DLListNULLIterator) {
                result = ec_abort_test_case (work_case_item);
        } else {
                result = -1;
        }

        if (result == 0) {
                send_to_master (case_id, "remote cancel");
        }
        /* Protocol specification describes only response to successfull 
         * remote pause. If the pause was not successfull, master test 
         * case should fail. Based on that, if remote pause fails for whatever
         * reason, response will not be sent - what should cause master test
         * case fo fail.
         */
        return result;
}


/* ------------------------------------------------------------------------- */
/**Function splits RCP's adress string fields into two ints.
 * @param hex [in] string to be split, has to be 8 characters, otherwise
 *  function fails
 * @param dev_id [out] pointer to int that will hold device id
 * @param case_id [out] pointer to int that will hold case id
 * @return result of operation, 0 if ok
*/

LOCAL int splithex (char *hex, int *dev_id, int *case_id)
{
        char            dev_id_c[5];
        char            case_id_c[5];
        char           *endptr;

        if (strlen (hex) != 8) {
                return -1;
        }

        snprintf (dev_id_c, 5, "%s", hex);
        snprintf (case_id_c, 5, "%s", hex + 4);
        *dev_id = strtol (dev_id_c, &endptr, 16);
        *case_id = strtol (case_id_c, &endptr, 16);

        return 0;
}

/* ------------------------------------------------------------------------- */

/** Function writes text representation of slave adress, in hexadecimal with 
 * leading zeros
 * @param deviceid identifier of device
 * @param caseid id of testcase
 * @return string containing proper representation, or INITPTR 
 * in case of problem.
 */

LOCAL char     *writehex (int devid, int caseid)
{
        char           *hex = NEW2 (char, 9);
        int             val = devid * 0x10000 + caseid;

        if (val > 0xffffffff) {
                DELETE (hex);
        } else {
                sprintf (hex, "%.8x", val);
        }

        return hex;
}

/* ------------------------------------------------------------------------- */

/**Function recognizes message received from external controller and calls 
 * apropriate handler
 * @param extif_message pointer to MinItemParser with parser created
 *  from message string.
 * @return result of operation, 0 if successfull.
 */
LOCAL int min_if_dispatch_extif_msg (MinItemParser * extif_message)
{
        /*strings used to parse fields of RCP message */
        char           *messtype = NULL;

        int             result = 666;

        result = mip_get_string (extif_message, "", &messtype);

        if (result != 0) {
                MIN_WARN ("Could not get EXTIF message type");
                goto DISPATCH_EXIT;
        }

        if (strcasestr (messtype, "reserve") != NULL) {

                result = extif_msg_handle_reserve (extif_message);
                goto DISPATCH_EXIT;
        }

        if (strcasestr (messtype, "release") != NULL) {
                result = extif_msg_handle_release (extif_message);
                goto DISPATCH_EXIT;
        }

        if (strcasestr (messtype, "response") != NULL) {
#ifdef MIN_EXTIF
                result = extif_msg_handle_response (extif_message);
#else
                result = tcp_msg_handle_response (extif_message);

#endif
                goto DISPATCH_EXIT;
        }

        if (strcasestr (messtype, "remote") != NULL) {
                result = extif_msg_handle_command (extif_message);
                goto DISPATCH_EXIT;
        }

      DISPATCH_EXIT:
        DELETE (messtype);

        return result;
}

/* ------------------------------------------------------------------------- */
/**Function handles "response" message coming from external controller
 * @param extif_message pointer to item parser. It is assumed that 
 * mip_get_string was  executed once for this parser to get first "word"
 * @return result of operation, 0 if ok.
 */
LOCAL int extif_msg_handle_response (MinItemParser * extif_message)
{
        char           *command = INITPTR;
        char           *param1 = NULL;
        char           *srcid = INITPTR;
        char           *destid = INITPTR;
        int             result = 666;
        int             retval = 666;
        int             slave_id = 0;
        int             case_id = 0;
        MsgBuffer       ipc_message;
        slave_info     *slave_entry = INITPTR;
        char           *slave_name = NULL;
        DLListIterator  slave_entry_item;

        mip_get_next_string (extif_message, &srcid);

        mip_get_next_string (extif_message, &destid);

        mip_get_next_string (extif_message, &command);

        if (strcasecmp (command, "reserve") == 0) {
                retval =
                    mip_get_next_tagged_int (extif_message, "result=", &result);

                if (retval == -1)
                        mip_get_next_int (extif_message, &result);
                /*result was not tagged int */
                if (retval == -1)
                        result = 0;

                /*it seems that result was not send, assume success */

                /* now find slave entry without device id. Currently
                 * external controller picks first available slave during
                 * allocation, so we do the same */
                slave_entry_item = dl_list_head (ms_assoc);
                while (slave_entry_item != INITPTR) {
                        slave_entry =
                            (slave_info *) dl_list_data (slave_entry_item);
                        MIN_DEBUG (" slave: %d, %s", slave_entry->slave_id_,
				   tx_share_buf (slave_entry->slave_name_));
                        if (slave_entry->slave_id_ == 0) {
                                slave_name = tx_get_buf 
					(slave_entry->slave_name_);
                                break;
                        }
                        slave_entry_item = dl_list_next (slave_entry_item);
                }
                if (slave_name == NULL) {
                        MIN_WARN ("Unrequested response ?");
                        retval = -1;
                        goto out;
                }
                splithex (srcid, &slave_id, &case_id);
                slave_entry->slave_id_ = slave_id;
                if (retval == -1)
                        result = 0;
		slave_entry->status_ = SLAVE_STAT_RESERVED;
                /*it seems that result was not sent, assume success */
                ipc_message.sender_ = ec_settings.engine_pid_;
                ipc_message.receiver_ = own_id;
                ipc_message.type_ = MSG_EXTIF;
                ipc_message.special_ = 0;
                ipc_message.extif_msg_type_ = EResponseSlave;
                ipc_message.param_ = result;
                MIN_DEBUG ("ipc sending with result %d", result);
                mq_send_message (mq_id, &ipc_message);
                DELETE (slave_name);
                retval = 0;
        } else if (strcasecmp (command, "release") == 0) {
                retval =
                    mip_get_next_tagged_int (extif_message, "result=", &result);
                if (retval == -1)
                        mip_get_next_int (extif_message, &result);
                /*result was not tagged int */
                if (retval == -1)
                        result = 0;
                /*it seems that result was not send, assume success */
                ipc_message.sender_ = ec_settings.engine_pid_;
                ipc_message.receiver_ = own_id;
                ipc_message.type_ = MSG_EXTIF;
                ipc_message.param_ = result;
                ipc_message.special_ = 0;
                ipc_message.extif_msg_type_ = EResponseSlave;
                MIN_DEBUG ("ipc sending with result %d", result);
                mq_send_message (mq_id, &ipc_message);
                /*slave released, remove it from ms_assoc */
                splithex (srcid, &slave_id, &case_id);
                slave_entry_item = dl_list_head (ms_assoc);
                while (slave_entry_item != DLListNULLIterator) {
			
                        slave_entry =
                            (slave_info *) dl_list_data (slave_entry_item);
                        if (slave_entry->slave_id_ == slave_id) {
				tx_destroy (&slave_entry->slave_name_);
                                DELETE (slave_entry);
                                dl_list_remove_it (slave_entry_item);
                                break;
                        }

                }
                retval = 0;
        } else if (strcasecmp (command, "remote") == 0) {
                slave_entry_item = dl_list_head (ms_assoc);
                while (slave_entry_item != DLListNULLIterator) {
                        slave_entry =
                            (slave_info *) dl_list_data (slave_entry_item);
                        if (slave_entry->slave_id_ == slave_id) 
                                break;
                        

                }
                retval = 0;

                mip_get_next_string (extif_message, &command);
                if (strcasecmp (command, "run") == 0) {
                        splithex (srcid, &slave_id, &case_id);
                        /*translate received caseid into proper 
                        number that can be stored in scripter*/
                        case_id = (slave_id<<16) + case_id;
                        mip_get_next_string (extif_message, &param1);
                        if (strcasecmp (param1, "started") == 0) {
                                ipc_message.sender_ = ec_settings.engine_pid_;
                                ipc_message.receiver_ = own_id;
                                ipc_message.type_ = MSG_EXTIF;
                                ipc_message.special_ = case_id;
                                ipc_message.param_ = 0;
                                ipc_message.extif_msg_type_ = EResponseSlave;
                                MIN_DEBUG ("ipc sending with result 0");
                                mq_send_message (mq_id, &ipc_message);
                                retval = 0;
                        } else if (strcasecmp (param1, "ready") == 0) {
                                mip_get_int (extif_message, "result=", &result);
                                ipc_message.sender_ = ec_settings.engine_pid_;
                                ipc_message.receiver_ = own_id;
                                ipc_message.type_ = MSG_EXTIF;
                                ipc_message.special_ = case_id;
                                ipc_message.param_ = result;
                                ipc_message.extif_msg_type_ =
                                    ERemoteSlaveResponse;
                                MIN_DEBUG ("ipc sending with result %d",
                                             result);
                                mq_send_message (mq_id, &ipc_message);
                                retval = 0;
                        } else if (strcasecmp (param1, "error") == 0) {
                                mip_get_int (extif_message, "result=", &result);
                                ipc_message.sender_ = ec_settings.engine_pid_;
                                ipc_message.receiver_ = own_id;
                                ipc_message.type_ = MSG_EXTIF;
                                ipc_message.special_ = case_id;
                                ipc_message.param_ = result;
                                ipc_message.extif_msg_type_ = EResponseSlave;
                                MIN_DEBUG ("ipc sending with result %d",
					   result);
                                mq_send_message (mq_id, &ipc_message);
                                retval = 0;
                        }
                } else if (strcasecmp (command, "request") == 0) {
                        retval =
                            handle_remote_event_request_resp (extif_message);
                } else if (strcasecmp (command, "release") == 0)
                        /*TEMPORARY SOLUTION, INVESTIGATE EVENT SYSTEM */
                        retval = 0;
        }
      out:
        DELETE (command);
        DELETE (param1);
        DELETE (srcid);
        DELETE (destid);

        return retval;
}

/* ------------------------------------------------------------------------- */
/** Function handles external controller "reserve" message
 * @param extif_message pointer to item parser. It is assumed that 
 * mip_get_string was executed once for this parser to get first "word"
 * @return result of operation, 0 if ok.
 */
LOCAL int extif_msg_handle_reserve (MinItemParser * extif_message)
{
        char            rtype[11];

        /*change "complete case" callback */
        pthread_mutex_lock (&tec_mutex_);
        ok_to_break = ESFalse;
#ifdef MIN_EXTIF
        original_complete_callback = tfwif_callbacks.complete_callback_;
        MIN_DEBUG ("old callback = %x ", tfwif_callbacks.complete_callback_);
        MIN_DEBUG ("master report = %x ", master_report);
        tfwif_callbacks.complete_callback_ = master_report;
        MIN_DEBUG ("new callback = %x ", tfwif_callbacks.complete_callback_);
#endif
        pthread_mutex_unlock (&tec_mutex_);
        sprintf (rtype, "reserve 0");
        send_to_master (0, rtype);

        return 0;
}

LOCAL slave_info *find_slave_by_he (struct hostent *he, DLListIterator *itp)
{
       DLListIterator it;
       slave_info *ips;
 
       *itp = INITPTR;

       for (it = dl_list_head (ms_assoc); it != INITPTR;
            it = dl_list_next (it)) {
               ips = dl_list_data (it);
               if (!memcmp (he, &ips->he_, sizeof (struct hostent)))  {
                       *itp = it;
                       return ips;
               }
       }
       
       return INITPTR;
}

/* ========================  FUNCTIONS ===================================== */

/*---------------------------------------------------------------------------*/
/** Function called to intitialize rcp handling
 */
void rcp_handling_init ()
{
	ms_assoc = dl_list_create();
	EXTIF_received_data = dl_list_create();
}
/*---------------------------------------------------------------------------*/
/** Clean up
 */
void rcp_handling_cleanup ()
{
        DLListIterator  work_slave_item;
        DLListIterator  work_data_item;
        received_data  *work_data_entry;

        work_slave_item = dl_list_head (ms_assoc);
        while (work_slave_item != DLListNULLIterator) {
                dl_list_remove_it (work_slave_item);
                work_slave_item = dl_list_head (ms_assoc);
        }
        dl_list_free (&ms_assoc);

        work_data_item = dl_list_head (EXTIF_received_data);
        while (work_data_item != DLListNULLIterator) {
                work_data_entry =
                    (received_data *) dl_list_data (work_data_item);
                DELETE (work_data_entry);
                dl_list_remove_it (work_data_item);
		work_data_item = dl_list_head (EXTIF_received_data);
        }
        dl_list_free (&EXTIF_received_data);

}

/* ------------------------------------------------------------------------- */
/** Function called to send external controller message to master device.
 * @param tc_id id of slave's test case
 * @param msg NULL-terminated string containing body of message 
 *            (everything following adresses)
 */
void send_to_master (int tc_id, char *msg)
{

        char           *hex;

	if (tc_id != 0) {
	      tc_id = dl_list_size (selected_cases);
	}
        hex = writehex (own_id, tc_id);
	in->send_rcp ("response", hex, "deadbeef", msg, 0);

        DELETE (hex);
}

/*---------------------------------------------------------------------------*/
/** Function called to send external controller message to slave device.
 * @param slave_name NULL-terminated string containing slave's name
 * @param tc_id id of slave's test case
 * @param msg NULL-terminated string containing body of message 
 *        (everything following adresses)
 */
void
send_to_slave (TMSCommand command, char *slave_name, int tc_id, char *message)
{
        DLListIterator  slave_entry_item = DLListNULLIterator;
        slave_info     *slave_entry = INITPTR;
        int             slave_id = 0;
        char           *hex;

        slave_entry_item = dl_list_head (ms_assoc);
        while (slave_entry_item != DLListNULLIterator) {
                slave_entry = (slave_info *) dl_list_data (slave_entry_item);
                if (strcmp (slave_name, 
			    tx_share_buf (slave_entry->slave_name_)) == 0) {
                        slave_id = slave_entry->slave_id_;
                        break;
                }
                slave_entry_item = dl_list_next (slave_entry_item);
        }
        /* scripter keeps tc_id as a combination of slave id and testcase 
         * number (slave number as last 4 digits), test case id needs to 
         * be stripped if given at all. Not used in current implementation, 
         * since all calls to this function give 0 in this argument so far, 
         * but it might prevent problems in in case someone wants to expand 
         * functionality.*/
        if (tc_id != 0){
                tc_id = tc_id&0xffff; /*to strip slave id*/
        }
        hex = writehex (slave_id, tc_id);
        switch (command) {
        case EAllocateSlave:
		sleep (1);
		in->send_rcp ("reserve", "deadbeef", hex, message, 
			      slave_entry->fd_);
                break;
        case EFreeSlave:
		in->send_rcp ("release", "deadbeef", hex, message,
			      slave_entry->fd_);
                break;
        case ERemoteSlave:
		in->send_rcp ("remote", "deadbeef", hex, message,
			      slave_entry->fd_);
                break;
        default:
                break;
        }
        DELETE (hex);
}

/* ------------------------------------------------------------------------- */
/**Handler for sendreceive ipc message from slave
 */
int ec_msg_sndrcv_handler (MsgBuffer * message)
{
        char           *hex;
	Text *tx;

	tx = tx_create (message->message_);
	tx_c_append (tx, "=");
	tx_c_append (tx, message->desc_);
        hex = writehex (own_id, 0);
	in->send_rcp ("remote", hex, "deadbeef", tx_share_buf(tx), 0);
	
	tx_destroy (&tx);
        DELETE (hex);
        return 0;
}

/* ------------------------------------------------------------------------- */
/** Handler for ipc external controller message,
 * used in master/slave scenarios (hence _ms_);
 * since those in turn have several "subtypes",
 * function reads type and calls apropriate handlers.
 * @param message MsgBUffer containing received IPC message
 * @return result of the operation
*/
int ec_msg_ms_handler (MsgBuffer * message)
{
        char           *extifmessage = NULL;
        char           *param1 = NULL;
        char           *param2 = NULL;
        char           *param3 = NULL;
        char           *param4 = NULL;
 	char           *param5 = NULL;
        TScripterKeyword kw;
        TParsingType   parsing=ENormalParsing;

        MinItemParser *params = INITPTR;
        slave_info     *slave_entry = INITPTR;


        extifmessage = NEW2 (char, strlen (message->message_) + 255);

        switch (message->extif_msg_type_) {
        case EAllocateSlave:
                /* since only master test case can send this message,
                   we will set own_id of this min instance to PID of this 
                   TMC, so that we can forward IPC messages correctly */

                own_id = message->sender_;
#ifdef MIN_EXTIF
                slave_entry = NEW (slave_info);
		slave_entry->slave_name_ = tx_create (message->message_);
                slave_entry->slave_id_ = 0;
                dl_list_add (ms_assoc, (void *)slave_entry);
#else
		if (allocate_ip_slave (message->desc_, own_id)) {
			MIN_FATAL ("slave allocation failed");
			DELETE (extifmessage);
			return -1;
		}
#endif
                send_to_slave (EAllocateSlave, message->message_, 0,
			       message->desc_);
                break;

        case EFreeSlave:

                send_to_slave (EFreeSlave, message->message_, 0, "");
                break;

        case ERemoteSlave:
                MIN_DEBUG ("MESSAGE: %s\n", message->message_);
                MIN_DEBUG ("DESC: %s\n", message->desc_);
                params =
                    mip_create (message->desc_, 0,
                                strlen (message->desc_) + 1);
                mip_get_string (params, "", &param1);


                kw = get_keyword (param1);
                switch (kw) {
                case EKeywordRun:
                        mip_get_next_string (params, &param2);
                        mip_get_next_string (params, &param3);
                        mip_get_next_string (params, &param4);
			/* parsing is set to QuoteStyle, because test case
			 * title may contain whitespaces */
			parsing=mip_get_parsing_type (params);
			mip_set_parsing_type (params, EQuoteStyleParsing);
			/* if title is specified, it's passed in here... */
			if(mip_get_next_string (params, &param5) == 0){
                        	sprintf (extifmessage, "run module=%s "
                                     "testcasefile=%s testcasenum=%s "
				     "testcasetitle=\"%s\"", param2,
                                     param3, param4, param5);
			/* if it's not, id is used for compatibility */
			} else { 
                        	sprintf (extifmessage, "run module=%s "
                                     "testcasefile=%s testcasenum=%s", param2,
                                     param3, param4);
			}
        		mip_set_parsing_type (params, parsing);

                        break;
                case EKeywordSet:
                        mip_get_next_string (params, &param2);
                        sprintf (extifmessage, "set %s", param2);
                        if (mip_get_next_string (params, &param3) == ENOERR)
                                sprintf (extifmessage + strlen (extifmessage),
                                         " type=%s", param3);
                        break;
                case EKeywordRequest:
                        mip_get_next_string (params, &param2);
                        sprintf (extifmessage, "request %s", param2);
                        if (mip_get_next_string (params, &param3) == ENOERR)
                                sprintf (extifmessage + strlen (extifmessage),
                                         " type=%s", param3);
                        break;
                case EKeywordExpect:
                        mip_get_next_string (params, &param2);
                        tec_ms_handle_data_request (get_id_from_slavename
                                                    (message->message_),
                                                    param2, message->sender_);
                        DELETE (extifmessage);
                        return 0;
                        break;
                case EKeywordRelease:
                case EKeywordWait:
                case EKeywordUnset:

                default:
                        strcpy (extifmessage, message->desc_);
                        break;
                }
                send_to_slave (ERemoteSlave, message->message_, 0,
                               extifmessage);
                break;
        default:
                MIN_DEBUG ("MESSAGE: %s\n", message->message_);
                MIN_DEBUG ("DESC: %s\n", message->desc_);
                break;
        }

        DELETE (extifmessage);
        DELETE (param1);
        DELETE (param2);
        DELETE (param3);
        DELETE (param4);
        DELETE (param5);
        mip_destroy (&params);

        return 0;
}

/* ------------------------------------------------------------------------- */
/** Function reads message received from external controller and 
 * calls  handling
 */
int tec_extif_message_received (char *message, int length)
{
        MinItemParser *extif_message = INITPTR;
        int             result = 666;

        MIN_DEBUG ("tec_extif_message_received: length %d", length);
        MIN_DEBUG ("EXTIF SAYS: %s", message);
        extif_message = mip_create (message, 0, length);
        if (extif_message != INITPTR) {
                result = min_if_dispatch_extif_msg (extif_message);
                mip_destroy (&extif_message);
        } else {
                result = -1;
                MIN_WARN ("Could not create item parser object");
        }
        MIN_DEBUG (" RETURNING: %d ", result);

        return result;
}

/* ------------------------------------------------------------------------- */
/** Function that replaces "test case complete" callback, if min instance 
 * is used as slave.
 * @param run_id runtime id of test case (position in "selected cases" list)
 * @param execution_result indicates if starting of test case was successful
 * @param test_result result of test
 * @param desc description (taken from ipc message);
 */
void
master_report (int run_id, int execution_result, int test_result, char *desc)
{
        DLListIterator  finished_case_item = DLListNULLIterator;
        DLListIterator  work_module_item = DLListNULLIterator;
        char           *extifmessage;

        MIN_DEBUG ("MIN CALLBACK");
        /*let's get module pid from finished case */
        finished_case_item = dl_list_at (selected_cases, run_id);
        work_module_item = tc_get_test_module_ptr (finished_case_item);
        extifmessage = NEW2 (char, 30);
        sprintf (extifmessage, "remote run ready result=%d", test_result);
        send_to_master (run_id + 1, extifmessage);
#ifdef MIN_EXTIF
        original_complete_callback (run_id, execution_result, test_result,
                                    desc);
#endif

        /*Revert to "normal" external controller communincation if 
	 * it is allowed, otherwise mark that it's allowed to revert. */
        if (ok_to_break == ESTrue) {
#ifdef MIN_EXTIF
                MIN_DEBUG ("Restoring Callback");
                tfwif_callbacks.complete_callback_ = original_complete_callback;
#else
		;
#endif
        } else
                ok_to_break = ESTrue;
        DELETE (extifmessage);
}

/* ------------------------------------------------------------------------- */
/** Adds a new entry to pool of ip slaves
 *  @param he host address information
 *  @param slavetype type of the slave e.g. "phone"
 *  @return 0 on success, 1 on error
 */
int tec_add_ip_slave_to_pool (struct hostent *he, char *slavetype)
{
       slave_info *slave;
       DLListIterator it;
       
       if (find_slave_by_he (he, &it) != INITPTR) {
               MIN_WARN ("Slave already registered");
               return 1;
       }
       slave = NEW(slave_info);
       slave->status_ = SLAVE_STAT_FREE;
       slave->slave_id_ = 0;
       memcpy (&slave->he_, he, sizeof (struct hostent));
       slave->slave_type_ = tx_create (slavetype);
       slave->slave_name_ = INITPTR;
       slave->fd_ = -1;
       slave->write_queue_ = dl_list_create ();
       
       dl_list_add (ms_assoc, slave);

       return 0;
}


/* ------------------------------------------------------------------------- */
/** Deletes entry from the pool of ip slaves
 *  @param he host address information
 *  @param slavetype type of the slave e.g. "phone"
 *  @return 0 on success, 1 on error
 */
int tec_del_ip_slave_from_pool (struct hostent *he, char *slavetype)
{
       slave_info *slave;
       DLListIterator it;

       if (ms_assoc == INITPTR)
               return 1;
       
       slave = find_slave_by_he (he, &it); 
       if (slave == INITPTR) {
               MIN_WARN ("slave not found");
               return 1;
       }
       
       dl_list_remove_it (it);
       
       if (slave->status_ & SLAVE_STAT_RESERVED) {
               MIN_WARN ("slave still reserved");
       }

       tx_destroy (&slave->slave_name_);
       tx_destroy (&slave->slave_type_);

       /* FIXME flush write queue */

       DELETE (slave);

       return 0;
}

void
tcp_master_report (int run_id, int execution_result, int test_result, 
		   char *desc)
{
        DLListIterator  finished_case_item = DLListNULLIterator;
        DLListIterator  work_module_item = DLListNULLIterator;
        char           *extifmessage;

        /*let's get module pid from finished case */
        finished_case_item = dl_list_at (selected_cases, run_id);
        work_module_item = tc_get_test_module_ptr (finished_case_item);
        extifmessage = NEW2 (char, 30);
        sprintf (extifmessage, "remote run ready result=%d", test_result);
        send_to_master (run_id + 1, extifmessage);
        DELETE (extifmessage);
}


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST


#endif                          /* MIN_UNIT_TEST */

/* End of file */
