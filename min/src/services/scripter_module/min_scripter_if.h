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
 *  @file       min_scripter_if.h
 *  @version    0.1
 *  @brief      This file contains header file for the scripter
 */

#ifndef MIN_SCRIPTER_IF_H
#define MIN_SCRIPTER_IF_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <string.h>
#include <dllist.h>
#include <min_common.h>
#include <min_parser.h>
#include <test_module_api.h>
#include <tllib.h>
#include <scripter_dsa.h>
#include <scripter_common.h>
#include <min_logger.h>
#include <min_logger_common.h>
#include <tmc_common.h>
#include <dlfcn.h>
#include <sys/time.h>

#include "scripted_test_process.h"
#include "combined_test_process.h"
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* ------------------------------------------------------------------------- */
typedef struct _ScriptedTestProcessDetails ScriptedTestProcessDetails;
typedef struct _TestClassDetails TestClassDetails;
typedef struct _TestModDetails TestModDetails;
typedef struct _ExtraParams ExtraParams;
/** Pointer to run function from test class */
typedef int     (*ptr2runtc) (MinItemParser *);
/** Pointer to cases function from test class */
typedef int     (*ptr2casetc) (DLList **);
/* ------------------------------------------------------------------------- */
/* Test category. */
typedef enum {
        ECategoryNormal,        /**< Normal category */
        ECategoryLeave,         /**< Leave category */
        ECategoryPanic,         /**< Panic category */
        ECategoryException,     /**< Exception category */
        ECategoryTimeout,       /**< Timeout category */
        ECategoryUnknown        /**< Unknown category */
} TTestCategory;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* Structures */
/* None */

/* ------------------------------------------------------------------------- */
/** MIN Scripter interface callback structure.
 */
typedef struct _min_scripter_if {
        /**
	 * Called to validate syntax before executing.
	 */
        int             (*validateSyntax) (filename_t scriptfile);
        /**
	 * Returns a list containing all dllNames and symbols
	 */
        int             (*getSymbols) (DLList * symlist,
                                       filename_t scriptfile);
        /**
	 * Returns a list of test case titles
	 */
        int             (*getCaseTitles) (const char *cfg_file,
                                          DLList ** cases);
        /**
	 * Gives the scripter opportunity to execute next command
	 * on the script
	 */
        int             (*scheduleScripter) (const char *scriptfile,
                                             int section_number);
} minScripterIf;
/* ------------------------------------------------------------------------- */
/* Hold optionals parameters. Used with RUN keyword in Combiner */
struct _ExtraParams {
        unsigned int    expect_;        /**< The expected result */
        char            testid_[128];   /**< Test case identyfication */
        char            ini_[512];      /**< Initialization file for module */
        TTestCategory   category_;      /**< The result category */
        unsigned int    timeout_;       /**< Test case timeout */
        char            title_[128];    /**< Test case title */
};
/* ------------------------------------------------------------------------- */
/** Detailed information about test process 
 *  options, status and tcr are used only with Combiner csaes (RUN keyword)*/
struct _ScriptedTestProcessDetails {
        char            testclass_[128]; /**< name of the loaded test class  */
        pid_t           pid_;            /**< pid of the created process     */
        filename_t      dllname_;        /**< dllname in which test class is */
        filename_t      cfgfile_;        /**< config file used with Normal   */
        TDLLType        mod_type_;       /**< type of the module             */
        ExtraParams     options_;        /**< optional parameters            */
        MINTPStatus    status_;         /**< overall status of the TP       */
        DLList         *tcr_list_;       /**< result of the test case        */
        DLList         *allowed_results_;/**< allowed result of the test     */
        unsigned int    tc_id_;          /**< test case id (combiner)        */
};
/* ------------------------------------------------------------------------- */
/** Information we need to call test function */
struct _TestClassDetails {
        void           *dllhandle_;     /**< handler to the loaded dll */
        char           *classname_;     /**< class name */
        ptr2casetc      casetc_;        /**< handle to case function from TC*/
        ptr2runtc       runtc_;         /**< handle to run function from TC */
};

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** substracts two timeval structures.
 *  @param result [out] the result of substraction
 *  @param x [in] left operand
 *  @param y [in] right operand
 *  @return 1 if result negative, 0 otherwise */
int             substract_timeval (struct timeval *result, struct timeval *x,
                                   struct timeval *y);
