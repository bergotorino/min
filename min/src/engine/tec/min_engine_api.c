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
 *  @file       min_engine_api.c
 *  @version    0.1
 *  @brief      MIN Engine API 
 */

/* ----------------------------------------------------------------------------
 * INCLUDE FILES
 */
#include <dllist.h>
#include <tec.h>
#include <tec_rcp_handling.h>
#include <min_logger.h>
#include <data_api.h>
#include <min_engine_api.h>
#include <min_settings.h>

#include <dirent.h>
#include <netdb.h>
#include <sys/socket.h>
#include <assert.h>
/* ----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 */
DLList *modules; /** List of modules added through this API */
DLList *plugins = INITPTR; /** List of plugins */
static int open; /** Flag stating whether the API is open */
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
/* ------------------------------------------------------------------------- */
LOCAL DLListIterator tc_find_by_runid (DLList * list_handle, long runid);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_add_test_module (char *modulepath);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_add_test_case_file (unsigned module_id, char *testcasefile);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_start_test_case (unsigned module_id, unsigned case_id, 
				unsigned groupid);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_pause_case (long test_run_id);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_resume_case (long test_run_id);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_abort_case (long test_run_id);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_error (const char *what, const char *msg);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_open ();
/* ------------------------------------------------------------------------- */
LOCAL int eapi_close ();
/* ------------------------------------------------------------------------- */
LOCAL DLListIterator tc_find_by_runid (DLList * list_handle, long runid);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_query_test_modules(char **modulelist);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_query_test_files(char **filelist);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_receive_rcp (char *message, int length);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_register_slave (char *host, char *slavetype);
/* ------------------------------------------------------------------------- */
LOCAL int eapi_debug_test_case (unsigned module_id, unsigned case_id);
/* -------------------------------------------------------------------------
 * FORWARD DECLARATIONS
 */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Add test module to engine .
 *  @param modulepath path to test module.
 *  @return 1 on error, 0 on success
 */
LOCAL int eapi_add_test_module (char *modulepath)
{
        test_module_info_s *modinfo = INITPTR;
	int ret = 1;

        modinfo = tm_create (modulepath, INITPTR, 0);
        if (tm_add (modules, modinfo) != INITPTR) {
		MIN_DEBUG ("Number of plugins %d", 
			   dl_list_size (get_plugin_list()));
                MINAPI_PLUGIN_CALL(new_module,new_module (modulepath,
							  modinfo->module_id_));
		ret = 0;
        } else {
                MIN_WARN ("failed to add module");
                MINAPI_PLUGIN_CALL(no_module,no_module(modulepath));
        }
                
        return ret;
}
/* ------------------------------------------------------------------------- */
/** Add test case file to module.
 *  @param module_id module identifier.
 *  @param testcasefile test case file name.
 *  @return 0 on success, 1 on error.
 */
