/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 *  @file       scripter_plugin.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN Scripter plugin.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <ctype.h>
#include <string.h>

#include <scripter_plugin.h>
#include <min_logger.h>
/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
extern DLList  *defines;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
extern void set_caller_name (const char *caller); /* test_module_api.c */
/* ------------------------------------------------------------------------- */

//extern char *strcasestr (const char *haystack,const char *needle);
//extern char *strchr(const char *s, int c);

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
char *module_date = __DATE__;
char *module_time = __TIME__;
TTestModuleType module_type     = ENormal;
unsigned int    module_version  = 200950;

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
#define SCRIPTER_SYNTAX_ERROR(__keyword__,__errstr__)\
do {\
        MIN_ERROR ("Test case \"%s\": line %d: %s keyword syntax error - %s.",\
		   tc_title ? tc_title : "<null>", line_number, __keyword__, \
		   __errstr__ );\
        tm_print_err ("Test case \"%s\": line %d:"\
		      " %s keyword syntax error - %s.",			\
		      tc_title ? tc_title : "<null>", line_number, __keyword__,\
		      __errstr__);\
} while (0)

#define SCRIPTER_SYNTAX_ERROR_ARG(__keyword__,__errstr__,__arg__)	\
do {\
	MIN_ERROR ("Test case \"%s\": line %d: %s keyword syntax error - "\
		   "%s:%s",tc_title ? tc_title : "<null>", line_number,	\
		   __keyword__,__errstr__, __arg__);			\
	tm_print_err ("Test case \"%s\": line %d: %s keyword syntax error - " \
		      "%s:%s", tc_title ? tc_title : "<null>", line_number, \
		      __keyword__, __errstr__, __arg__);		\
} while (0)

