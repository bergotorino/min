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
 *  @file       min_engine_api.h
 *  @version    0.1
 *  @brief      
 */

#ifndef MIN_ENGINE_API_H
#define MIN_ENGINE_API_H

/* ----------------------------------------------------------------------------
 * INCLUDES 
 */
/* ----------------------------------------------------------------------------
 * CONSTANTS
 */
/* ----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 */
/* ----------------------------------------------------------------------------
 * MACROS
 */

#define MINAPI_PLUGIN_CALL(fname,fcall) \
        do { \
                DLListIterator it = dl_list_head(get_plugin_list()); \
                while (it!=DLListNULLIterator) { \
                        if ( ((eapiIn_t*)dl_list_data(it))->fname ) { \
                                ((eapiIn_t*)dl_list_data(it))->fcall; \
                        } \
                        it = dl_list_next(it); \
                } \
        } while(0);
/* ----------------------------------------------------------------------------
 * DATA TYPES
 */
/* ---------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 */
/* ----------------------------------------------------------------------------
 * Structures
 */

/** Callbacks from plugin called by Engine */
typedef struct  {

	/* --------------------------------------------------------------------
	** MIN engine calls this for every new test module it instantiates.
	** The module_id uniquelly identifies the test module.
	*/
        void (*new_module) (char *modulename, unsigned module_id);

	/* --------------------------------------------------------------------
	** In case of error loading a module MIN engine calls this one.
	*/
        void (*no_module) (char *modulename);

	/* --------------------------------------------------------------------
	** MIN engine calls this when all the test cases found from test module 
	** identified by module_id are reported (with new_case())
	*/
        void (*module_ready) (unsigned module_id);

	/* --------------------------------------------------------------------
	** Each test case found from test module. The module_id identifies the 
	** test module, case_id is unique within the test module.
	*/
        void (*new_case) (unsigned module_id, unsigned case_id, 
			  char *casetitle);
	/* --------------------------------------------------------------------
	** MIN engine reports each started test case. The test_run_id 
	** identifies the test run. The identifier is to be considred valid 
	** while test case is running (ongoing/paused).   
	*/
        void (*case_started) (unsigned module_id, unsigned case_id, 
			      long test_run_id);

	/* --------------------------------------------------------------------
	** MIN engine reports that a test case identified with test_run_id has
	** been paused.
	*/
        void (*case_paused) (long test_run_id);

	/* --------------------------------------------------------------------
	** MIN engine reports that a test case identified with test_run_id has
	** been resumed.
	*/
        void (*case_resumed) (long test_run_id);

	/* --------------------------------------------------------------------
	** MIN engine reports test case result.
	** Values for result as in TPResult (min_common.h)
	*/
        void (*case_result) (long test_run_id, int result, char *desc,
			     long starttime, long endtime);

       	/* --------------------------------------------------------------------
	** Prints from test case (tm_printf()) are reported by this function. 
	*/
        void (*module_prints) (long test_run_id, char *message);

	/* --------------------------------------------------------------------
	** Lists all *.so seen in the MIN engine search paths.
	*/
        void (*test_modules) (char* modules);

	/* --------------------------------------------------------------------
	** Lists all text files seen in the MIN engine search paths.
	*/
        void (*test_files) (char* modules);

	/* --------------------------------------------------------------------
	** Engine reports fatal errors
	*/
        void (*error_report) (char* error);
        
	/* --------------------------------------------------------------------
	** Send RCP
	*/
        void (*send_rcp) (char* message, int length);

} eapiIn_t;

/** Callbacks to functions from engine called by plugin */
typedef struct {
	/* --------------------------------------------------------------------
	** Adds a new test module to MIN. Engine calls new_module() if the 
	** adding is succesfull.
	** returns 0 if the async operation was started succesfully
	**         1 in case of error
	*/
        int (*add_test_module) (char *modulepath);

	/* --------------------------------------------------------------------
	** Adds a test case file to the test module identified by moduleid. 
	** Note: This must be called at least once for every module added(
	** with add_test_module()), with the testcasefile parameter as "\0". So
	** that engine knows not to expect any more files. 
	** returns 0 if the async operation was started succesfully
	**         1 in case of error
	*/
        int (*add_test_case_file) (unsigned moduleid, char *testcasefile);

	/* --------------------------------------------------------------------
	** Signals engine to start executing test case with caseid from module 
	** with moduleid. If the groupid is non-zero test case is started when
	** there are no cases running with the same groupid. (sequential
	** execution)
	** returns 0 if the async operation was started succesfully
	**         1 in case of error
	*/
        int (*start_case) (unsigned moduleid, unsigned caseid, 
			    unsigned groupid);
	/* --------------------------------------------------------------------
	** Pause test case
	** returns 0 if the async operation was started succesfully
	**         1 in case of error
	*/
        int (*pause_case) (long test_run_id);

	/* --------------------------------------------------------------------
	** Resume test case
	** returns 0 if the async operation was started succesfully
	**         1 in case of error
	*/
        int (*resume_case) (long test_run_id);

	/* --------------------------------------------------------------------
	** Abort test case
	** returns 0 if the async operation was started succesfully
	**         1 in case of error
	*/
        int (*abort_case) (long test_run_id);

	/* --------------------------------------------------------------------
	** Reports fatal error to engine
	** returns 0 always
	*/
        int (*fatal_error) (const char *what, const char *error_string);

	/* --------------------------------------------------------------------
	** Open interface towars the MIN engine
	** returns number of available modules 
	*/
        int (*min_open) ();

	/* --------------------------------------------------------------------
	** Close the interface
	** returns 0 on success 1 on error
	*/
        int (*min_close) ();

	/* --------------------------------------------------------------------
	** Triggers sending a list of test modules ( test_modules() )
	** returns 0 on success 1 on error
	*/
        int (*query_test_modules) ();

	/* --------------------------------------------------------------------
	** Triggers sending a list of test files ( test_files() ) 
	** returns 0 on success 1 on error
	*/
        int (*query_test_files) ();
	/* --------------------------------------------------------------------
	** Send RCP messages towards the engine
	** returns 0 on success 1 on error
	*/
	int (*receive_rcp) (char *message, int length);
	/* --------------------------------------------------------------------
	** Register slave device
	**
	** returns 0 on success 1 on error
	*/
	int (*register_slave) (char *host, char *slavetype);
} eapiOut_t;

/* ----------------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 */
/** Initializes the Engine API - binds pointers to functions to callback
 *  functions.
 */
void eapi_init (eapiIn_t *in, eapiOut_t *out);

/** Gets pointer to the plugin list. Used by the MINAPI macro
 */
DLList* get_plugin_list();

DLListIterator* get_it();
/* -------------------------------------------------------------------------*/

#endif                          /* MIN_ENGINE_API_H */

/* End of file */
