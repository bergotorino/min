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
//extern char *strcasestr (const char *haystack,const char *needle);
//extern char *strchr(const char *s, int c);

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* ------------------------------------------------------------------------- */
/** list of allocated slaves. Used during the validation process. */
static LegoBasicType *current = INITPTR;
/** used for validation of stopping interference */
DLList         *interf_objs = INITPTR;
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
/** Handles keyword 
 *  @param keyword [in] keyword to be handled 
 *  @param mip [in] item parser which contains the rest of the line 
 *
 *  NOTE: Item parser should be rewinded after usage in order to be able
 *        to correctly handle loops.
 */
LOCAL void      interpreter_handle_keyword (TScripterKeyword keyword,
                                            MinItemParser * mip);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "run" keyword
 * @param line - min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number - line number for debug messages
 * @return 0 if line is valid*/
LOCAL int       check_run_line (MinItemParser * line, int line_number);
/* ------------------------------------------------------------------------- */
/** Checks validitye of line with "pause" keyword
 * @param line - min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number - line number for debug messages
 * @return 0 if line is valid*/
LOCAL int       check_pause_line (MinItemParser * line, int line_number);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "loop" keyword
 * @param line - min item parser containing line. Assume that 
 * mip_get_line was executed once to extract first keyword
 * @param line_number - line number for debug messages
 * @return 0 if line is valid*/
LOCAL int       check_loop_line (MinItemParser * line, int line_number);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "allocate" keyword
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param slaves [in] list of slaves
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int       check_allocate_line (MinItemParser * line, int line_number,
				     DLList * slaves);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "free" keyword
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @param slaves [in] list of slaves
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int       check_free_line (MinItemParser * line, int line_number,
				 DLList * slaves);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "remote" keyword
 *  @param line [in] MinItemParser containing line.
 *  @param variables [in] list of variables
 *  @param line_number - line number for debug messages
 *  @param slaves [in] list of slaves
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int       check_remote_line (MinItemParser * line, DLList * variables,
                                   int line_number, DLList * slaves);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "allownextresult" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int       check_allownextresult_line (MinItemParser * line,
                                            int line_number);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "complete" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int       check_complete_line (MinItemParser * line, int line_number);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "timeout" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int       check_timeout_line (MinItemParser * line, int line_number);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "sleep" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int       check_sleep_line (MinItemParser * line, int line_number);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "testinterference" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int       check_interference_line (MinItemParser * line,
                                         int line_number);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "expect" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @return ENOERR if line is valid, -1 otherwise. 
 *
 *  NOTE: mip_get_line was executed once to extract first keyword. 
 */
LOCAL int       check_expect_line (MinItemParser * line, DLList * varnames,
                                   int line_number);
/* ------------------------------------------------------------------------- */
/** Checks validity of line with "if" keyword 
 *  @param line [in] MinItemParser containing line.
 *  @param line_number - line number for debug messages
 *  @return ENOERR if line is valid, -1 otherwise. 
 */
LOCAL int       check_if_line (MinItemParser * line, int line_number);

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL int check_expect_line (MinItemParser * line, DLList * varnames,
                             int line_number)
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
                MIN_ERROR ("Eexpect keyword syntax error in line %d "
                            "- Expecting undeclared variable",line_number);
                retval = -1;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL int look4slave (const void *a, const void *b)
{
        return strcmp ((char *)a, (char *)b);
}

/* ------------------------------------------------------------------------- */
LOCAL void interpreter_handle_keyword (TScripterKeyword keyword,
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
                *p = '\0';
                p++;
                sendreceive_slave_send (token, p);
                break;
        case EKeywordExpect:
                MIN_DEBUG ("EKeywordExpect");
                /* expect <variable> */
                mip_get_next_string (mip, &token);
                sendreceive_slave_expect (token);
                token = INITPTR;
                break;
        case EKeywordInterference:
                test_interference (mip);
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
LOCAL int check_run_line (MinItemParser * line, int line_number)
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
                MIN_ERROR ("run keyword syntax error, line: %d -"
                            "test module is not defined",line_number);
                goto EXIT;
        }

        opresult = mip_get_next_string (line, &cfg_name);
        if (opresult != 0) {
                MIN_ERROR ("run keyword syntax error, line: %d -"
                            "test case cfg file is not defined",line_number);
                goto EXIT;
        }

        opresult = mip_get_int (line, "", &case_id);
        if (case_id < 0) {
                opresult = -1;
                MIN_ERROR ("run keyword syntax error, line: %d -"
                            "test case id is not defined",line_number);
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
                opresult = -1;
                MIN_ERROR ("run keyword syntax error, line: %d -"
                            "library not opened",line_number,lib_name);
                goto EXIT;
        }

        run_case = (ptr2run) dlsym (dll_handle, "tm_run_test_case");
        if (run_case == NULL) {
                MIN_ERROR ("run keyword syntax error, line: %d -"
                            "run_case unresolved in %s, error: %s",
                            line_number,lib_name,dlerror());
                opresult = -1;
                goto EXIT;
        }

        get_cases = (ptr2case) dlsym (dll_handle, "tm_get_test_cases");
        if (get_cases == NULL) {
                MIN_ERROR ("run keyword syntax error, line: %d -"
                            "faulty test library %s,",line_number,lib_name);
                dlclose (dll_handle);
                goto EXIT;
        }

        opresult = get_cases (cfg_name, &module_cases);
        if ((dl_list_size (module_cases) == 0) || (opresult != 0)) {

                MIN_WARN ("Failed to get test cases from module %s",
                           lib_name);
                dlclose (dll_handle);
                goto EXIT;
        }
        if (case_title_given == ESFalse) {
                if ((dl_list_size (module_cases) + 1) >= case_id) {
                        opresult = 0;
                } else {
                        MIN_ERROR ("run keyword syntax error, line: %d "
                                    ". Test case id out of range.", 
                                   line_number);
                        opresult = -1;
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
                        opresult = -1;
                        MIN_ERROR ("run keyword syntax error in line %d"
                                    ". Case with matching title not found.",
                                    line_number);
                        goto EXIT;
                }
        }
      EXIT:
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
LOCAL int check_pause_line (MinItemParser * line, int line_number)
{
        char           *string = INITPTR;
        int             result = -1;

        result = mip_get_next_string (line, &string);
        if (result != 0) {
                MIN_ERROR ("pause keyword syntax error in line %d"
                            ". Test id is not defined.",line_number);
                return -1;
        }
        DELETE (string);
        return 0;
}