LOCAL int eapi_add_test_case_file (unsigned module_id, char *testcasefile)
{
        DLListIterator it;
        test_module_info_s *modinfo;
        char *p;

        it = tm_find_by_module_id (modules, module_id);

        if (it == INITPTR) {
                MIN_WARN ("No module with id %u found", module_id);
                return 1;
        }
        modinfo = dl_list_data (it);
        if (*testcasefile != '\0') {
                p =  NEW2 (char, strlen (testcasefile) + 1);
                STRCPY (p, testcasefile, strlen (testcasefile) + 1);
		if (modinfo->cfg_filename_list_ == INITPTR)
		        modinfo->cfg_filename_list_ = dl_list_create();
                dl_list_add (modinfo->cfg_filename_list_, p);
        } else {
                /* we have all the test case files for the module,
                   so we can add the module into engine */
                if(ec_add_module (modinfo->module_filename_, 
				  modinfo->cfg_filename_list_,
				  modinfo->module_id_,
				  0))
			return 1;
        } 
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Start test case execution.
 *  @param module_id test module identifierm
 *  @param case_id test case identifier.
 *  @param groupid test group id.
 *  @return 0 on success, 1 on error.
 *
 *  Start executing test case identified by module_id and case_id. If groupid 
 *  is greater than zero, check first if there is ongoing case with the same
 *  groupid. 
 */
LOCAL int eapi_start_test_case (unsigned module_id, unsigned case_id, 
				unsigned groupid)
{
        int             result = 0, run = 1, stat;
	DLListIterator mod_it, case_it, case_it2;
        test_module_info_s *module;
	
        pthread_mutex_lock (&tec_mutex_);

	mod_it = tm_find_by_module_id (instantiated_modules, module_id);
	if (mod_it == INITPTR) {
		MIN_WARN ("No module by id %d found", module_id);
		pthread_mutex_unlock (&tec_mutex_);
		return 1;
	}
	module = (test_module_info_s *)dl_list_data (mod_it);
	case_it = tc_find_by_id (module->test_case_list_, case_id);
	if (case_it == INITPTR) {
		MIN_WARN ("No case by id %d found", case_id);
		pthread_mutex_unlock (&tec_mutex_);
		return 1;
	}

	/* if we have a groupid, check if there already is an ongoing case
	 * with the id */
	if (groupid != 0) {
		for (case_it2 = dl_list_head (selected_cases);
		     case_it2 != INITPTR;
		     case_it2 = dl_list_next (case_it2)) {
			if (tc_get_group_id (case_it2) == groupid) {
				stat = tc_get_status (case_it2); 
				if (stat == TEST_CASE_ONGOING || 
				    stat == TEST_CASE_PAUSED) {
					run = 0;
					break;
				}
				
			}
		}
	}
	pthread_mutex_unlock (&tec_mutex_);
	case_it = ec_select_case (case_it, groupid);

	if (run)
		result = ec_exec_case (case_it);

        return result;
}
/* ------------------------------------------------------------------------- */
/** Pause test case.
 *  @param test_run_id identifier for the ongoing case.
 *  @return 0 on success, 1 on error.
 */
LOCAL int eapi_pause_case (long test_run_id)
{
	DLListIterator it;

	it = tc_find_by_runid (selected_cases, test_run_id);
	if (it != INITPTR)
		return ec_pause_test_case (it);
	return 1;
}
/* ------------------------------------------------------------------------- */
/** Resume test case.
 *  @param test_run_id runtime identifier for the paused case.
 *  @return 0 on success, 1 on error.
 */
LOCAL int eapi_resume_case (long test_run_id)
{
	DLListIterator it;

	it = tc_find_by_runid (selected_cases, test_run_id);
	if (it != INITPTR)
		return ec_resume_test_case (it);
	return 1;
}

/* ------------------------------------------------------------------------- */
/** Abort test case.
 *  @param test_run_id runtime identifier for the ongoing case.
 *  @return 0 on success, 1 on error.
 */
LOCAL int eapi_abort_case (long test_run_id)
{
	DLListIterator it;

	it = tc_find_by_runid (selected_cases, test_run_id);
	if (it != INITPTR)
		return ec_abort_test_case (it);
	return 1;
}

/* ------------------------------------------------------------------------- */
/** Print error message to log. 
 *  @param what high level error message.
 *  @param msg  detailed message.
 *  @return 0 always
 */
LOCAL int eapi_error (const char *what, const char *msg)
{
        MIN_FATAL ("%s - %s",what,msg);
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Opens the Engine API. Runs ec_configure() and starts all the modules.
 *  @return number of started modules.
 */ 
LOCAL int eapi_open ()
{
	MIN_DEBUG ("Opening");
	//assert (open == 0);
	open = 1;
	ec_configure ();
	return ec_start_modules();
}

/* ------------------------------------------------------------------------- */
/** Closes the Engine api.
 *  @return 0 always.
 */
LOCAL int eapi_close ()
{
        MIN_DEBUG ("Closing");
	ec_reinit();
        MIN_DEBUG ("Closed");
	open = 0;
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Searches for test case by runid from the given list
 *  @param list_handle pointer to linked list of test case data
 *  @param runid search key
 *  @return pointer to test case data item,
 *          or returns INITPTR if operation failed.  
 *
 */
LOCAL DLListIterator tc_find_by_runid (DLList * list_handle, long runid)
{
	test_case_s    *tc;
        DLListIterator  it;

        
        for (it = dl_list_head (list_handle); it != INITPTR;
             it = dl_list_next(it)) {
		tc = (test_case_s *)dl_list_data (it);
		if (tc->tc_run_id_ == runid)
			return it;
        }

        
        return INITPTR;

}
/* ------------------------------------------------------------------------- */
/** Scan all the modules from search directories and add them to modulelist.
 *  @param [out] modulelist list of module file names.
 *  @return 0 on success, 1 on errror.
 */
LOCAL int eapi_query_test_modules(char **modulelist)
{
        char *modules = INITPTR;
        char *tmpmodules = INITPTR;

        int i = 0;
        int currlength = 0;
        int tmplen = 0;
        DLListIterator dl_item = DLListNULLIterator;
        char *dir;
        char *ptr;
        char *test_module;
        int n = 0;
        struct dirent **namelist;
	EngineDefaults *ec_settings;

	ec_settings = (EngineDefaults *)settings_get("Engine_Defaults");
	if (ec_settings == INITPTR)
		return 1;

        dl_item = dl_list_head (ec_settings->search_dirs);
        while (dl_item != INITPTR) {
                /* get data from list iterator */
                dir = (char *)dl_list_data (dl_item);

                /* return the number of directory entries */
                n = scandir (dir, &namelist, 0, alphasort);

                /* add .so files to test sets list */
                while (n--) {
                        ptr = strrchr (namelist[n]->d_name, '.');
                        if ((ptr != NULL) && (strcmp (ptr, ".so") == 0)) {

                                tmplen = strlen (namelist[n]->d_name);
                                if (modules==INITPTR) {
                                        modules = NEW2(char,tmplen+2);
                                        memset(modules,'\0',tmplen+2);
                                        test_module = &modules[currlength];
                                } else {
                                        tmpmodules = modules;
                                        modules = NEW2(char,tmplen+currlength+2);
                                        memset(modules,'\0',tmplen+currlength+2);
                                        memcpy(modules,tmpmodules,currlength);
                                        free(tmpmodules);
                                        test_module = &modules[currlength];
                                }

                                strncpy (test_module,namelist[n]->d_name,tmplen);
                                currlength = currlength + tmplen + 1;

                                i++;
                        }
                        free (namelist[n]);
                }
                free (namelist);

                /* get next item */
                dl_item = dl_list_next (dl_item);
        }

        *modulelist = modules;
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Scan all the test case files  from search directories and add them to 
 *  filelist.
 *  @param [out] filelist list of test case file names.
 *  @return 0 or 1 on error.
 */
LOCAL int eapi_query_test_files(char **filelist)
{
        char *files = INITPTR;
        char *tmpfiles = INITPTR;
        char *case_file = INITPTR, *p;
        TSBool addcasefile = ESTrue;
        char *dir = INITPTR;
        struct dirent **namelist;
        FILE *shell_pipe = INITPTR;
        char *shell_io = INITPTR;
        int tmplen = 0;
        int currlength = 0;
	EngineDefaults *ec_settings;
        DLListItem     *dl_item = INITPTR;
        int             i = 0;
        int             n = 0;
	
	ec_settings = (EngineDefaults *)settings_get("Engine_Defaults");
	if (ec_settings == INITPTR)
		return 1;

        dl_item = dl_list_head (ec_settings->search_dirs);
        while (dl_item != INITPTR) {
                /* get data from list iterator */
                dir = (char *)dl_list_data (dl_item);

                /* return the number of directory entries */
                n = scandir (dir, &namelist, 0, alphasort);

                /* add files to test sets list */
                while (n--) {
                        /* only normal files, not dots (. and ..) 
			 * and basic filter for non-case-files */
                        if ((strcmp (namelist[n]->d_name, ".") != 0) && 
                                (strcmp (namelist[n]->d_name, "..") != 0) &&
		                (strncmp (namelist[n]->d_name + 
		                strlen(namelist[n]->d_name) - 3
			        , ".la", 3) != 0) &&
			        (strncmp (namelist[n]->d_name +
			        strlen(namelist[n]->d_name) - 1
			        , "~", 1) !=0 ))
                        {
			    
                                addcasefile=ESTrue; /* restore flag value */

			        shell_io = malloc (strlen ("which file"
							   " 2>/dev/null") + 1);
			        strcpy(shell_io, "which file 2>/dev/null");
			        if ((shell_pipe=popen(shell_io, "r")) != NULL ) {
			                free(shell_io);
        				shell_io = malloc (sizeof(char) * 1);
	        			*shell_io = (char)fgetc(shell_pipe);
		        		pclose(shell_pipe);
			        	if (strncmp (shell_io, "/", 1) == 0) {
			    	                free(shell_io);
			    	                shell_io = malloc (sizeof(char) *
					        (strlen (namelist[n]->d_name) +
				                strlen (dir) +
				                strlen ("file -i -b ") +
				                2 +
				                strlen (" 2>/dev/null")));
			                        strcpy (shell_io, "file -i -b ");
                                                strcat (shell_io, dir );
			                        strcat (shell_io, "/" );
			                        strcat (shell_io, namelist[n]->d_name );
			                        strcat (shell_io, " 2>/dev/null" );
                                                if ((shell_pipe = popen (shell_io , "r"))
                                                        != NULL) {
                                                        free (shell_io);
                                                        shell_io = malloc (sizeof(char) * 5);
                                                        p = fgets (shell_io, 5, shell_pipe);
							if (p != NULL)
								*(shell_io+4)=0x00;
                                                        if (p && strcmp(shell_io, "text" ) != 0) {
                                                                /* don't add file if not text*/
                                                                addcasefile=ESFalse;
                                                        }
                                                        pclose (shell_pipe);
                                                }
				        } 
			        } 

			        /* add file to the list if it's not marked to omit
			         * by previous checks */
			        if(addcasefile==ESTrue){
                                        tmplen = strlen (namelist[n]->d_name);
                                        if (files==INITPTR) {
                                                files = NEW2(char,tmplen+2);
                                                memset(files,'\0',tmplen+2);
                                                case_file = &files[currlength];
                                        } else {
                                                tmpfiles = files;
                                                files = NEW2(char,tmplen+currlength+2);
                                                memset(files,'\0',tmplen+currlength+2);
                                                memcpy(files,tmpfiles,currlength);
                                                free(tmpfiles);
                                                case_file = &files[currlength];
                                        }

                                        strncpy (case_file,namelist[n]->d_name,tmplen);
                                        currlength = currlength + tmplen + 1;

			                i++;
			        }

	                        free (shell_io);			
                        }
                        free (namelist[n]);
                }
                free (namelist);

                /* get next item */
                dl_item = dl_list_next (dl_item);
        }

        *filelist = files;
	return 0;
}

/* ------------------------------------------------------------------------- */
/** Receive RCP prototocol message.
 *  @param message the message
 *  @param lentght message lenght.
 *  @return 0 on success, -1 on error. 
 */
LOCAL int eapi_receive_rcp (char *message, int length)
{
	return tec_extif_message_received (message, length);
}

/* ------------------------------------------------------------------------- */
/** Register slave device.
 *  @param host hostname or ip address (in dot format) of the slave device.
 *  @param slavetype type of slave (can be null, defaults to "phone")
 *  @return 0 on success 1 on error.
 */
LOCAL int eapi_register_slave (char *host, char *slavetype)
{
	int ret;
	struct addrinfo hints, *result;
	

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	ret = getaddrinfo(host, "51551", &hints, &result);
	if (ret != 0) {
		MIN_WARN ("failed to resolve host %s: %s",
			  gai_strerror (ret));
		return 1;
	}
       

       return tec_add_ip_slave_to_pool (&result, 
					slavetype ? slavetype : "phone");
}
/* ------------------------------------------------------------------------- */
/** Start test case is debugger.
 *  @param module_id test module identifier.
 *  @param case_id test case identifier.
 *  @return 0 on success -1 on error.
 */
LOCAL int eapi_debug_test_case (unsigned module_id, unsigned case_id)
{
	int result = 0;
	DLListIterator mod_it, case_it;
        test_module_info_s *module;
	
        pthread_mutex_lock (&tec_mutex_);

	mod_it = tm_find_by_module_id (instantiated_modules, module_id);
	if (mod_it == INITPTR) {
		MIN_WARN ("No module by id %d found", module_id);
		return 1;
	}
	module = (test_module_info_s *)dl_list_data (mod_it);
	case_it = tc_find_by_id (module->test_case_list_, case_id);
	if (case_it == INITPTR) {
		MIN_WARN ("No case by id %d found", case_id);
		return 1;
	}

	pthread_mutex_unlock (&tec_mutex_);
	case_it = ec_select_case (case_it, 0);

	result = ec_debug_case (case_it);

        return result;
}


/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Initializes the Engine API - binds pointers to functions to callback
 *  functions.
 */
void eapi_init (eapiIn_t *inp, eapiOut_t *out)
{
        if (plugins == INITPTR) plugins = dl_list_create();
        dl_list_add (plugins, (void*)inp);

        modules = dl_list_create();        
        out->add_test_module    = eapi_add_test_module;
        out->add_test_case_file = eapi_add_test_case_file;
        out->start_case         = eapi_start_test_case;
	out->pause_case         = eapi_pause_case;
	out->resume_case        = eapi_resume_case;
	out->abort_case         = eapi_abort_case;
        out->fatal_error        = eapi_error;
	
	out->min_close          = eapi_close;
	out->min_open           = eapi_open;
        out->query_test_modules = eapi_query_test_modules;
        out->query_test_files   = eapi_query_test_files;

	out->register_slave     = eapi_register_slave;
	out->receive_rcp        = eapi_receive_rcp;
	out->debug_case         = eapi_debug_test_case;
}

/* ------------------------------------------------------------------------- */
/** Gets pointer to the plugin list. Used by the MINAPI macro
 */
DLList* get_plugin_list()
{
        return plugins;
}
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */


/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
//#include "min_engine_api.tests"
#endif                          /* MIN_UNIT_TEST */

/* End of file */