#define SCRIPTER_ERROR(__errstr__,__arg__)\
do {\
	MIN_ERROR ("Test case \"%s\": line %d: error - "		\
		   "%s %s", tc_title ? tc_title : "<null>", line_number,\
		   __errstr__, __arg__);				\
	tm_print_err ("Test case \"%s\": line %d: error - "	\
		      "%s %s", tc_title ? tc_title : "<null>", line_number, \
		      __errstr__, __arg__);				\
} while (0)

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* ------------------------------------------------------------------------- */
/** list of allocated slaves. Used during the validation process. */
static LegoBasicType *current = INITPTR;
/** used for validation of stopping interference */
/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */
/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL int       look4slave (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
LOCAL int       check_create_line (MinItemParser * line, int line_number, 
				   char * tc_title, DLList *assoc_lnames,
				   DLList *assoc_cnames, DLList *symblist);
/* ------------------------------------------------------------------------- */
LOCAL int       check_delete_line (MinItemParser * line, int line_number, 
				   char * tc_title, DLList *assoc_lnames,
				   DLList *assoc_cnames);
/* ------------------------------------------------------------------------- */
LOCAL int       check_run_line (MinItemParser * line, int line_number, 
				char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_pause_line (MinItemParser * line, int line_number,
				  char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_loop_line (MinItemParser * line, int line_number,
				 char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_request_line (MinItemParser * line, int line_number, 
				    char * tc_title, DLList *requested_events);
/* ------------------------------------------------------------------------- */
LOCAL int       check_release_line (MinItemParser *line, int line_number, 
				    char *tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_wait_line (MinItemParser *line, int line_number, 
				 char *tc_title, DLList *requested_events);
/* ------------------------------------------------------------------------- */
LOCAL int       check_methodcall_line (MinItemParser * line, int line_number, 
				       char * tc_title, char *cname,
				       DLList *assoc_lnames,
				       DLList *assoc_cnames, DLList *symblist);
/* ------------------------------------------------------------------------- */
LOCAL int       check_allocate_line (MinItemParser * line, int line_number,
				     DLList * slaves, char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_free_line (MinItemParser * line, int line_number,
				 DLList * slaves, char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_remote_line (MinItemParser * line, DLList * variables,
                                   int line_number, DLList * slaves, 
				   char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_allownextresult_line (MinItemParser * line,
                                            int line_number,
					    char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_complete_line (MinItemParser * line, int line_number,
				     char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_timeout_line (MinItemParser * line, int line_number,
				    char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_sleep_line (MinItemParser *line, int line_number,
				  char *tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_var_line (MinItemParser *line, int line_number, 
				char *tc_title, DLList *var_list);
/* ------------------------------------------------------------------------- */
LOCAL int       check_sendreceive_line (MinItemParser *line, int line_number, 
					char *tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_blockingtimeout_line (MinItemParser * line, 
					    int line_number,
					    char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_interference_line (MinItemParser * line,
                                         int line_number,
					 DLList * interf_objs,
					 char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_expect_line (MinItemParser * line, DLList * varnames,
                                   int line_number, char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_if_line (MinItemParser * line, int line_number, 
			       char * tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       validate_symbols (DLList *symblist, DLList *testclasses, 
				  DLList *class_methods, 
				  int line_number, char *tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_interference_objects (DLList *interf_objs, 
					    int line_number,
					    char *tc_title);
/* ------------------------------------------------------------------------- */
LOCAL int       check_classes_for_deletion (DLList *assoc_names, 
					    int line_number, char *tc_title);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "expect" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_expect_line (MinItemParser * line, DLList * varnames,
                             int line_number, char *tc_title)
{
        char           *varname = NULL;
        char           *workstring = NULL;
        int             retval = ENOERR;

        DLListIterator  it = DLListNULLIterator;
        mip_get_next_string (line, &varname);
        it = dl_list_head (varnames);
        while (it != DLListNULLIterator) {
                workstring = (char *)dl_list_data (it);
                if (strcmp (workstring, varname) == 0) {
                        break;
                }
                it = dl_list_next (it);
        }
        if (it == DLListNULLIterator) {
                SCRIPTER_SYNTAX_ERROR ("expect", 
				       "Expecting undeclared variable");
                retval = 1;
        }

	DELETE (varname);

        return retval;
}
/* ------------------------------------------------------------------------- */
/** Compare function for dl_list_find().
 */
LOCAL int look4slave (const void *a, const void *b)
{
        return strcmp ((char *)a, (char *)b);
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "createx" keyword
 * @param line  min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number  line number for debug messages
 * @param tc_title  title of validated test case
 * @param assoc_lname  library names list
 * @param assoc_cnames class names list
 * @param symblist list of symbols 
 * @return 0 if line is valid
 */
LOCAL int check_create_line (MinItemParser * line, int line_number, 
			     char * tc_title, DLList *assoc_lnames,
			     DLList *assoc_cnames, DLList *symblist)
{
        char  *ass_lname = NULL;
	char  *ass_cname = NULL;
        char  *libname = NULL;
	char  *classname = NULL;
        int   lib_already_found = 0;
        ScripterDataItem *library;
	DLListIterator it;
	
	/*
	** 1. do we have a class library
	*/
	if (mip_get_next_string (line, &libname) != 0) {
		SCRIPTER_SYNTAX_ERROR ("createx",
				       "library name "
				       "is not defined");
		return 1;
	}

	/*
	** 2. do we name for the created class object
	*/
	ass_lname = NEW2 (char, strlen (libname) + 1);
	sprintf (ass_lname, "%s", libname);
	DELETE (libname);
	dl_list_add (assoc_lnames, (void *)ass_lname);
	if (mip_get_next_string (line, &classname) != 0) {
		SCRIPTER_SYNTAX_ERROR ("createx",
				       "class name is"
				       " not defined");
		return 1;
	}

	/*
	** 3. add the class to symbol list if not already present
	*/
	ass_cname = NEW2 (char, strlen (classname) + 1);
	sprintf (ass_cname, "%s", classname);
	dl_list_add (assoc_cnames, (void *)ass_cname);
	DELETE (classname);
	it = dl_list_head (symblist);
	while (it != DLListNULLIterator) {
		library = (ScripterDataItem *)
			dl_list_data (it);
		if (strcmp (library->DLL_name_, ass_lname) ==
		    0) {
			lib_already_found = 1;
			break;
		}
		it = dl_list_next (it);
	}
	if (lib_already_found == 0) {
		library =
			scripter_dsa_create (ass_lname, ass_cname,
					     EDLLTypeClass);
		scripter_dsa_add (symblist, library);
	}
	
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "delete" keyword
 * @param line  min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number  line number for debug messages
 * @param tc_title  title of validated test case
 * @param assoc_lname  library names list
 * @param assoc_cnames class names list
 * @return 0 if line is valid
 */
LOCAL int check_delete_line (MinItemParser * line, int line_number, 
			     char * tc_title, DLList *assoc_lnames,
			     DLList *assoc_cnames)
{
	int check_result, i;
	char *classname = NULL;
	char *ass_cname = NULL, *ass_lname = NULL;

	if ( mip_get_next_string(line,&classname) != 0 ){
		SCRIPTER_SYNTAX_ERROR ("delete", "no classname"
				       "to delete specified");
		return 1;
	}
	check_result = -1;      /*indicate if class was 
				  created before */
	for (i = 0;
	     i < dl_list_size (assoc_cnames);
	     i++) {
		ass_cname = (char *)dl_list_data (dl_list_at
						  (assoc_cnames,
						   i));
		if (strcmp (ass_cname, classname) == 0) {
			check_result = 0;
			break;
		}
	}
	if (check_result !=0 ){
		SCRIPTER_SYNTAX_ERROR ("delete", 
				       "classname not created");
		DELETE (classname);
		return 1;
	}
	/* ok, test class was created previously, remove 
	   information from assoc. lists */
	DELETE (ass_cname);
	DELETE (classname);
	dl_list_remove_it (dl_list_at
			   (assoc_cnames, i));
	ass_lname =
		(char *)
		dl_list_data ((dl_list_at
                                           (assoc_lnames, i)));
	DELETE (ass_lname);
	dl_list_remove_it (dl_list_at
			   (assoc_lnames, i));
	/* this way, by the end of validation, assoc_cnames & 
	   assoc_lnames lists should be empty. 
	   If they are not, validation fails */
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "run" keyword
 * @param line - min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number - line number for debug messages
 * @param tc_title - title of validated test case
 * @return 0 if line is valid
 */
LOCAL int check_run_line (MinItemParser * line, int line_number, char *tc_title)
{
        char           *lib_name = NULL;
        char           *cfg_name = NULL;
        int             case_id = -1;
        int             opresult = 0;
        char           *case_title = NULL;
        TSBool          case_title_given = ESFalse;
        TSChar         *f_path = NULL;
        DLList         *module_cases = dl_list_create ();
        ptr2case        get_cases = NULL;
        ptr2run         run_case = NULL;
        void           *dll_handle = NULL;
        DLListIterator  work_case_item = DLListNULLIterator;
        TestCaseInfo   *work_case = INITPTR;

        opresult = mip_get_next_string (line, &lib_name);
        if (opresult != 0) {
                SCRIPTER_SYNTAX_ERROR ("run",
				       "test module is not defined");
                goto EXIT;
        }

        opresult = mip_get_next_string (line, &cfg_name);
        if (opresult != 0) {
                SCRIPTER_SYNTAX_ERROR ("run",
				       "test case id is not defined");
                goto EXIT;
        }

        opresult = mip_get_string (line, "title", &case_title);
        if (case_title != NULL) {
                case_title_given = ESTrue;
        }
        if (strstr (lib_name, ".so") == NULL) {
                f_path = NEW2 (char, strlen (lib_name) + 4);
                sprintf (f_path, "%s.so", lib_name);
        } else {
                f_path = NEW2 (char, strlen (lib_name) + 1);
                sprintf (f_path, "%s", lib_name);
        }

        dll_handle = tl_open_tc (f_path);
        if (dll_handle == INITPTR) {
                opresult = 1;
                SCRIPTER_SYNTAX_ERROR_ARG ("run",
					   "library not opened", lib_name);
                goto EXIT;
        }

        run_case = (ptr2run) dlsym (dll_handle, "tm_run_test_case");
        if (run_case == NULL) {
                SCRIPTER_SYNTAX_ERROR_ARG ("run",
					   "run_case() unresolved", dlerror());
                opresult = 1;
                goto EXIT;
        }

        get_cases = (ptr2case) dlsym (dll_handle, "tm_get_test_cases");
        if (get_cases == NULL) {
                SCRIPTER_SYNTAX_ERROR_ARG ("run",
					   "get_cases() unresolved", dlerror);
                goto EXIT;
        }

        opresult = get_cases (cfg_name, &module_cases);
        if ((dl_list_size (module_cases) == 0) || (opresult != 0)) {

                SCRIPTER_SYNTAX_ERROR_ARG ("run", "Failed to get test "
					   "cases from module",
					   lib_name);
                goto EXIT;
        }
        if (case_title_given == ESFalse) {
                if ((dl_list_size (module_cases) + 1) >= case_id) {
                        opresult = 0;
                } else {
                        SCRIPTER_SYNTAX_ERROR ("run", 
					       "Test case id out of range.");
                        opresult = 1;
                }
                goto EXIT;
        }

        work_case_item = dl_list_head (module_cases);
        while (work_case_item != DLListNULLIterator) {
                work_case = (TestCaseInfo *) dl_list_data (work_case_item);
                if (strcmp (work_case->name_, case_title) == 0) {
                        /*case with matching title found */
                        opresult = 0;
                        goto EXIT;
                }
                work_case_item = dl_list_next (work_case_item);
                if (work_case_item == DLListNULLIterator) {
                        /* whole list has been searched, case with matching
                           title was not found */
                        opresult = 1;
                        SCRIPTER_SYNTAX_ERROR ("run",
					       "case with matching title not"
					       " found.");
                        goto EXIT;
                }
        }
      EXIT:
	if (dll_handle) dlclose (dll_handle);
        DELETE (f_path);
        DELETE (lib_name);
        DELETE (case_title);
        DELETE (cfg_name);
        if (dl_list_size (module_cases) != 0) {
                /*free memory allocated for test cases */
                work_case_item = dl_list_head (module_cases);
                while (work_case_item != DLListNULLIterator) {
                        work_case =
                            (TestCaseInfo *) dl_list_data (work_case_item);
                        DELETE (work_case);
                        dl_list_remove_it (work_case_item);
                        work_case_item = dl_list_head (module_cases);
                }
        }
        dl_list_free (&module_cases);
        return opresult;
}
/*------------------------------------------------------------------------- */
/** Checks validitye of line with "pause" keyword
 * @param line - min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number - line number for debug messages
 * @param tc_title - title of validated test case
 * @return 0 if line is valid
 */
LOCAL int check_pause_line (MinItemParser * line, int line_number, 
			    char *tc_title)
{
        char           *string = INITPTR;
        int             result = -1;

        result = mip_get_next_string (line, &string);
        if (result != 0) {
                SCRIPTER_SYNTAX_ERROR ("pause",
				       "test id is not defined");
                return 1;
        }
        DELETE (string);
        return 0;
}
/*------------------------------------------------------------------------- */
/** Checks validity of line with "loop" keyword
 * @param line - min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number - line number for debug messages
 * @param tc_title - title of validated test case
 * @return 0 if line is valid
 */
LOCAL int check_loop_line (MinItemParser * line, int line_number, 
			   char * tc_title)
{
        int             param1 = 0;
        char           *opt_msec = NULL;
        int             result = -1;

        /*check value of int after "loop" */
        result = mip_get_int (line, "", &param1);
        if (result != 0) {
                SCRIPTER_SYNTAX_ERROR ("loop",
				       "number of iterations/time is "
				       "not defined");
                return 1;
        }
        /* check if the "msec" is stated */
        result = mip_get_next_string (line, &opt_msec);
        if (result == 0) {
                if (strcmp (opt_msec, "msec") == 0) {
                        result = 0;
                } else {
                        SCRIPTER_SYNTAX_ERROR ("loop",
					       "wrong keyword in loop "
					       "statement");
                        result = 1;
                }
        }

	DELETE (opt_msec);

        return 0;
}
/*------------------------------------------------------------------------- */
/** Checks validity of line with "request" keyword
 * @param line - min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number - line number for debug messages
 * @param tc_title - title of validated test case
 * @param requested_events list of already requested events
 * @return 0 if line is valid
 */
LOCAL int check_request_line (MinItemParser *line, int line_number, 
			      char *tc_title, DLList *requested_events)
{
	char *eventname = NULL, *ass_evname;

	if (mip_get_next_string (line, &eventname) != 0) {
		SCRIPTER_SYNTAX_ERROR ("request",
				       "event name is not "
				       "defined");
		return 1;
	}
	
	ass_evname = NEW2 (char, strlen (eventname) + 1);
	sprintf (ass_evname, "%s", eventname);
	DELETE (eventname);
	dl_list_add (requested_events, (void *)ass_evname);

	return 0;
}
/*------------------------------------------------------------------------- */
/** Checks validity of line with "release" keyword
 * @param line - min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number - line number for debug messages
 * @param tc_title - title of validated test case
 * @return 0 if line is valid
 */
LOCAL int check_release_line (MinItemParser *line, int line_number, 
			      char *tc_title) 
{
	char *eventname = NULL;

	if (mip_get_next_string (line, &eventname) != 0) {
		SCRIPTER_SYNTAX_ERROR ("release",
				       "event name is not "
				       "defined");
		DELETE (eventname);
		return 1;
	}
	DELETE (eventname);
	
	return 0;
}

/*------------------------------------------------------------------------- */
/** Checks validity of line with "wait" keyword
 * @param line - min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number - line number for debug messages
 * @param tc_title - title of validated test case
 * @param requested_events list of requested events
 * @return 0 if line is valid
 */
LOCAL int check_wait_line (MinItemParser *line, int line_number, 
			   char *tc_title, DLList *requested_events)
{
	char *eventname = NULL, *ass_evname;
	DLListIterator it;

	
	if (mip_get_next_string (line, &eventname) != 0) {
		SCRIPTER_SYNTAX_ERROR ("wait",
				       "event name is not defined");
		return 1;
	}
	
	it = dl_list_head (requested_events);
	if (it == DLListNULLIterator) {
		SCRIPTER_SYNTAX_ERROR ("wait",
				       "wait for not requested event");
		return 1;
	}
	while (it != DLListNULLIterator) {
		ass_evname = (char *)dl_list_data (it);
		if (strcmp (ass_evname, eventname) == 0) {
			break;
		}
		it = dl_list_next (it);
		if (it == DLListNULLIterator) {
			DELETE (eventname);
			SCRIPTER_SYNTAX_ERROR ("wait",
					       "wait for not "
					       "requested event");
			return 1;
		}
		
	}
	DELETE (eventname);
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with test method call
 * @param line  min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number  line number for debug messages
 * @param tc_title  title of validated test case
 * @param cname object name 
 * @param assoc_lname  library names list
 * @param assoc_cnames class names list
 * @param symblist list of symbols 
 * @return 0 if line is valid
 */
LOCAL int check_methodcall_line (MinItemParser * line, int line_number, 
				 char * tc_title, char *cname,
				 DLList *assoc_lnames,
				 DLList *assoc_cnames, DLList *symblist)
{
	char *ass_lname = NULL, *callname = NULL, *symb_callname = NULL;
	char *ass_cname = NULL;
	DLListIterator it;
	int i, lib_found = 0;
        ScripterDataItem *library;

	for (i = 0; i < dl_list_size (assoc_cnames); i++) {
		ass_cname = (char *)dl_list_data (dl_list_at (assoc_cnames, i));
		if (strcmp (ass_cname, cname) == 0) {
			ass_lname = (char *)dl_list_data 
				(dl_list_at (assoc_lnames, i));
			break;
		}
	}
	
	if (ass_lname == NULL) {
		SCRIPTER_SYNTAX_ERROR ("<method call>",
				       "call for method of "
				       "non existing class");
		return 1;
	}
	
	mip_get_next_string (line, &callname);
	if (callname == NULL) {
		SCRIPTER_SYNTAX_ERROR ("<method call>",
				       "method name is not"
				       " defined");
		return 1;
	}
	symb_callname = NEW2 (char, strlen (callname) + 1);
	STRCPY (symb_callname, callname,
                                strlen (callname) + 1);
	DELETE (callname);
	it = dl_list_head (symblist);
	
	lib_found = 0;
	while (it != DLListNULLIterator) {
		library = (ScripterDataItem *)dl_list_data (it);
		if (strcmp (ass_lname, library->DLL_name_) == 0) {
			lib_found = 1;
			scripter_dsa_add_symbol (it,
						 symb_callname);
			break;
		}
		it = dl_list_next (it);
	}
	if (lib_found == 0) {
		SCRIPTER_SYNTAX_ERROR ("<method call>",
				       "Selected library "
				       "was not found");
		return 1;
	}

	return 0;
}
/*------------------------------------------------------------------------- */
/** Checks validity of line with "allocate" keyword
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param slaves [in] list of slaves
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_allocate_line (MinItemParser * line, int line_number,
			       DLList * slaves, char * tc_title)
{
        int             retval = ENOERR;
        int             result = 0;
        char           *token = INITPTR;
        DLListIterator  it = DLListNULLIterator;

        /* Check syntax: allocate <slave type> <slave name> */
        result = mip_get_next_string (line, &token);
        if (result != ENOERR) {
                SCRIPTER_SYNTAX_ERROR ("allocate",
				       "slave type is not defined");
                retval = 1;
                goto EXIT;
        }
        DELETE (token);
        result = mip_get_next_string (line, &token);
        if (result != ENOERR) {
		SCRIPTER_SYNTAX_ERROR ("allocate",
				       "slave name is not defined");
                retval = 1;
                goto EXIT;
        }

        /* Check if slave of this name was already allocated. */
        it = dl_list_find (dl_list_head (slaves)
                           , dl_list_tail (slaves)
                           , look4slave, (const void *)token);
        if (it != DLListNULLIterator) {
                SCRIPTER_SYNTAX_ERROR ("allocate",
				       "slave with selected name "
				       "already exists");
                retval = 1;
                DELETE (token);
                goto EXIT;
        }

        /* If slave not allocated than add it to the allocated slaves list. */
        dl_list_add (slaves, (void *)token);
      EXIT:
        return retval;
}
/*------------------------------------------------------------------------- */
/** Checks validity of line with "free" keyword
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param slaves [in] list of slaves
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_free_line (MinItemParser * line, int line_number,
			   DLList *slaves, char * tc_title)
{
        int             retval = ENOERR;
        int             result = 0;
        char           *token = INITPTR;
        DLListIterator  it = DLListNULLIterator;

        /* Check syntax. */
        result = mip_get_next_string (line, &token);
        if (result != ENOERR) {
                retval = 1;
                DELETE (token);
                SCRIPTER_SYNTAX_ERROR ("free",
				       "slave name is not defined");
                goto EXIT;
        }

        /* Check if slave of this name was already allocated. */
        it = dl_list_find (dl_list_head (slaves)
                           , dl_list_tail (slaves)
                           , look4slave, (const void *)token);
        if (it == DLListNULLIterator) {
                retval = 1;
                DELETE (token);
                SCRIPTER_SYNTAX_ERROR ("free",
				       "slave not allocated");
                goto EXIT;
        }
        DELETE (token);

        /* If slave allocated and this is free command then remove slave */
        token = (char *)dl_list_data (it);
        DELETE (token);
        dl_list_remove_it (it);
      EXIT:
        return retval;
}
/*------------------------------------------------------------------------- */
/** Checks validity of line with "remote" keyword
 *  @param line [in] MinItemParser containing line.
 *  @param variables [in] list of variables
 *  @param line_number - line number for debug messages
 *  @param slaves [in] list of slaves
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_remote_line (MinItemParser * line, DLList * variables,
                             int line_number, DLList * slaves, char * tc_title)
{
        int             retval = ENOERR;
        int             result = 0;
        char           *token = INITPTR;
        char           *arg = NULL;
        DLListIterator  it = DLListNULLIterator;
        TScripterKeyword command = EKeywordUnknown;

        /* check syntax: remote <slave name> <command> */
        result = mip_get_next_string (line, &token);
        if (result != ENOERR) {
                retval = 1;
                DELETE (token);
                SCRIPTER_SYNTAX_ERROR ("remote",
				       "slave name is not defined");
                goto EXIT;
        }
        /* Check if slave of this name was already allocated. */
        it = dl_list_find (dl_list_head (slaves)
                           , dl_list_tail (slaves)
                           , look4slave, (const void *)token);
        if (it == DLListNULLIterator) {
                retval = 1;
                DELETE (token);
                SCRIPTER_SYNTAX_ERROR ("remote",
				       "slave not allocated");
                goto EXIT;
        }
        DELETE (token);

        /* Go back to syntax checking */
        result = mip_get_next_string (line, &token);
        if (result != ENOERR) {
                retval = 1;
                DELETE (token);
                SCRIPTER_SYNTAX_ERROR ("remote",
				       "remote command is not defined");
                goto EXIT;
        }

        command = get_keyword (token);
        switch (command) {
        case EKeywordSendreceive:
                mip_get_next_string (line, &arg);
                if (strchr (arg, '=') == NULL) {
                        SCRIPTER_SYNTAX_ERROR("remote",
					      "wrong argument for remote "
					      "sendreceive (no \"=\")");
                        retval = 1;
                } else
                        retval = 0;
                DELETE (arg);
                break;
        case EKeywordExpect:

                mip_get_next_string (line, &arg);
                it = dl_list_find (dl_list_head (variables)
                                   , dl_list_tail (variables)
                                   , look4slave, (const void *)arg);
                if (it == DLListNULLIterator) {
                        SCRIPTER_SYNTAX_ERROR ("remote",
					       "remote expect for unknown var");
                        retval = 1;
                } else
                        retval = 0;
                DELETE (arg);
                break;
        default:
                break;
        }
        DELETE (token);
      EXIT:
        return retval;
}
/*------------------------------------------------------------------------- */
/** Checks validity of line with "allownextresult" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_allownextresult_line (MinItemParser * line, int line_number,
				      char * tc_title)
{
        int             retval = ENOERR;
        int             tmp = 0;

        if (line == INITPTR) {
                retval = 1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_int (line, &tmp);

        if (retval != ENOERR) {
                SCRIPTER_SYNTAX_ERROR ("allownextresult",
				       "result code is not defined");
                retval = 1;
                goto EXIT;
        }
      EXIT:
        return retval;
}
/*------------------------------------------------------------------------- */
/** Checks validity of line with "complete" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_complete_line (MinItemParser * line, int line_number, 
			       char * tc_title)
{
        int             retval = ENOERR;
        char           *token = INITPTR;

        if (line == INITPTR) {
                retval = 1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_string (line, &token);

        /* Check syntax */
        if (retval != ENOERR) {
                SCRIPTER_SYNTAX_ERROR ("complete",
				       "testid is not defined");
                retval = 1;
                goto EXIT;
        }

        DELETE (token);
      EXIT:
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "timeout" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_timeout_line (MinItemParser * line, int line_number,
			      char * tc_title)
{
        int             retval = ENOERR;
        int             tmp = 0;

        if (line == INITPTR) {
                retval = 1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_int (line, &tmp);

        /* Check syntax */
        if (retval != ENOERR) {
                SCRIPTER_SYNTAX_ERROR ("timeout",
				       "timeout interval is not defined");
                retval = 1;
                goto EXIT;
        }
        if (tmp < 1) {
                SCRIPTER_SYNTAX_ERROR ("timeout",
				       "invalid interval value");
                retval = 1;
                goto EXIT;
        }

      EXIT:
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "testinterference" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param interf_objs list of interference objects
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_interference_line (MinItemParser * line, int line_number,
				   DLList * interf_objs, char * tc_title)
{
        int             retval = ENOERR;
        char           *name = INITPTR;
        char           *command = INITPTR;
        char           *tmp = INITPTR;
        char           *type = INITPTR;
        int             value = 0;
        int             idle_time = 0;
        int             busy_time = 0;
        DLListIterator  work_inter = DLListNULLIterator;

        if (line == INITPTR) {
                retval = 1;
                errno = EINVAL;
                goto EXIT;
        }
        /*first check if sp-stress is available at all
           this will be done by checking if execs exist in 
           /usr/bin */
        if ((access ("/usr/bin/cpuload", X_OK) != 0) ||
            (access ("/usr/bin/memload", X_OK) != 0) ||
            (access ("/usr/bin/ioload", X_OK))) {
                SCRIPTER_SYNTAX_ERROR ("testinterference", "check if sp-stress"
				       " is installed properly");
                retval = 1;
                goto EXIT;
        }
        retval = mip_get_next_string (line, &name);

        /* check syntax */
        if (retval != ENOERR) {
                SCRIPTER_SYNTAX_ERROR ("testinterference","name missing");
                retval = 1;
                goto EXIT;
        }

        retval = mip_get_next_string (line, &command);

        if (retval != ENOERR) {
                SCRIPTER_SYNTAX_ERROR ("testinterference",
				       "command not specified");
                retval = 1;
                goto EXIT;
        }
        if (strcasecmp (command, "start") == 0) {

                retval = mip_get_next_string (line, &type);
                if (retval != ENOERR) {
			SCRIPTER_SYNTAX_ERROR ("testinterference",
					       "type not specified");
                        retval = 1;
                        goto EXIT;
                }
                if ((strcmp (type, "cpuload") != 0) &&
                    (strcmp (type, "memload") != 0) &&
                    (strcmp (type, "ioload") != 0)) {
                        SCRIPTER_SYNTAX_ERROR ("testinterference",
					       "type not specified correctly");
                        retval = 1;
                        goto EXIT;
                }
                retval = mip_get_next_int (line, &value);

                if (retval != ENOERR) {
                        SCRIPTER_SYNTAX_ERROR ("testinterference",
					       "value not specified");
                        retval = 1;
                        goto EXIT;
                }
                retval = mip_get_next_int (line, &idle_time);

                if (retval != ENOERR) {
                        SCRIPTER_SYNTAX_ERROR ("testinterference",
					       "idle time not specified");
                        retval = 1;
                        goto EXIT;
                }
                retval = mip_get_next_int (line, &busy_time);

                if (retval != ENOERR) {
			SCRIPTER_SYNTAX_ERROR ("testinterference",
					       "busy time not specified");
                        retval = 1;
                        goto EXIT;
                }
                retval = ENOERR;

                tmp = NEW2 (char, strlen (name) + 1);
                sprintf (tmp, "%s", name);
                dl_list_add (interf_objs, (void *)tmp);
        } else if (strcasecmp (command, "stop") == 0) {
                if (dl_list_size (interf_objs) == 0) {
                        retval = 1;
                        SCRIPTER_SYNTAX_ERROR ("testinterference",
					       "no interferences started at "
					       "this point");
                        goto EXIT;
                }

                work_inter = dl_list_head (interf_objs);
                while (work_inter != DLListNULLIterator) {
                        tmp = (char *)dl_list_data (work_inter);
                        if (strcmp (tmp, name) == 0)
                                break;
                        work_inter = dl_list_next (work_inter);
                }
                if (work_inter == DLListNULLIterator) {
                        retval = 1;
                        SCRIPTER_SYNTAX_ERROR ("testinterference",
					       "no such interference started "
					       "at this point");
                        goto EXIT;
                } else {
                        tmp = (char *)dl_list_data (work_inter);
                        DELETE (tmp);
                        dl_list_remove_it (work_inter);

                }
                retval = ENOERR;
                goto EXIT;
        } else {
                retval = 1;
                SCRIPTER_SYNTAX_ERROR ("testinterference",
				       "wrong command name");
                goto EXIT;
        }


      EXIT:
        if (name != INITPTR)
                DELETE (name);
        if (command != INITPTR)
                DELETE (command);
        if (type != INITPTR)
                DELETE (type);
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "sleep" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_sleep_line (MinItemParser * line, int line_number, 
			    char * tc_title)
{
        int             retval = ENOERR;
        int             tmp = 0;

        if (line == INITPTR) {
                retval = 1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_int (line, &tmp);

        /* check syntax */
        if (retval != ENOERR) {
                SCRIPTER_SYNTAX_ERROR ("sleep",
				       "sleep interval is not defined");
                retval = 1;
                goto EXIT;
        }
        if (tmp < 1) {
                SCRIPTER_SYNTAX_ERROR ("sleep",
				       "invalid interval value");
                retval = 1;
                goto EXIT;
        }
      EXIT:
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "var" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number line number for debug messages
 *  @param tc_title title of validated test case
 *  @param var_list list of variables
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_var_line (MinItemParser *line, int line_number, 
			  char *tc_title, DLList *var_list)
{
	char *varname = NULL;
        char *ass_varname = NULL;

	mip_get_next_string (line, &varname);
	if ((varname != NULL) || (varname != INITPTR)) {
		ass_varname =
			NEW2 (char, strlen (varname) + 1);
		sprintf (ass_varname, "%s", varname);
		dl_list_add (var_list, (void *)ass_varname);
		DELETE (varname);
	} else {
		SCRIPTER_SYNTAX_ERROR ("var",
				       "variable name not "
				       "specified");
		return 1;
	}
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "sendreceive" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number line number for debug messages
 *  @param tc_title title of validated test case
 *  @return ENOERR if line is valid, 1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_sendreceive_line (MinItemParser *line, int line_number, 
				  char *tc_title)
{
	char *varname = NULL;
	mip_get_next_string (line, &varname);
	if ((varname != NULL) && (varname != INITPTR)) {
		if (strchr (varname, '=') == NULL) {
			SCRIPTER_SYNTAX_ERROR ("sendrecive",
					       "Argument has faulty syntax "
					       "(no '=' character)");
			return 1;
		}
	}

	return 0;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "blockingtimeout" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int check_blockingtimeout_line (MinItemParser * line, int line_number, 
				      char * tc_title)
{
        int             retval = ENOERR;
        int             tmp = 0;

        if (line == INITPTR) {
                retval = 1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_int (line, &tmp);

        /* check syntax */
        if (retval != ENOERR) {
                SCRIPTER_SYNTAX_ERROR ("blockingtimeout",
				       "timeout is not definced");
                retval = 1;
                goto EXIT;
        }
        if (tmp < 0) {
                SCRIPTER_SYNTAX_ERROR ("blockingtimeout",
				       "invalid timeout value");
                retval = 1;
                goto EXIT;
        }
      EXIT:
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "if" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param tc_title - title of validated test case
 *  @return ENOERR if line is valid, -1 otherwise. 
 */
LOCAL int check_if_line (MinItemParser * line, int line_number, char * tc_title)
{
        int retval;
        char *token = INITPTR;

        if (line == INITPTR) {
                retval = 1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_string (line, &token);
        if (retval != ENOERR) {
                SCRIPTER_SYNTAX_ERROR ("if",
				       "no if condition specified");
        }
        DELETE (token);

EXIT:
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Validate symbols found from script test case.
 *  @param symblist list of symbols
 *  @param testclasses list of test class names
 *  @param class_methods list of method names
 *  @param line_number - line number for debug messages
 *  @param tc_title - title of validated test case
 *  @return ENOERR if symbols validate, 1 otherwise. 
 */
LOCAL int validate_symbols (DLList *symblist, DLList *testclasses, 
			    DLList *class_methods, 
			    int line_number, char *tc_title) {

	DLListIterator it, call_item, tc_item;
	ScripterDataItem *library;
        TestClassDetails *test_class = INITPTR;
	TestCaseInfo *tc;
	char *libpath = NULL;
        void *dll_handle = NULL;
        char *callname = NULL;

        it = dl_list_head (symblist);
	
        while (it != DLListNULLIterator) {
                library = (ScripterDataItem *) dl_list_data (it);
		
                switch (library->DLL_type_) {
			
                case EDLLTypeClass:
                        if (strstr (library->DLL_name_, ".so") == NULL) {
				if (libpath) free (libpath);
                                libpath =
					NEW2 (char,
					      strlen (library->DLL_name_) + 4);
                                sprintf (libpath, "%s.so",
                                         library->DLL_name_);
                        } else {
				if (libpath) free (libpath);
                                libpath =
					NEW2 (char,
					      strlen (library->DLL_name_) + 1);
                                sprintf (libpath, "%s", library->DLL_name_);
                        }
                        dll_handle = tl_open_tc (libpath);
                        if (dll_handle == INITPTR) {
                                SCRIPTER_ERROR ("Unable to load "
						"test library",
						libpath);
				DELETE (libpath);
				return 1;
                        }
                        test_class = NEW (TestClassDetails);
                        test_class->dllhandle_ = dll_handle;
                        test_class->classname_ =
				NEW2 (char, strlen (library->Class_name_) + 1);
                        STRCPY (test_class->classname_,
                                library->Class_name_,
                                strlen (library->Class_name_) + 1);
                        test_class->casetc_ =
				(ptr2casetc) dlsym (dll_handle,
						    "ts_get_test_cases");
                        if (test_class->casetc_ == NULL) {
                                SCRIPTER_ERROR ("tm_get_cases() unresolved "
						"in Test Library",
						dlerror ());
				DELETE (test_class->classname_);
				DELETE (test_class);
				DELETE (libpath);
				return 1;
                        }
                        test_class->runtc_ =
				(ptr2runtc) dlsym (dll_handle, "ts_run_method");
                        if (test_class->runtc_ == NULL) {
                                SCRIPTER_ERROR ("tm_run_case() unresolved "
						"in Test Library",
						dlerror ());
				DELETE (test_class->classname_);
				DELETE (test_class);
				DELETE (libpath);
                                return 1;
                        }
                        dl_list_add (testclasses, (void *)test_class);
                        /*get test case names */
                        test_class->casetc_ (&class_methods);
                        if ((class_methods == INITPTR)
                            || (dl_list_size (class_methods) == 0)) {
                                MIN_WARN ("Could not get list of "
					  "functions from %s",
					  test_class->classname_);
				DELETE (libpath);
				return 1;
                        }
                        call_item = dl_list_head (library->symbol_list_);
                        while (call_item != DLListNULLIterator) {
                                callname = (char *)dl_list_data (call_item);
                                tc_item = dl_list_head (class_methods);
                                while (tc_item != DLListNULLIterator) {
                                        tc = (TestCaseInfo *)
						dl_list_data (tc_item);
                                        if (strcmp (callname, tc->name_) == 0)
                                                break;
                                        tc_item = dl_list_next (tc_item);
                                        if (tc_item == DLListNULLIterator) {
                                                SCRIPTER_ERROR
							("Symbol not found "
							 "in class",
							 callname);
                                                dlclose (dll_handle);
						DELETE (libpath);
						return 1;
                                        }
					
                                }
                                call_item = dl_list_next (call_item);
                        }
			
                case EDLLTypeNormal:
                        {
                                break;
                        }
			
                }
                it = dl_list_next (it);
        }
	DELETE (libpath);
				
	return ENOERR;
}
/* ------------------------------------------------------------------------- */
/** Check that all interference objects are stopped within the test case
 *  @param interf_objs list of interference objects
 *  @param line_number line number for debug messages
 *  @param tc_title title of validated test case
 *  @return number of errors
 */
LOCAL int check_interference_objects (DLList *interf_objs, int line_number,
				      char *tc_title)
{
	int errors = 0;
	char *p;
	DLListIterator it;

        if ((interf_objs != INITPTR)&&(dl_list_size (interf_objs) != 0)) {
                SCRIPTER_SYNTAX_ERROR ("interference",
				       "Not all test interference "
				       "instances stopped!");
                it = dl_list_head (interf_objs);
                while (it != DLListNULLIterator) {
                        p = (char *)dl_list_data (it);
                        DELETE (p);
                        dl_list_remove_it (it);
                        it = dl_list_head (interf_objs);
                }
		errors ++;
        }
	return errors;
}
/* ------------------------------------------------------------------------- */
/** Check that classes are deleted before the end.
 *  @param assoc_names name list which should be empty 
 *  @param line_number line number for debug messages
 *  @param tc_title title of validated test case
 *  @return number of errors
 */
LOCAL int check_classes_for_deletion (DLList *assoc_names, 
				      int line_number, char *tc_title) 
{
	int errors = 0;
	DLListIterator call_item;
	char *callname;

        call_item = dl_list_head (assoc_names);
        while (call_item != DLListNULLIterator) {
                callname = (char *)dl_list_data (call_item);
		SCRIPTER_ERROR ("Validation error: testclass not "
				"deleted:",
				callname);
                call_item = dl_list_next (call_item);
		errors ++;
        }

	return errors;
}
/* ------------------------------------------------------------------------- */
/** Clean-up function for validate_test_case().
 *  @param slaves list of slaves
 *  @param testclasses list of test classes
 *  @param classmethods list of methods
 *  @param assoc_cnames list of test class names
 *  @param assoc_lnames list of test method names
 *  @param requested_events list of requested events
 *  @param symblist list of symbols
 *  @param var_list list of variable names
 *  @param interf_objs list of interference objects
 */
void do_cleanup (DLList *slaves, 
		 DLList *testclasses, 
		 DLList *classmethods,
		 DLList *assoc_cnames,
		 DLList *assoc_lnames,
		 DLList *requested_events,
		 DLList *symblist,
		 DLList *var_list,
		 DLList *interf_objs)
{
	DLListIterator it;
        TestClassDetails *test_class;

	dl_list_free_data (&slaves);
	dl_list_free (&slaves);

        for (it = dl_list_head (testclasses); it != INITPTR; 
	     it = dl_list_next (it)) {
                test_class = dl_list_data (it);
		if (test_class->dllhandle_ &&
		    test_class->dllhandle_ != INITPTR) 
			dlclose (test_class->dllhandle_);
                DELETE (test_class->classname_);
                DELETE (test_class);
        }
        dl_list_free (&testclasses);

	dl_list_free_data (&classmethods);
	dl_list_free (&classmethods);

	dl_list_free_data (&assoc_cnames);
	dl_list_free (&assoc_cnames);

	dl_list_free_data (&assoc_lnames);
	dl_list_free (&assoc_lnames);

	dl_list_free_data (&requested_events);
	dl_list_free (&requested_events);

        it = dl_list_head (symblist);
        while (it != DLListNULLIterator) {
                scripter_dsa_remove (it);
                it = dl_list_head (symblist);
        }
        dl_list_free (&symblist);

	dl_list_free_data (&var_list);
	dl_list_free (&var_list);
	
	dl_list_free (&interf_objs);

}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Validate the define section of script file
 *  @param define [in] the define section of script file
 *  @return ESTrue if all symbols are available, ESFalse otherwise.
 */
TSBool validate_define (MinSectionParser * define)
{
        TSBool          retval = ESTrue;
        MinItemParser  *line = INITPTR;
        char           *str1 = INITPTR;
        char           *str2 = INITPTR;
        int             pom = 0;
        unsigned int    line_number = 1;
	char *tc_title = "Define Section";

        if (define == INITPTR) {
                retval = ESFalse;
                goto EXIT;
        }

        line = mmp_get_item_line (define, "", ESNoTag);

        while (line != INITPTR) {

                mip_set_parsing_type (line, EQuoteStyleParsing);

                /* Correct line in define section consist of two strings
                 * that are separated by space */
                pom = mip_get_string (line, "", &str1);
                if (pom != ENOERR) {
                        SCRIPTER_SYNTAX_ERROR ("define section",
					       "identifier was expected");
                        retval = ESFalse;
                        mip_destroy (&line);
                        goto EXIT;
                }

                pom = mip_get_next_string (line, &str2);
                if (pom == ENOERR) {
                        struct define  *def = NEW (struct define);
                        memset (def, '\0', sizeof (struct define));
                        STRCPY (def->label_, str1, strlen (str1) + 1);
                        STRCPY (def->value_, str2, strlen (str2) + 1);
                        dl_list_add (defines, (void *)def);
                        DELETE (str1);
                        DELETE (str2);
                } else {
                        SCRIPTER_SYNTAX_ERROR ("define section ",
					       "definition value was expected");
                        retval = ESFalse;
                        mip_destroy (&line);
			DELETE (str1);
                        goto EXIT;
                }

                pom = mip_get_next_string (line, &str1);
                if (pom == ENOERR) {
                        DELETE (str1);
			MIN_ERROR ("Syntax warning on line %d of Define section"
				   " unexpected literal", line_number);
                }

                mip_destroy (&line);
                line = mmp_get_next_item_line (define);
                line_number ++;
        }

      EXIT:
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Load test libraries used by script and checks if used symbols are available
 *  from those libraries
 *  @param tescase [in] scripted test case data
 *  @param description [out] test case description
 *  @return tc_title if the case validates NULL otherwise
 */
char      *validate_test_case (MinSectionParser * testcase, char **description)
{
        MinItemParser *line = INITPTR;
        char           *token = INITPTR;
        int             line_number = -1;       // Line number in test case

        /* association lists, assoc_cnames holds class names defined
	   in script, assoc_lnames holds library names in corresponding
	   positions */
        DLList         *assoc_cnames = dl_list_create ();
        DLList         *assoc_lnames = dl_list_create ();
        DLList         *symblist = dl_list_create ();
	/*holds "variable" names, for validating var and expect keywords */
        DLList         *var_list = dl_list_create ();   
        /* holds names of requested events,  to validate "wait" statements */
        DLList         *requested_events = dl_list_create ();
        /* holds list of slaves for "allocate"/"free" validation */
	DLList         *slaves = dl_list_create ();

        DLList         *class_methods = dl_list_create();
	DLList         *interf_objs = dl_list_create();
        char           *tc_title = NULL;
        DLList         *testclasses = dl_list_create ();
        int             nest_level = 0;
        char            nesting [255];
	int             errors = 0;
	Text           *tx_desc = tx_create ("");

        line = mmp_get_item_line (testcase, "", ESNoTag);
        while (line != INITPTR) {
                line_number++;  
                line->parsing_type_ = EQuoteStyleParsing;

                if (token != INITPTR)
                        DELETE (token);
                mip_get_string (line, "", &token);
		
		errors += validate_line (token, 
					 line, 
					 line_number, 
					 &tc_title, 
					 tx_desc,
					 assoc_lnames, 
					 assoc_cnames,
					 symblist, 
					 var_list,
					 requested_events,
					 slaves, 
					 interf_objs, 
					 nesting,
					 &nest_level);
		mip_destroy (&line);
		if (errors)
			goto EXIT_VALIDATE;
		line = mmp_get_next_item_line (testcase);

        }

        /* check if script's loop counter is 0, otherwise fail syntax check */
        if (nest_level != 0) {
		errors ++;
                SCRIPTER_SYNTAX_ERROR (nesting [nest_level] == 1 ? "endif"
				       : "endloop", "missing");
                goto EXIT_VALIDATE;
        }

        /*ok, finished fetching symbols from script, now let's validate them */
	errors += validate_symbols (symblist, testclasses, class_methods, 
				    line_number, tc_title);
        /*now check if all interference objects have been stopped (this is
           very important!) */
	errors += check_interference_objects (interf_objs, line_number,
					      tc_title);
	errors += check_classes_for_deletion (assoc_cnames, line_number,
					      tc_title);
	errors += check_classes_for_deletion (assoc_lnames, line_number,
					      tc_title);

      EXIT_VALIDATE:
        /* free all allocated memory */
        if (token != INITPTR)
                DELETE (token);

	do_cleanup (slaves, testclasses, class_methods, assoc_cnames,
		    assoc_lnames, requested_events, symblist,
		    var_list, interf_objs);


	if (errors) {
		DELETE (tc_title);
		tc_title = NULL;
	}

	*description = tx_get_buf (tx_desc);
	tx_destroy (&tx_desc);

        return tc_title;
}


/* ------------------------------------------------------------------------- */
/** Handle current script command and move to next.
 *  @return ENOERR in case of success, -1 otherwise.
 */
int interpreter_next ()
{
        LegoActiveType *active = INITPTR;
        LegoPassiveType *passive = INITPTR;
        LegoLoopType   *loop = INITPTR;
        LegoEndloopType *eloop = INITPTR;
        LegoIfBlockType *ifblock = INITPTR;
        LegoElseBlockType *elseblock = INITPTR;

        int             retval = ENOERR;
        struct timeval  t;
        struct timeval  now;
        DLListIterator  it = DLListNULLIterator;
        struct define  *def = INITPTR;

        if (current == INITPTR) {
                current = mli_get_legosnake ();
        }
        if (current == INITPTR) {
                retval = -1;
                goto EXIT;
        }

        switch (current->type_) {
        case ELegoStart:
                MIN_DEBUG ("ELegoStart - Script Started");
                break;
        case ELegoEnd:
                MIN_DEBUG ("ELegoEnd - Script Ended");
                script_finish ();
                break;
        case ELegoPassive:

                passive = (LegoPassiveType *) current;
                interpreter_handle_keyword (passive->keyword_,
                                            passive->line_);
                break;
        case ELegoActive:
                active = (LegoActiveType *) current;


                /* We are going to also replace defines on runtime
                 * because of sendrecieve which sets defines during
                 * script execution. */
                it = dl_list_head (defines);
                while (it != DLListNULLIterator) {
                        def = (struct define *)dl_list_data (it);
                        mip_replace (active->line_, def->label_, def->value_);
                        it = dl_list_next (it);
                }

                /* now we can interprete it */
                interpreter_handle_keyword (active->keyword_, active->line_);
                break;
        case ELegoLoop:
                loop = (LegoLoopType *) current;

                if (loop->timeout_ == ESTrue && loop->started_ == ESFalse) {
                        gettimeofday (&now, NULL);
                        loop->endtime_.tv_sec = loop->oryginal2_.tv_sec;
                        loop->endtime_.tv_sec += now.tv_sec;
                        loop->endtime_.tv_usec = loop->oryginal2_.tv_usec;
                        loop->endtime_.tv_usec += now.tv_usec;

                        while (loop->endtime_.tv_usec >= 1000000) {
                                loop->endtime_.tv_sec++;
                                loop->endtime_.tv_usec -= 1000000;
                        }
                }

                break;
        case ELegoEndloop:
                eloop = (LegoEndloopType *) current;

                /* Check mode of the loop:
                 * - timeout
                 * - iteration 
                 */
                if (eloop->beyondloop_ == INITPTR) {
                        eloop->beyondloop_ = eloop->next_;
                        eloop->next_ = (LegoBasicType *) eloop->loopback_;
                }

                if (eloop->timeout_ == ESFalse) {
                        /* Check range */
                        if (eloop->counter_ == 0) {
                                eloop->next_ =
                                    (LegoBasicType *) eloop->loopback_;
                                eloop->counter_++;
                        } else if (eloop->counter_ <
                                   ((*eloop->oryginal_) - 1)) {
                                /* Increment 'visited' counter */
                                eloop->counter_++;
                        } else {
                                eloop->next_ =
                                    (LegoBasicType *) eloop->beyondloop_;
                                eloop->counter_ = 0;
                        }
                } else {
                        *eloop->started_ = ESTrue;
                        gettimeofday (&now, NULL);

                        if (substract_timeval (&t, eloop->endtime_, &now) ==
                            1) {
                                /* loop last too long */
                                eloop->next_ =
                                    (LegoBasicType *) eloop->beyondloop_;
                                *eloop->started_ = ESFalse;
                        } else {
                                /* loop still can operate */
                                eloop->next_ =
                                    (LegoBasicType *) eloop->loopback_;
                        }
                }

                break;
        case ELegoBreakloop:
                /*
                ** Fast forward to endloop
                */
                while (current->type_ != ELegoEndloop)
                        current = current->next_;
                eloop = (LegoEndloopType *) current;
                if (eloop->beyondloop_ != INITPTR) {
                        current->next_ = eloop->beyondloop_;
                }
                break;
        case ELegoIfBlock:
                ifblock = (LegoIfBlockType *)current;
                if (eval_if (ifblock->condition_) == ESFalse) {
                        elseblock = (LegoElseBlockType *) ifblock->else_;
                        if (elseblock != INITPTR) {
                                current = (LegoBasicType *) elseblock;
                        } else {
                                current = (LegoBasicType *) ifblock->block_end_;
                        }
                }
                break;
        case ELegoElseBlock:
                break;
        case ELegoEndifBlock:
                break;
        default:
                MIN_ERROR ("Unknown Type of Lego Piece [%d]",
                             current->type_);
        }

        /* Move forward one element */
        current = current->next_;

      EXIT:
        return retval;
}
/* ------------------------------------------------------------------------- */
/** Inititialize scripter. 
 */
int scripter_init (minScripterIf * scripter_if)
{
        current = INITPTR;
        return 0;
}
/* ------------------------------------------------------------------------- */
/** Validates one line in test script.
 *  @param keyword keyword to be handled 
 *  @param line item parser which contains the rest of the line 
 *  @param line_number  line number for debug messages
 *  @param p_tc_title pointer to test case title string
 *  @param tx_desc test case description container
 *  @param assoc_lnames list of test method names
 *  @param assoc_cnames list of test class names
 *  @param symblist list of symbols 
 *  @param var_list list of variables
 *  @param requested_events list of already requested events
 *  @param slaves  list of slaves
 *  @param interf_objs list of interference objects
 *  @param nesting buffer to keep track of nesting types (if/loop)
 *  @param nest_level current level of nesting
 *  @return number of errors
 */
int validate_line (char *keyword, 
		   MinItemParser * line, 
		   int line_number,
		   char **p_tc_title,
		   Text *tx_desc,
		   DLList *assoc_lnames,
		   DLList *assoc_cnames,
		   DLList *symblist,
		   DLList *var_list,
		   DLList *requested_events,
		   DLList *slaves,
		   DLList *interf_objs,
		   char   *nesting,
		   int    *nest_level) 
{
	int errors = 0, i = 0, check_result = 0;
	char *tc_title = *p_tc_title;
        TSBool in_loop;
        unsigned int    len = 0;
        enum nesting_type {
                IF = 1,
                LOOP
        };

	switch (get_keyword (keyword)) {
	case EKeywordTitle:
		len = strlen (line->item_skip_and_mark_pos_);
		if (len == 0) {
			SCRIPTER_SYNTAX_ERROR ("title",
					       "Test case "
					       "title is not defined");
			errors ++;
                        }
		tc_title = NEW2 (char, len + 1);
		STRCPY (tc_title, line->item_skip_and_mark_pos_,
			len + 1);
		*p_tc_title = tc_title;
		break;
	case EKeywordSet:
	case EKeywordUnset:
	case EKeywordSkipIt:
		break;
	case EKeywordDescription:
		len = strlen (line->item_skip_and_mark_pos_);
		tx_c_append (tx_desc, " ");
		tx_c_append (tx_desc, line->item_skip_and_mark_pos_);
		break;
		
	case EKeywordCreate:
		/*class and dll names are writen to assoc_ lists */
		errors += check_create_line (line, 
					     line_number,
					     tc_title, 
					     assoc_lnames,
					     assoc_cnames,
					     symblist);
		break;

	case EKeywordDelete:
		errors += check_delete_line (line,
					     line_number,
					     tc_title,
					     assoc_lnames,
					     assoc_cnames);
		break;
		
	case EKeywordPause:
		errors = check_pause_line (line, line_number,
					   tc_title);
		break;
		
	case EKeywordLoop:
		check_result = check_loop_line (line, line_number,
							tc_title);
		if (check_result == 0) {
			*nest_level = *(nest_level) + 1;
			nesting [*nest_level] = LOOP;
		} else {
			errors ++;
		}
		break;
		
	case EKeywordEndloop:
		if (*nest_level <= 0 || nesting [*nest_level] != LOOP) {
			SCRIPTER_SYNTAX_ERROR ("endloop","unexpected");
			errors ++;
		} else
			*nest_level = *(nest_level) - 1;
		break;
			
	case EKeywordBreakloop:
		i = *nest_level;
		in_loop = ESFalse;
		while (i > 0) {
			if (nesting [i] == LOOP) {
				in_loop = ESTrue;
				break;
			}
			i --;
                        }
		if (in_loop == ESFalse) {
			SCRIPTER_SYNTAX_ERROR ("breakloop",
					       "no loop to break");
			errors ++;
		}
		break;
		
	case EKeywordRequest:
		errors += check_request_line (line, line_number, 
					      tc_title,
					      requested_events);
		break;
		
	case EKeywordRelease:
		errors += check_release_line (line, line_number, 
						      tc_title);
		break;
		
	case EKeywordWait:
		errors += check_wait_line (line, line_number, 
					   tc_title,
					   requested_events);
		
		break;
			
	case EKeywordClassName:
		errors += check_methodcall_line (line, 
						 line_number, 
						 tc_title,
						 keyword,
						 assoc_lnames,
						 assoc_cnames,
						 symblist);
		break;
	case EKeywordRun:
		errors += check_run_line (line, line_number, 
					  tc_title);
		break;
		
	case EKeywordPrint:
	case EKeywordResume:
	case EKeywordCancel:
	case EKeywordCancelIfError:
		/*we don't care about it at this point */
		break;
	case EKeywordAllocate:
		errors += check_allocate_line (line, line_number, 
					       slaves,
					       tc_title);
		break;

	case EKeywordFree:
		errors += check_free_line (line, line_number, 
					   slaves, tc_title);
                        break;
			
	case EKeywordRemote:
		errors += check_remote_line (line, var_list, 
					     line_number, slaves, 
					     tc_title);
		break;
		
	case EKeywordAllowNextResult:
		errors += check_allownextresult_line (line, line_number,
						      tc_title);
		break;
		
	case EKeywordComplete:
		errors += check_complete_line (line, line_number, 
						       tc_title);
		break;
		

	case EKeywordTimeout:
		errors += check_timeout_line (line, line_number,
					      tc_title);
		break;
		
	case EKeywordSleep:
		errors += check_sleep_line (line, line_number,
					    tc_title);
		break;
		
	case EKeywordVar:
		errors += check_var_line (line, line_number,
					  tc_title, var_list);
		break;
		
	case EKeywordSendreceive:
		errors += check_sendreceive_line (line, line_number,
						  tc_title);
		break;
	case EKeywordExpect:
		
		errors += check_expect_line (line, var_list, 
					     line_number,
						     tc_title);
		break;
		
	case EKeywordInterference:
		check_result =
			check_interference_line (line, line_number,
						 interf_objs,
						 tc_title);
		if (check_result != ENOERR) {
			MIN_ERROR ("Test Interference Fault");
			errors ++;
		}
		break;
		
	case EKeywordIf:
		errors += check_if_line (line, line_number,
					 tc_title);

		*nest_level = *(nest_level) + 1;
		nesting [*nest_level] = IF;
		break;
		
	case EKeywordElse:
		if (nesting [*nest_level] != IF) {
			SCRIPTER_SYNTAX_ERROR ("else",
					       "unexpected else");
			errors ++;
		}
		break;
		
	case EKeywordEndif:
		if (*nest_level <= 0 || nesting [*nest_level] != IF) {
			SCRIPTER_SYNTAX_ERROR ("endif",
					       "unexpected");
			errors ++;
		} else
			*nest_level = *(nest_level) -1;
		break;
		
	case EKeywordBlocktimeout:
		errors += check_blockingtimeout_line (line, 
						      line_number,
						      tc_title);
		break;
		
	case EKeywordUnknown:
		SCRIPTER_SYNTAX_ERROR_ARG ("<unknown>",
					   "[%s]", keyword);
		errors ++;
		break;
	default:
		SCRIPTER_SYNTAX_ERROR_ARG ("<unknown>",
					   "[%s]", keyword);
		errors ++;
		break;
	}
	return errors;
}
/* ------------------------------------------------------------------------- */
/** Handles keyword 
 *  @param keyword [in] keyword to be handled 
 *  @param mip [in] item parser which contains the rest of the line 
 *
 *  NOTE: Item parser should be rewinded after usage in order to be able
 *        to correctly handle loops.
 */
void interpreter_handle_keyword (TScripterKeyword keyword,
                                 MinItemParser * mip)
{
        char           *parser_pos = mip->item_skip_and_mark_pos_;
        char           *token = INITPTR;
        char           *token2 = INITPTR;
        char           *p;
        int             ival = -1;

        if (mip == INITPTR) {
                errno = EINVAL;
                goto EXIT;
        }

        /* Handle keyword */
        switch (keyword) {
        case EKeywordTitle:
                MIN_DEBUG ("EKeywordTitle");
                /* "title <testcasename>" */
		set_caller_name(mip->item_skip_and_mark_pos_);
                mip_get_next_string (mip, &token);
                break;
        case EKeywordSkipIt:
                MIN_DEBUG ("EKeywordSkipIt");
                /* "waittestclass <testcasename>" */
                mip_get_next_string (mip, &token);
                break;
        case EKeywordCreate:
                MIN_DEBUG ("EKeywordCreate");
                /* "create <dllname> <classname>" */
                mip_get_next_string (mip, &token);
                mip_get_next_string (mip, &token2);
                testclass_create (token, token2);
                break;
        case EKeywordClassName:
                MIN_DEBUG ("EKeywordClassName");
                /* "<classname> <functionname> [parameters]" */
                /* We need to goto beginning of the line to get the
                 * classname. */
                mip->item_skip_and_mark_pos_ = &mip->item_line_section_[0];
                mip_get_next_string (mip, &token);
                testclass_call_function (token, mip);
                break;
        case EKeywordDelete:
                MIN_DEBUG ("EKeywordDelete");
                /* "delete <classname>" */
                mip_get_next_string (mip, &token);
                testclass_destroy (token);
                break;
        case EKeywordPause:
                MIN_DEBUG ("EKeywordPause");
                mip_get_next_string (mip, &token);
                test_pause (token, mip);
                break;
        case EKeywordPrint:
                MIN_DEBUG ("EKeywordPrint");
                /* "print <text>" */
                testclass_print (mip);
                break;
        case EKeywordResume:
                MIN_DEBUG ("EKeywordResume");
                /* "resume <testid>" */
                mip_get_next_string (mip, &token);
                test_resume (token);
                break;
        case EKeywordCancel:
                MIN_DEBUG ("EKeywordCancel");
                /* "cancel <testid>" */
                mip_get_next_string (mip, &token);
                test_cancel (token);
                break;
        case EKeywordRelease:
                MIN_DEBUG ("EKeywordRelease");
                /* "release <eventname>" */
                mip_get_next_string (mip, &token);
                event_release (token);
                break;
        case EKeywordRequest:
                MIN_DEBUG ("EKeywordRequest");
                /* "request <eventname> [state]" */
                mip_get_next_string (mip, &token);
                mip_get_next_string (mip, &token2);
                if (token2 != INITPTR && !strcmp (token2, "state"))
                        event_request (token, 1);
                else
                        event_request (token, 0);

                break;
        case EKeywordRun:
                MIN_DEBUG ("EKeywordRun");
                /* "run <modulename> <config> <id> " */
                mip_get_next_string (mip, &token);
                mip_get_next_string (mip, &token2);
                mip_get_next_int (mip, &ival);
                test_run (token, token2, ival, mip);
                break;
        case EKeywordSet:
                MIN_DEBUG ("EKeywordSet");
                /* "set <eventname> [state]" */
                mip_get_next_string (mip, &token);
                mip_get_next_string (mip, &token2);
                if (token2 != INITPTR && !strcmp (token2, "state"))
                        event_set (token, 1);
                else
                        event_set (token, 0);
                break;
        case EKeywordUnset:
                MIN_DEBUG ("EKeywordUnset");
                /* "unset <eventname>" */
                mip_get_next_string (mip, &token);
                event_unset (token);
                break;
        case EKeywordWait:
                MIN_DEBUG ("EKeywordWait");
                /* "wait <eventname>" */
                mip_get_next_string (mip, &token);
                event_wait (token);
                break;
        case EKeywordAllocate:
                MIN_DEBUG ("EKeywordAllocate");
                /* "allocate phone <slave_name> */
                mip_get_next_string (mip, &token);
                mip_get_next_string (mip, &token2);
                test_allocate_slave (token, token2);
                break;
        case EKeywordFree:
                MIN_DEBUG ("EKeywordFree");
                /* free <slave_name> */
                mip_get_next_string (mip, &token);
                test_free_slave (token);
                break;
        case EKeywordRemote:
                MIN_DEBUG ("EKeywordRemote");
                /* remote <slave_name> <command> */
                mip_get_next_string (mip, &token);
                test_remote_exe (token, mip);
                break;
        case EKeywordAllowNextResult:
                MIN_DEBUG ("EKeywordAllowNextResult");
                /* allownextresult <result> [result2] ... [resultN] */
                mip_get_next_int (mip, &ival);
                testclass_allownextresult (ival, mip);
                break;
        case EKeywordComplete:
                MIN_DEBUG ("EKeywordComplete");
                /* complete <testid> */
                mip_get_next_string (mip, &token);
                test_complete (token);
                break;
        case EKeywordCancelIfError:
                MIN_DEBUG ("EKeywordCancelIfError");
                /* canceliferror */
                test_canceliferror ();
                break;
        case EKeywordTimeout:
                MIN_DEBUG ("EKeywordTimeout");
                /* timeout <interval in ms> */
                mip_get_next_int (mip, &ival);
                testclass_test_timeout ((unsigned long)ival);
                break;
        case EKeywordSleep:
                MIN_DEBUG ("EKeywordSleep");
                /* sleep <interval in ms> */
                mip_get_next_int (mip, &ival);
                testclass_test_sleep ((unsigned long)ival);
                break;
        case EKeywordVar:
                MIN_DEBUG ("EKeywordVar");
                /* var <name> [value] */
                mip_get_next_string (mip, &token);
                mip_get_next_string (mip, &token2);
                if (token2 != INITPTR) {
                        declare_var (token, ESTrue, token2);
                } else {
                        declare_var (token, ESFalse, NULL);
                }
                token = token2 = INITPTR;
                break;
        case EKeywordSendreceive:
                MIN_DEBUG ("EKeywordSendrecieve");
                /* sendreceive <variable>=<value> */
                mip_get_next_string (mip, &token);
                p = strchr (token, '=');
		if (p != NULL) {
			*p = '\0';
			p++;
			sendreceive_slave_send (token, p);
		} else {
			MIN_WARN ("Sendreceive error");
		}
                break;
        case EKeywordExpect:
                MIN_DEBUG ("EKeywordExpect");
                /* expect <variable> */
                mip_get_next_string (mip, &token);
                sendreceive_slave_expect (token);
                token = INITPTR;
                break;
        case EKeywordInterference:
                MIN_DEBUG ("EKeywordInterference");
                test_interference (mip);
                break;
	case EKeywordBlocktimeout:
		MIN_DEBUG ("EKeywordBlocktimeout");
                mip_get_next_int (mip, &ival);
                set_block_timeout ((unsigned long)ival);
		break;

        case EKeywordUnknown:
                MIN_WARN ("Unknown keyword [INITPTR]");
                break;
        default:
                mip_get_next_string (mip, &token);
                MIN_WARN ("Unknown keyword [%s]", token);
                break;
        }
      EXIT:
        /* Clean-up */
        if (token != INITPTR) {
                DELETE (token);
                token = INITPTR;
        }
        if (token2 != INITPTR) {
                DELETE (token2);
                token2 = INITPTR;
        }
        /* Rewind item parser */
        mip->item_skip_and_mark_pos_ = parser_pos;
        return;
}
/* ------------------------------------------------------------------------- */
/** return  test module type */
unsigned int get_module_type()
{ return module_type; }
/* ------------------------------------------------------------------------- */
/** return test module template version */
unsigned int get_module_version()
{ return module_version; }
/* ------------------------------------------------------------------------- */
/** return build date */
char* get_module_date()
{ return module_date; }
/* ------------------------------------------------------------------------- */
/** return build time */
char* get_module_time()
{ return module_time; }
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_scripter_if.tests"
#endif                          /* MIN_UNIT_TEST */
/* End of file */