/*------------------------------------------------------------------------- */
LOCAL int check_loop_line (MinItemParser * line, int line_number)
{
        int             param1 = 0;
        char           *opt_msec = NULL;
        int             result = -1;

        /*check value of int after "loop" */
        result = mip_get_int (line, "", &param1);
        if (result != 0) {
                MIN_ERROR ("loop keyword syntax error in line %d"
                            ". Number of iterations/time is not defined.",
                            line_number);
                return (-1);
        }

        /* check if the "msec" is stated */
        result = mip_get_next_string (line, &opt_msec);
        if (result == 0) {
                if (strcmp (opt_msec, "msec") == 0) {
                        return 0;
                } else {
                        MIN_ERROR ("loop keyword syntax error in line %d"
                                    ". Wrong keyword in loop statement.",
                                    line_number);
                        return (-1);
                }
        }
        return 0;
}

/*------------------------------------------------------------------------- */
LOCAL int check_allocate_line (MinItemParser * line, int line_number,
			       DLList * slaves)
{
        int             retval = ENOERR;
        int             result = 0;
        char           *token = INITPTR;
        DLListIterator  it = DLListNULLIterator;

        /* Check syntax: allocate <slave type> <slave name> */
        result = mip_get_next_string (line, &token);
        if (result != ENOERR) {
                MIN_ERROR ("allocate keyword syntax error in line %d"
                            ". Slave type is not defined.",line_number);
                retval = -1;
                goto EXIT;
        }
        if (strcmp (token, "phone") != 0) {
                MIN_ERROR ("allocate keyword syntax error in line %d"
                            ". Slave type differs from phone.",line_number);
                DELETE (token);
                retval = -1;
                goto EXIT;
        }
        DELETE (token);
        result = mip_get_next_string (line, &token);
        if (result != ENOERR) {
                MIN_ERROR ("allocate keyword syntax error in line %d"
                            ". Slave name is not defined.",line_number);
                retval = -1;
                goto EXIT;
        }

        /* Check if slave of this name was already allocated. */
        it = dl_list_find (dl_list_head (slaves)
                           , dl_list_tail (slaves)
                           , look4slave, (const void *)token);
        if (it != DLListNULLIterator) {
                MIN_ERROR ("allocate keyword syntax error in line %d"
                            ". Slave with selected name already exists.",
                            line_number);
                retval = -1;
                DELETE (token);
                goto EXIT;
        }

        /* If slave not allocated than add it to the allocated slaves list. */
        dl_list_add (slaves, (void *)token);
      EXIT:
        return retval;
}

