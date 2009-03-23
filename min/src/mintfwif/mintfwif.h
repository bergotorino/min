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
 *  @file       mintfwif.h
 *  @version    0.1
 *  @brief      This file contains header file of the mintfwif
 */

#ifndef MINTFWIF_H
#define MINTFWIF_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#if 0
#include <tec.h>
#include <tec_rcp_handling.h>
#include <min_common.h>
#endif
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/** Test case complete callback function */
typedef void    (*min_case_complete_func) (int run_id, int execution_result,
					   int test_result, char *desc);
/** Test case print callback function */
typedef void    (*min_case_print_func) (int run_id, char *text);
/** External controller message sending function */
typedef void    (*min_extif_message_cb_) (char *message, int length);

/** Container for callbacks */
typedef struct {
        min_case_complete_func complete_callback_;
        min_case_print_func print_callback_;
        min_extif_message_cb_ send_extif_msg_;
} tfwif_callbacks_s;

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */
/* DLList         *user_selected_cases; */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */

/** Structure for test caser data storage*/
typedef struct {
        /**test case id, unique in module scope*/
        int             case_id_;
        /**title of test case - NULL terminated string*/
        char            case_name_[256];
} min_case;
/* ------------------------------------------------------------------------- */

/** Structure for storage of test module data*/
typedef struct {
        /** Module name - null terminated string */
        char            module_name_[128];
        /** Number of test cases */
        unsigned int    num_test_cases_;
        /** Array of min_case structures - collection of test cases
        in module*/
        min_case      *test_cases_;
} module_info;

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/**Function used to open min test framework
 * @param engine_ini path to engine settings file, provided by external 
 * controller; file can contain engine options and module definitions;
 * can be NULL if parameter not specified
 * @param complete_cb test case complete callback function
 * @param print_cb test case print callback function
 * @param extifsend_cb "external controller send" callback function
 * @param envp pointer to environment variables as in main(arc,argv,envp)
 * @return result of operation : 0 if success, -1 in case of error
*/
int             min_if_open (min_case_complete_func complete_cb,
                             min_case_print_func print_cb,
                             min_extif_message_cb_ extifsend_cb,
                             char *engine_ini, char *envp[]);

/**Function used to close min test framework
 * @return result of operation : 0 if success, -1 in case of error
*/
int             min_if_close ();

/* ------------------------------------------------------------------------- */
/** Function used to fetch test modules and test cases info from min engine
 * @param modules pointer to module_info structures, if function completes 
 * successfully, this will point to array holding fetched data, in case of 
 * failure it will be set to NULL
 * @return number of fetched modules if function is successfull, otherwise  
*/
int             min_if_get_cases (module_info ** modules);

/**Function used to execute selected test case
 * @param module - name of module
 * @param id identifier of test case
 * @return runtime identifier of the test case, -1 in case of error
*/
int             min_if_exec_case (char *module, unsigned int id);

/**Function used to cancel selected test case
 * @param runtime_id runtime identifier of test case, returned 
 * by min_if_exec_case
 * @return result of operation : 0 if test case execution was cancelled, 
 * -1 if test case is already cancelled or if case is not running, 
 * -3 if invalid argument passed, -4 in case of unspecified internal error
*/
int             min_if_cancel_case (unsigned int runtime_id);


/**Function used to pause selected test case
 * @param runtime_id runtime identifier of test case, returned by 
 * min_if_exec_case
 * @return result of operation : 0 if test case execution was paused, 
 * -1 if test case is already paused, -2 if case is not running, 
 * -3 if invalid argument passed, -4 in case of unsepcified internal error
*/
int             min_if_pause_case (unsigned int runtime_id);

/**Function used to resume execetution of previously paused test case
 * @param runtime_id runtime identifier of test case, returned by 
 * min_if_exec_case
 * @return result of operation : 0 if operation was successful, 
 * -1 if test case is running (was not paused ) ,
 * -2 if case is not running (not started or already finished), 
 * -3 if invalid argument was passed, 
 * -4 in case of unspecified internal error
*/
int             min_if_resume_case (unsigned int runtime_id);

/** Function called when message received from external controller
 * @param message string containing actual message
 * @param length length of message (this param will probobly be dropped
 * @return result of operation
*/
int             min_if_message_received (char *message, int length);

/**Function used to set device_id for external controller
 * @param device_id value of device id assigned by external controller
 * @return result of operation - always 0, since only failure
 * that can occur in this function will be indicatet by signals,
 * e.g segmentation fault, abort and such.
*/
int             min_if_set_device_id (int device_id);

/**Function used to handle add_module command from external controller
 * @param module_name string with module name (NULL terminated string)
 * @param conf_name name of config file, can be NULL if no testcase
 * file given.
 * @return result of operation - 0 if all was ok,
 * -1 if module not found . NOTE: this function
 * only writes module name to variuable storing data of module
 * to be added, and checks if actual file exists.
 * Actual "instantiation" of module is done in min_if_get_cases
*/
int             min_if_module_add (char *module_name, char *conf_name);

#endif                          /* MINTFWIF_H */
/* End of file */