/* ------------------------------------------------------------------------- */
/** add two timeval structures.
 *  @param result [out] the result of addition
 *  @param x [in] left operand
 *  @param y [in] right operand
 *  @return 1 if result negative, 0 otherwise */
int             add_timeval (struct timeval *result, struct timeval *x,
                             struct timeval *y);
/* ------------------------------------------------------------------------- */
/** Initiates the scripter interface
 *  @param sif [in/out] the scripter interface to be filled by scripter plugin.
 *  @param tmc [in] pointer to the tmc saved for later use
 */
int             scripter_if_init (minScripterIf * sif, TMC_t * tmc);
/* ------------------------------------------------------------------------- */
/** Creates testclass
 *  @param dllName [in] the name of the dll library.
 *  @param className [in] name of the class that will be created.
 * 
 *  NOTE: syntax which does the call: "create <dllname> <classname>"
 */
int             testclass_create (char *dllName, char *className);
/* ------------------------------------------------------------------------- */
/** Calls function from a given test class.
 *  @param className [in] the name of the class.
 *  NOTE: syntax which does the call: "<dllname> <funcname>"
 */
int             testclass_call_function (char *className,
                                         MinItemParser * mip);
/* ------------------------------------------------------------------------- */
/** Destroys testclass instance.
 *  @param className [in] name of the class to de destroyed.
 * 
 *  NOTE: syntax which does the call: "delete <classname>"
 */
int             testclass_destroy (char *className);

/* ------------------------------------------------------------------------- */
/** Registers scripter for an Event
 *  @param eventname [in] name of the event
 *  @param is_state  [in] set if the event is state envent
 *  NOTE: syntax which does the call: "request <eventname> [state]"
 */
int             event_request (char *eventname, int is_state);

/* ------------------------------------------------------------------------- */
/** Wait for Event
 *  @param className [in] name of the event
 *  NOTE: syntax which does the call: "wait <eventname>"
 */
int             event_wait (char *eventname);

/* ------------------------------------------------------------------------- */
/** Release the event
 *  @param eventname [in] name of the event
 *  NOTE: syntax which does the call: "release <eventname>"
 */
int             event_release (char *eventname);

/* ------------------------------------------------------------------------- */
/** Set the event
 *  @param eventname [in] name of the event
 *  NOTE: syntax which does the call: "set <eventname> [state]"
 */
int             event_set (char *eventname, int is_state);

/* ------------------------------------------------------------------------- */
/** Set the event
 *  @param eventname [in] name of the event
 *  NOTE: syntax which does the call: "unset <eventname>"
 */
int             event_unset (char *eventname);

/* ------------------------------------------------------------------------- */
/** Load test libraries used by script and checks if used symbols are available
 *  from those libraries
 *  @param symlist [in] list of ScripterDataItems
 *  @return 0 if all symbols are available, -1 otherwise.
 */
char           *validate_test_case (MinSectionParser * testcase);
/* ------------------------------------------------------------------------- */
/** Validate the define section of script file
 *  @param define [in] the define section of script file
 *  @return ESTrue if all symbols are available, ESFalse otherwise.
 */
TSBool          validate_define (MinSectionParser * define);
/* ------------------------------------------------------------------------- */
/** Called when the end of test case is reached in the script
 * 
 *  NOTE: syntax which does the call: "[Endtest]"
 */
int             script_finish (void);
/* ------------------------------------------------------------------------- */
/** Prints text from a testclass.
 *  @param mip [in] pointer to the min item parser.
 *
 *  NOTE: syntax which does the call: "print <text>"
 */
int             testclass_print (MinItemParser * mip);
/* ------------------------------------------------------------------------- */
/** Sets next allowed result for a testclass execution.
 *  @param nextresult [in] allowed result
 *  @param mip [in] posible next results, we support multiple next results. 
 *
 *  NOTE: syntax which does the call: 
 *  "allownextresult <result> [result2] .. [resultN]"
 */
int             testclass_allownextresult (int nextresult,
                                           MinItemParser * mip);
/* ------------------------------------------------------------------------- */
/** Start a new testcase 
 *  @param modulename [in] name of the test module.
 *  @param configfile [in] module configuration file name.
 *  @param id         [in] test case id number
 *  @param mip        [in] the optional parameters
 *
 *  NOTE: syntax which does the call: 
 * "run <modulename> <configfile> <id> ..."
 */