/*------------------------------------------------------------------------- */
LOCAL int check_free_line (MinItemParser * line, int line_number,
			   DLList *slaves)
{
        int             retval = ENOERR;
        int             result = 0;
        char           *token = INITPTR;
        DLListIterator  it = DLListNULLIterator;

        /* Check syntax. */
        result = mip_get_next_string (line, &token);
        if (result != ENOERR) {
                retval = -1;
                DELETE (token);
                MIN_ERROR ("free keyword syntax error in line %d"
                            ". Slave name is not defined.",line_number);
                goto EXIT;
        }

        /* Check if slave of this name was already allocated. */
        it = dl_list_find (dl_list_head (slaves)
                           , dl_list_tail (slaves)
                           , look4slave, (const void *)token);
        if (it == DLListNULLIterator) {
                retval = -1;
                DELETE (token);
                MIN_ERROR ("free keyword syntax error in line %d"
                            ". Slave not allocated.",line_number);
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
LOCAL int check_remote_line (MinItemParser * line, DLList * variables,
                             int line_number, DLList * slaves)
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
                retval = -1;
                DELETE (token);
                MIN_ERROR ("remote keyword syntax error in line %d"
                            ". Slave name is not defined.",line_number);
                goto EXIT;
        }
        /* Check if slave of this name was already allocated. */
        it = dl_list_find (dl_list_head (slaves)
                           , dl_list_tail (slaves)
                           , look4slave, (const void *)token);
        if (it == DLListNULLIterator) {
                retval = -1;
                DELETE (token);
                MIN_ERROR ("remote keyword syntax error in line "
                            ". Slave not allocated.",line_number);
                goto EXIT;
        }
        DELETE (token);

        /* Go back to syntax checking */
        result = mip_get_next_string (line, &token);
        if (result != ENOERR) {
                retval = -1;
                DELETE (token);
                MIN_ERROR ("remote keyword syntax error in line %d"
                            ". Remote command is not defined",line_number);
                goto EXIT;
        }

        command = get_keyword (token);
        switch (command) {
        case EKeywordSendreceive:
                mip_get_next_string (line, &arg);
                if (strchr (arg, '=') == NULL) {
                        MIN_ERROR ("remote keyword syntax error in line %d"
                                    ". Wrong argument for remote sendreceive"
                                    " (no \"=\").",
                                    line_number);
                        retval = -1;
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
                        MIN_ERROR ("remote keyword syntax error in line %d. "
                                   "Remote expect for unknown var.",
                                   line_number);
                        retval = -1;
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
LOCAL int check_allownextresult_line (MinItemParser * line, int line_number)
{
        int             retval = ENOERR;
        int             tmp = 0;

        if (line == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_int (line, &tmp);

        if (retval != ENOERR) {
                MIN_ERROR ("allownextresult keyword syntax error in line %d"
                           ". Result code is not defined.", line_number);
                retval = -1;
                goto EXIT;
        }
      EXIT:
        return retval;
}

/*------------------------------------------------------------------------- */
LOCAL int check_complete_line (MinItemParser * line, int line_number)
{
        int             retval = ENOERR;
        char           *token = INITPTR;

        if (line == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_string (line, &token);

        /* Check syntax */
        if (retval != ENOERR) {
                MIN_ERROR ("complete keyword syntax error in line %d"
                            ". Testid is not defined.",line_number);
                retval = -1;
                goto EXIT;
        }

        DELETE (token);
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL int check_timeout_line (MinItemParser * line, int line_number)
{
        int             retval = ENOERR;
        int             tmp = 0;

        if (line == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_int (line, &tmp);

        /* Check syntax */
        if (retval != ENOERR) {
                MIN_ERROR ("timeout keyword syntax error in line %d"
                            ". Timeout interval is not defined.", line_number);
                retval = -1;
                goto EXIT;
        }
        if (tmp < 1) {
                MIN_ERROR ("timeout keyword syntax error in line %d"
                            ". Invalid interval value.", line_number);
                retval = -1;
                goto EXIT;
        }

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL int check_interference_line (MinItemParser * line, int line_number)
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
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }
        /*first check if sp-stress is available at all
           this will be done by checking if execs exist in 
           /usr/bin */
        if ((access ("/usr/bin/cpuload", X_OK) != 0) ||
            (access ("/usr/bin/memload", X_OK) != 0) ||
            (access ("/usr/bin/ioload", X_OK))) {
                MIN_ERROR ("Testinterference  error in line %d"
                            ". Check if sp-stress is installed properly.",
                            line_number);
                retval = -1;
                goto EXIT;
        }
        retval = mip_get_next_string (line, &name);

        /* check syntax */
        if (retval != ENOERR) {
                MIN_WARN ("Testinterference keyword syntax error in line %d",
                          line_number);
                retval = -1;
                goto EXIT;
        }

        retval = mip_get_next_string (line, &command);

        if (retval != ENOERR) {
                MIN_WARN ("Testinterference keyword syntax error in line %d",
                           ". Command not specified",line_number);
                retval = -1;
                goto EXIT;
        }
        if (strcasecmp (command, "start") == 0) {

                retval = mip_get_next_string (line, &type);
                if (retval != ENOERR) {
                        MIN_WARN ("Testinterference keyword syntax error in "
                                  "line %d. Type not specified", line_number);
                        retval = -1;
                        goto EXIT;
                }
                if ((strcmp (type, "cpuload") != 0) &&
                    (strcmp (type, "memload") != 0) &&
                    (strcmp (type, "ioload") != 0)) {
                        MIN_WARN ("Testinterference keyword syntax error in "
                                  "line %d. Type not specified correctly",
                                  line_number);
                        retval = -1;
                        goto EXIT;
                }
                retval = mip_get_next_int (line, &value);

                if (retval != ENOERR) {
                        MIN_WARN ("Testinterference keyword syntax error in "
                                  "line %d. Value not specified", line_number);
                        retval = -1;
                        goto EXIT;
                }
                retval = mip_get_next_int (line, &idle_time);

                if (retval != ENOERR) {
                        MIN_WARN ("Testinterference keyword syntax error in "
                                  "line %d. Idle time not specified",
                                  line_number);
                        retval = -1;
                        goto EXIT;
                }
                retval = mip_get_next_int (line, &busy_time);

                if (retval != ENOERR) {
                        MIN_WARN ("Testinterference keyword syntax error in "
                                  "line %d. Busy time not specified",
                                  line_number);
                        retval = -1;
                        goto EXIT;
                }
                retval = ENOERR;
                if (interf_objs == INITPTR) {
                        interf_objs = dl_list_create ();
                }

                tmp = NEW2 (char, strlen (name) + 1);
                sprintf (tmp, "%s", name);
                dl_list_add (interf_objs, (void *)tmp);
        } else if (strcasecmp (command, "stop") == 0) {
                if (interf_objs == INITPTR) {
                        retval = -1;
                        MIN_WARN ("Testinterference keyword syntax error "
                                  "in line %d. No interferences started at "
                                  "this point",
                                  line_number);
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
                        retval = -1;
                        MIN_WARN ("Testinterference keyword syntax error in "
                                  "line %d. No such interference started at "
                                  "this point",
                                  line_number);
                        goto EXIT;
                } else {
                        tmp = (char *)dl_list_data (work_inter);
                        DELETE (tmp);
                        dl_list_remove_it (work_inter);

                }
                retval = ENOERR;
                goto EXIT;
        } else {
                retval = -1;
                MIN_WARN ("Testinterference keyword syntax error in line %d. "
                          "Wrong command name", line_number);
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
LOCAL int check_sleep_line (MinItemParser * line, int line_number)
{
        int             retval = ENOERR;
        int             tmp = 0;

        if (line == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_int (line, &tmp);

        /* check syntax */
        if (retval != ENOERR) {
                MIN_ERROR ("sleep keyword syntax error in line %d"
                            ". Sleep interval is not defined", line_number);
                retval = -1;
                goto EXIT;
        }
        if (tmp < 1) {
                MIN_ERROR ("sleep keyword syntax error in line %d"
                           ". Invalid interval value", line_number);
                retval = -1;
                goto EXIT;
        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL int check_if_line (MinItemParser * line, int line_number)
{
        int retval;
        char *token = INITPTR;

        if (line == INITPTR) {
                retval = -1;
                errno = EINVAL;
                goto EXIT;
        }

        retval = mip_get_next_string (line, &token);
        if (retval != ENOERR) {
                MIN_ERROR ("if keyword syntax error in line %d. "
                           "No if condition specified", line_number);
        }
        DELETE (token);

EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
TSBool validate_define (MinSectionParser * define)
{
        TSBool          retval = ESTrue;
        MinItemParser *line = INITPTR;
        char           *str1 = INITPTR;
        char           *str2 = INITPTR;
        int             pom = 0;
        unsigned int    lineno = 1;

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
                        MIN_ERROR ("Syntax error on line %d of Define section"
                                    " - idenifier was expected", lineno);
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
                        MIN_ERROR ("Syntax error on line %d of Define section"
                                    " - definition value was expected", lineno);
                        retval = ESFalse;
                        mip_destroy (&line);
                        goto EXIT;
                }

                pom = mip_get_next_string (line, &str1);
                if (pom == ENOERR) {
                        DELETE (str1);
                        MIN_ERROR
                            ("Syntax warning on line %d of Define section"
                             " unexpected literal", lineno);
                }

                mip_destroy (&line);
                line = mmp_get_next_item_line (define);
                lineno++;
        }

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
char           *validate_test_case (MinSectionParser * testcase)
{
        MinItemParser *line = INITPTR;
        char           *token = INITPTR;
        int             line_number = -1;       // Line number in test case

        TestClassDetails *test_class = INITPTR;
        /*association lists, assoc_cnames holds class names defined
          in script, assoc_lnames holds library namse in corresponding
          positions */
        DLList         *assoc_cnames = dl_list_create ();
        DLList         *assoc_lnames = dl_list_create ();
        int             loopcounter = 0;
        int             check_result = -1;
        int             lib_already_found;
        char           *ass_lname = NULL;
        char           *ass_evname = NULL;
        char           *ass_cname = NULL;
        char           *libname = NULL;
        char           *classname = NULL;
        char           *callname = NULL;
        char           *symb_callname = NULL;
        char           *libpath = NULL;
        char           *varname = NULL;
        char           *ass_varname = NULL;
        ScripterDataItem *library;
        DLListIterator  library_item = DLListNULLIterator;
        DLListIterator  var_item = DLListNULLIterator;
        DLList         *symblist = dl_list_create ();
        DLList         *var_list = dl_list_create ();   /*holds "variable"
                                                          names, for validating
                                                          var and expect 
                                                          keywords */
        DLList         *class_methods = INITPTR;
        DLListIterator  call_item = INITPTR;
        DLListIterator  tc_item = INITPTR;
	DLList         *slaves;
        TestCaseInfo   *tc;
        char           *tc_title = NULL;
        DLList         *testclasses = dl_list_create ();
        void           *dll_handle = NULL;
        unsigned int    len = 0;
        /* this will hold names of requested events, 
         * to validate "wait" statements */
        DLList         *requested_events = dl_list_create ();
        enum            nesting_type {
                IF = 1,
                LOOP
        };
        int             nest_level = 0;
        char            nesting [255];
        TSBool          in_loop;

        /* allocate place for allocated slaves. */
        slaves = dl_list_create ();
        line = mmp_get_item_line (testcase, "", ESNoTag);
        //mmp_get_line(testcase,"",&line,ESNoTag);
        while (line != INITPTR) {
                line_number++;  // increment line number
                line->parsing_type_ = EQuoteStyleParsing;

                if (token != INITPTR)
                        DELETE (token);
                mip_get_string (line, "", &token);
                switch (get_keyword (token)) {
                case EKeywordSet:
                case EKeywordUnset:
                case EKeywordSkipIt:
                        /* currently this handles set and unset lines
                           since we are not really able to validate them */
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordTitle:
                        len = strlen (line->item_skip_and_mark_pos_);
                        if (len == 0) {
                                MIN_ERROR ("title keyword syntax error in line"
                                           " %d. Test case title is not "
                                           "defined",
                                            line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        tc_title = NEW2 (char, len + 1);
                        STRCPY (tc_title, line->item_skip_and_mark_pos_,
                                len + 1);
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordCreate:
                        /*class and dll names are writen to assoc_ lists */
                        if (mip_get_next_string (line, &libname) != 0) {
                                MIN_ERROR ("createx keyword syntax error in "
                                           "line %d. Library name is not "
                                           "defined.",
                                           line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        ass_lname = NEW2 (char, strlen (libname) + 1);
                        sprintf (ass_lname, "%s", libname);
                        DELETE (libname);
                        dl_list_add (assoc_lnames, (void *)ass_lname);
                        if (mip_get_next_string (line, &classname) != 0) {
                                MIN_ERROR ("createx keyword syntax error in "
                                           "line %d. Class name is not "
                                           "defined.", line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        ass_cname = NEW2 (char, strlen (classname) + 1);
                        sprintf (ass_cname, "%s", classname);
                        dl_list_add (assoc_cnames, (void *)ass_cname);
                        DELETE (classname);
                        lib_already_found = 0;
                        library_item = dl_list_head (symblist);
                        while (library_item != DLListNULLIterator) {
                                library = (ScripterDataItem *)
                                    dl_list_data (library_item);
                                if (strcmp (library->DLL_name_, ass_lname) ==
                                    0) {
                                        lib_already_found = 1;
                                        break;
                                }
                                library_item = dl_list_next (library_item);
                        }
                        if (lib_already_found == 0) {
                                library =
                                    scripter_dsa_create (ass_lname, ass_cname,
                                                         EDLLTypeClass);
                                scripter_dsa_add (symblist, library);
                        }
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordDelete:
                        if ( mip_get_next_string(line,&classname) != 0 ){
                                MIN_ERROR ("delete keyword syntax error in "
                                           "line %d. No classname to delete "
                                           "specified.",
                                           line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        check_result = -1;      /*indicate if class was 
                                                  created before */
                        for (loopcounter = 0;
                             loopcounter < dl_list_size (assoc_cnames);
                             loopcounter++) {
                                ass_cname =
                                    (char *)
                                    dl_list_data (dl_list_at
                                                  (assoc_cnames,
                                                   loopcounter));
                                if (strcmp (ass_cname, classname) == 0) {
                                        check_result = 0;
                                        break;
                                }
                        }
                        if (check_result !=0 ){
                                MIN_ERROR ("delete keyword syntax error in "
                                           "line %d. Classname not created.", 
                                           line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                DELETE (classname);
                                goto EXIT_VALIDATE;
                        }
                        /*ok, test class was created previously, remove 
                           information from assoc. lists */
                        DELETE (ass_cname);
                        DELETE (classname);
                        dl_list_remove_it (dl_list_at
                                           (assoc_cnames, loopcounter));
                        ass_lname =
                            (char *)
                            dl_list_data ((dl_list_at
                                           (assoc_lnames, loopcounter)));
                        DELETE (ass_lname);
                        dl_list_remove_it (dl_list_at
                                           (assoc_lnames, loopcounter));
                        /*this way, by the end of validation, assoc_cnames & 
                          assoc_lnames lists should be empty. 
                          If they are not, validation fails */
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordPause:
                        check_result = check_pause_line (line, line_number);
                        if (check_result != 0) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordLoop:
                        check_result = check_loop_line (line, line_number);
                        if (check_result == 0) {
                                nest_level++;
                                nesting [nest_level] = LOOP;
                        } else {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordEndloop:
                        if (nest_level <= 0 || nesting [nest_level] != LOOP) {
                                MIN_ERROR ("Syntax error in line %d. "
                                           "Unexpected endloop.", line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        nest_level--;
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordBreakloop:
                        loopcounter = nest_level;
                        in_loop = ESFalse;
                        while (loopcounter > 0) {
                                if (nesting [loopcounter] == LOOP) {
                                        in_loop = ESTrue;
                                        break;
                                }
                                loopcounter --;
                        }
                        if (in_loop == ESFalse) {
                                MIN_ERROR ("Syntax error in line %d. "
                                           "No loop to break!.", line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordRequest:
                        check_result = mip_get_next_string (line, &callname);
                        if (check_result != 0) {
                                MIN_ERROR ("request keyword syntax error in "
                                           "line %d. Event name is not "
                                           "defined.",line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        ass_evname = NEW2 (char, strlen (callname) + 1);
                        sprintf (ass_evname, "%s", callname);
                        dl_list_add (requested_events, (void *)ass_evname);
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordRelease:
                        check_result = mip_get_next_string (line, &callname);
                        if (check_result != 0) {
                                MIN_ERROR ("release keyword syntax error in "
                                           "line %d. Event name is not "
                                           "defined.", line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                DELETE (callname);
                                goto EXIT_VALIDATE;
                        }
                        DELETE (callname);
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordWait:
                        check_result = mip_get_next_string (line, &callname);
                        if (check_result != 0) {
                                MIN_ERROR ("wait keyword syntax error in "
                                           "line %d. Event name is "
                                           "not defined.", line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        call_item = dl_list_head (requested_events);
                        if (call_item == DLListNULLIterator) {
                                MIN_ERROR ("wait keyword syntax error in line"
                                           " %d. Wait for not requested event.",
                                           line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        while (call_item != DLListNULLIterator) {
                                ass_evname = (char *)dl_list_data (call_item);
                                if (strcmp (ass_evname, callname) == 0) {
                                        break;
                                }
                                call_item = dl_list_next (call_item);
                                if (call_item == DLListNULLIterator) {
                                        MIN_ERROR ("wait keyword syntax error "
                                                   "in line %d. Wait for not "
                                                   "requested event.",
                                                   line_number);
                                        DELETE (tc_title);
                                        tc_title = NULL;
                                        goto EXIT_VALIDATE;

                                }

                        }
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordClassName:
                        ass_lname = NULL;
                        for (loopcounter = 0;
                             loopcounter < dl_list_size (assoc_cnames);
                             loopcounter++) {
                                ass_cname = (char *)
                                    dl_list_data (dl_list_at
                                                  (assoc_cnames,
                                                   loopcounter));
                                if (strcmp (ass_cname, token) == 0) {
                                        ass_lname = (char *)
                                            dl_list_data (dl_list_at
                                                          (assoc_lnames,
                                                           loopcounter));
                                        break;
                                }
                        }

                        if (ass_lname == NULL) {
                                MIN_ERROR ("syntax error in line %d. Call for "
                                           "method of non existing class.",
                                           line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        };

                        mip_get_next_string (line, &callname);
                        if (callname == NULL) {
                                MIN_ERROR ("syntax error in line %d"
                                           ". Method name is not defined.", 
                                           line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        symb_callname = NEW2 (char, strlen (callname) + 1);
                        STRCPY (symb_callname, callname,
                                strlen (callname) + 1);
                        DELETE (callname);
                        library_item = dl_list_head (symblist);

                        lib_already_found = 0;
                        while (library_item != DLListNULLIterator) {
                                library = (ScripterDataItem *)
                                    dl_list_data (library_item);
                                if (strcmp (ass_lname, library->DLL_name_) ==
                                    0) {
                                        lib_already_found = 1;
                                        scripter_dsa_add_symbol (library_item,
                                                                 symb_callname);
                                        break;
                                }
                                library_item = dl_list_next (library_item);
                        }
                        if (lib_already_found == 0) {
                                MIN_ERROR ("Syntax error in line %d"
                                            ". Selected library was not found.",
                                            line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        break;
                case EKeywordRun:
                        check_result = check_run_line (line, line_number);
                        mip_destroy (&line);

                        if (check_result != 0) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordPrint:
                        /*we don't care about it at this point */
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordResume:
                        /*we don't care about it at this point */
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordCancel:
                        /*we don't care about it at this point */
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordAllocate:
                        check_result =
				check_allocate_line (line, line_number, slaves);
                        mip_destroy (&line);

                        if (check_result != ENOERR) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordFree:
                        check_result = check_free_line (line, line_number, 
							slaves);
                        mip_destroy (&line);

                        if (check_result != ENOERR) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordRemote:

                        check_result =
				check_remote_line (line, var_list, line_number,
						   slaves);
                        mip_destroy (&line);

                        if (check_result != ENOERR) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordAllowNextResult:
                        check_result =
                            check_allownextresult_line (line, line_number);
                        mip_destroy (&line);
                        if (check_result != ENOERR) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordComplete:
                        check_result =
                            check_complete_line (line, line_number);
                        mip_destroy (&line);

                        if (check_result != ENOERR) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;

                        }

                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordCancelIfError:
                        mip_destroy (&line);

                        /* No need to check because canceliferror keyword
                         * does not have any parameters. */
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordTimeout:
                        check_result = check_timeout_line (line, line_number);
                        mip_destroy (&line);

                        if (check_result != ENOERR) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordSleep:
                        check_result = check_sleep_line (line, line_number);
                        mip_destroy (&line);

                        if (check_result != ENOERR) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }

                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordVar:

                        mip_get_next_string (line, &varname);
                        if ((varname != NULL) || (varname != INITPTR)) {
                                ass_varname =
                                    NEW2 (char, strlen (varname) + 1);
                                sprintf (ass_varname, "%s", varname);
                                dl_list_add (var_list, (void *)ass_varname);
                                mip_destroy (&line);
                                DELETE (varname);
                        } else {
                                MIN_ERROR ("var keyword syntax error in line %d"
                                            ". Variable name not specified.",
                                            line_number);
                                mip_destroy (&line);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                        
                case EKeywordSendreceive:
                        mip_get_next_string (line, &varname);
                        if ((varname != NULL) && (varname != INITPTR)) {
                                if (strchr (varname, '=') == NULL) {
                                        MIN_ERROR ("sendrecive keyword syntax "
                                                   "error in line %d. "
                                                   "Argument has faulty syntax "
                                                   "(no '=' character).", 
                                                   line_number);
                                        mip_destroy (&line);
                                        tc_title = NULL;
                                        goto EXIT_VALIDATE;
                                }
                        }
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordExpect:

                        check_result =
                            check_expect_line (line, var_list, line_number);
                        mip_destroy (&line);

                        if (check_result != ENOERR) {
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordInterference:
                        check_result =
                            check_interference_line (line, line_number);
                        if (check_result != ENOERR) {
                                MIN_ERROR ("Test Interference Fault");
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordIf:
                        check_result = check_if_line (line, line_number);
                        if (check_result != ENOERR) {
                                MIN_ERROR ("Syntax error in line %d. "
                                           "Invalid if statetment", 
                                           line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        nest_level++;
                        nesting [nest_level] = IF;
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordElse:
                        if (nesting [nest_level] != IF) {
                                MIN_ERROR ("Syntax error in line %d. "
                                           "Unexpected else.", line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordEndif:
                        if (nest_level <= 0 || nesting [nest_level] != IF) {
                                MIN_ERROR ("Syntax error in line %d. "
                                           "Unexpected endif.", line_number);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        nest_level--;
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                case EKeywordUnknown:
                        MIN_ERROR ("Syntax error in line %d"
                                    ". Unknown keyword [%s]", line_number, 
                                   token);
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        break;
                default:
                        MIN_ERROR ("Syntax error in line %d"
                                    ". Unknown keyword [%s]", line_number,
                                   token);
                        mip_destroy (&line);
                        line = mmp_get_next_item_line (testcase);
                        continue;
                        break;
                }
        }
        if (token != INITPTR)
                DELETE (token);

        /* check if script's loop counter is 0, otherwise fail syntax check */
        if (nest_level != 0) {
                DELETE (tc_title);
                tc_title = NULL;
                MIN_ERROR ("Syntax error: missing %s", 
                           nesting [nest_level] == IF ? "endif" : "endloop");
                goto EXIT_VALIDATE;
        }

        /*now check if all interference objects have been stopped (this is
           very important!) */
        if ((interf_objs != INITPTR)&&(dl_list_size (interf_objs) != 0)) {
                MIN_WARN ("Not all test interference instances stopped!");
                var_item = dl_list_head (interf_objs);
                while (var_item != DLListNULLIterator) {
                        varname = (char *)dl_list_data (var_item);
                        DELETE (varname);
                        dl_list_remove_it (var_item);
                        var_item = dl_list_head (interf_objs);
                }
                DELETE (tc_title);
                tc_title = NULL;
                goto EXIT_VALIDATE;
        }
        /*ok, finished fetching symbols from script, now let's validate them */
        library_item = dl_list_head (symblist);

        while (library_item != DLListNULLIterator) {
                library = (ScripterDataItem *) dl_list_data (library_item);

                switch (library->DLL_type_) {

                case EDLLTypeClass:
                        if (strstr (library->DLL_name_, ".so") == NULL) {
                                libpath =
                                    NEW2 (char,
                                          strlen (library->DLL_name_) + 4);
                                sprintf (libpath, "%s.so",
                                         library->DLL_name_);
                        } else {
                                libpath =
                                    NEW2 (char,
                                          strlen (library->DLL_name_) + 1);
                                sprintf (libpath, "%s", library->DLL_name_);
                        }
                        dll_handle = tl_open_tc (libpath);
                        if (dll_handle == INITPTR) {
                                MIN_ERROR ("Unable to load "
                                            "test library %s: %s\n",
                                            library->DLL_name_, dlerror ());
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
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
                                MIN_ERROR ("get cases unresolved "
                                            "in Test Library %s: %s",
                                            test_class->classname_, dlerror ());
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        test_class->runtc_ =
                            (ptr2runtc) dlsym (dll_handle, "ts_run_method");
                        if (test_class->runtc_ == NULL) {
                                MIN_ERROR ("run case unresolved "
                                            "in Test Library %s: %s",
                                            test_class->classname_, dlerror ());
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
                        }
                        dl_list_add (testclasses, (void *)test_class);
                        class_methods = dl_list_create ();
                        /*get test case names */
                        test_class->casetc_ (&class_methods);
                        if ((class_methods == INITPTR)
                            || (dl_list_size (class_methods) == 0)) {
                                MIN_WARN ("Could not get list of "
                                           "functions from %s",
                                           test_class->classname_);
                                DELETE (tc_title);
                                tc_title = NULL;
                                goto EXIT_VALIDATE;
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
                                                MIN_ERROR
                                                    ("Symbol %s not found "
                                                     "in %s",
                                                     callname,
                                                     library->DLL_name_);
                                                dlclose (dll_handle);
                                                DELETE (tc_title);
                                                tc_title = NULL;
                                                goto EXIT_VALIDATE;
                                        }

                                }
                                call_item = dl_list_next (call_item);
                        }

                case EDLLTypeNormal:
                        {
                                break;
                        }

                }
                library_item = dl_list_next (library_item);
        }
        /*free all allocated memory */
      EXIT_VALIDATE:

        call_item = dl_list_head (slaves);
        while (call_item != DLListNULLIterator) {
                callname = (char *)dl_list_data (call_item);
                DELETE (callname);
                dl_list_remove_it (call_item);
                call_item = dl_list_head (slaves);
        }

        call_item = dl_list_head (testclasses);
        while (call_item != DLListNULLIterator) {
                test_class = dl_list_data (call_item);
                DELETE (test_class->classname_);
                DELETE (test_class);
                call_item = dl_list_next (call_item);
        }
        dl_list_free (&testclasses);

        call_item = dl_list_head (class_methods);
        while (call_item != DLListNULLIterator) {
                tc = (TestCaseInfo *) dl_list_data (call_item);
                DELETE (tc);
                call_item = dl_list_next (call_item);
        }
        dl_list_free (&class_methods);


        dl_list_free (&slaves);
        free (libpath);
        call_item = dl_list_head (assoc_cnames);
        while (call_item != DLListNULLIterator) {
                callname = (char *)dl_list_data (call_item);
                if (tc_title != INITPTR)
                        MIN_ERROR ("Validation error: testclass %s not "
                                   "deleted in testcase %s",
                                   callname, tc_title);
                free (callname);
                dl_list_remove_it (call_item);
                call_item = dl_list_head (assoc_cnames);
        }
        dl_list_free (&assoc_cnames);
        if (dl_list_size (assoc_lnames) != 0) {
                /*list not empty, so not all classes
                   were "deleted" */
                if (tc_title != INITPTR) {
                        DELETE (tc_title);
                        tc_title = NULL;
                }
        }

        call_item = dl_list_head (requested_events);
        while (call_item != DLListNULLIterator) {
                ass_evname = (char *)dl_list_data (call_item);
                free (ass_evname);
                dl_list_remove_it (call_item);
                call_item = dl_list_head (requested_events);
        }
        dl_list_free (&requested_events);

        call_item = dl_list_head (assoc_lnames);
        while (call_item != DLListNULLIterator) {
                callname = (char *)dl_list_data (call_item);
                free (callname);
                dl_list_remove_it (call_item);
                call_item = dl_list_head (assoc_lnames);
        }
        dl_list_free (&assoc_lnames);

        library_item = dl_list_head (symblist);
        while (library_item != DLListNULLIterator) {
                scripter_dsa_remove (library_item);
                library_item = dl_list_head (symblist);
        }
        dl_list_free (&symblist);

        var_item = dl_list_head (var_list);
        while (var_item != DLListNULLIterator) {
                varname = (char *)dl_list_data (var_item);
                DELETE (varname);
                dl_list_remove_it (var_item);
                var_item = dl_list_head (var_list);
        }
        dl_list_free (&var_list);


        return tc_title;
}

/* ------------------------------------------------------------------------- */
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

int scripter_init (minScripterIf * scripter_if)
{
        current = INITPTR;
        return 0;
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_scripter_if.tests"
#endif                          /* MIN_UNIT_TEST */
/* End of file */