int             test_run (const char *modulename, const char *configfile,
                          unsigned int id, MinItemParser * mip);
/* ------------------------------------------------------------------------- */
/** Resumes paused test process
 *  @param testid [in] testid of the test process to be resumed.
 *
 *  NOTE: syntax which does the call: "resume <testid>"
 */
int             test_resume (char *testid);
/* ------------------------------------------------------------------------- */
/** Aborts test process
 *  @param testid [in] testid of the test process to be aborted.
 *
 *  NOTE: syntax which does the call: "cancel <testid>"
 */
int             test_cancel (char *testid);
/* ------------------------------------------------------------------------- */
/** Pauses test process 
 *  @param testid [in] id of the process to be paused
 *  @param mip [in] optional parameters
 *  NOTE: syntax which does the call: "pause <testid> [time=<time>]"
 */
int             test_pause (char *testid, MinItemParser * mip);
/* ------------------------------------------------------------------------- */
/** Stops execution till specified test finishes. 
 *  @param testid [in] id of the test that we are waiting to complete.
 *
 *  NOTE: syntax which does the call: "complete <testid>"
 */
int             test_complete (const char *testid);
/* ------------------------------------------------------------------------- */
/** Allocates a slave
 *  @param slave_type [in] is the type of slave, currently "phone" type is
 *                         the one that is supported. 
 *  @param slave_name [in] name of the slave which will be further used.
 *
 *  NOTE: syntax which does the call: "allocate <slavetype> <slaveid>"
 */
int             test_allocate_slave (const char *slave_type,
                                     const char *slave_name);
/* ------------------------------------------------------------------------- */
/** Frees allocated slave
 *  @param slave_name [in] name of the slave which will be freed.
 *
 *  NOTE: syntax which does the call: "free <slaveid>"
 */
int             test_free_slave (const char *slave_name);
/* ------------------------------------------------------------------------- */
/** Executes a command on a remote slave device.
 *  @param slave_name [in[ the name of the slave that is going to be used.
 *  @param mip [in] contains the test we are going to execute on slave.
 *
 *  NOTE: syntax which does the call: "remote <slaveid> <command>"
 */
int             test_remote_exe (const char *slave_name,
                                 MinItemParser * mip);
/* ------------------------------------------------------------------------- */
/** Cancels script execution when test fails.
 *
 *  NOTE: syntax which does the call: "canceliferror"
 */
int             test_canceliferror ();
/* ------------------------------------------------------------------------- */
/** Sets timeout for the scripter/combiner 
 *  @param interval [in] the timeout value in miliseconds
 *
 *  NOTE: syntax which does the call: "timeout <interval>"
 * 
 */
int             testclass_test_timeout (unsigned long interval);
/* ------------------------------------------------------------------------- */
/** Takes script execution into sleep for specified period of time. 
 *  @param interval [in] the sleep time in miliseconds
 *
 *  NOTE: syntax which does the call: "sleep <interval>" 
*/
int             testclass_test_sleep (unsigned long interval);
/* ------------------------------------------------------------------------- */
/** Declares a variable for the scripter
 *  @param name [in] name of the variable
 *  @param initialize [in] flag to tell whether third argument is to considered
 *         as initilial value for the variable
 *  @param val [in] initial value for the variable
 *
 *  NOTE: syntax which does the call: "var <value>"  
 */
int             declare_var (char *name, TSBool initialize, char *val);
/* ------------------------------------------------------------------------- */
/** Sends a variable value from slave to master
 *  @param variable [in] name of the variable
 *  @param value [in] variable value
 *
 *  NOTE: syntax which does the call: "sendreceive <variable>=<value>"  
 */
int             sendreceive_slave_send (char *variable, char *value);
/* ------------------------------------------------------------------------- */
/** Expects a variable value from master
 *  @param variable [in] name of the variable
 *
 *  NOTE: syntax which does the call: "expect <variable>"  
 */
int             sendreceive_slave_expect (char *variable);
/* ------------------------------------------------------------------------- */
/** Operates test interference in script
 * @param mip [in] pointer to min item parser containing script 
 * line with parameters
 * NOTE: syntax which does the call: "testinterference <name>.<command> [type] 
 * [value] [idle time] [active time]"
 */
int             test_interference(MinItemParser* args);
/* ------------------------------------------------------------------------- */
#endif                          /* MIN_SCRIPTER_IF_H */

/* End of file */
