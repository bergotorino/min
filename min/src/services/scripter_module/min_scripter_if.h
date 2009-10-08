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
#define DEFAULT_BLOCKING_TIMEOUT 600 /** Default value for blocking timeout */

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
	char            has_crashed_;    /**< set if test class has crashed  */
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
int             substract_timeval (struct timeval *result, struct timeval *x,
                                   struct timeval *y);
/* ------------------------------------------------------------------------- */
int             add_timeval (struct timeval *result, struct timeval *x,
                             struct timeval *y);
/* ------------------------------------------------------------------------- */
int             testclass_create (char *dllName, char *className);
/* ------------------------------------------------------------------------- */
int             testclass_call_function (char *className,
                                         MinItemParser * mip);
/* ------------------------------------------------------------------------- */
int             testclass_destroy (char *className);
/* ------------------------------------------------------------------------- */
int             event_request (char *eventname, int is_state);
/* ------------------------------------------------------------------------- */
int             event_wait (char *eventname);
/* ------------------------------------------------------------------------- */
int             event_release (char *eventname);
/* ------------------------------------------------------------------------- */
int             event_set (char *eventname, int is_state);
/* ------------------------------------------------------------------------- */
int             event_unset (char *eventname);
/* ------------------------------------------------------------------------- */
char           *validate_test_case (MinSectionParser * testcase);
/* ------------------------------------------------------------------------- */
TSBool          validate_define (MinSectionParser * define);
/* ------------------------------------------------------------------------- */
int             script_finish (void);
/* ------------------------------------------------------------------------- */
int             testclass_print (MinItemParser * mip);
/* ------------------------------------------------------------------------- */
int             testclass_allownextresult (int nextresult,
                                           MinItemParser * mip);
/* ------------------------------------------------------------------------- */
int             test_run (const char *modulename, const char *configfile,
                          unsigned int id, MinItemParser * mip);
/* ------------------------------------------------------------------------- */
int             test_resume (char *testid);
/* ------------------------------------------------------------------------- */
int             test_cancel (char *testid);
/* ------------------------------------------------------------------------- */
int             test_pause (char *testid, MinItemParser * mip);
/* ------------------------------------------------------------------------- */
int             test_complete (const char *testid);
/* ------------------------------------------------------------------------- */
int             test_allocate_slave (const char *slave_type,
                                     const char *slave_name);
/* ------------------------------------------------------------------------- */
int             test_free_slave (const char *slave_name);
/* ------------------------------------------------------------------------- */
int             test_remote_exe (const char *slave_name,
                                 MinItemParser * mip);
/* ------------------------------------------------------------------------- */
int             test_canceliferror ();
/* ------------------------------------------------------------------------- */
int             testclass_test_timeout (unsigned long interval);
/* ------------------------------------------------------------------------- */
int             testclass_test_sleep (unsigned long interval);
/* ------------------------------------------------------------------------- */
int             declare_var (char *name, TSBool initialize, char *val);
/* ------------------------------------------------------------------------- */
int             sendreceive_slave_send (char *variable, char *value);
/* ------------------------------------------------------------------------- */
int             sendreceive_slave_expect (char *variable);
/* ------------------------------------------------------------------------- */
int             test_interference (MinItemParser* args);
/* ------------------------------------------------------------------------- */
TSBool          eval_if (char *condition);
/* ------------------------------------------------------------------------- */
int             set_block_timeout (unsigned long timeout);
/* ------------------------------------------------------------------------- */

#endif                          /* MIN_SCRIPTER_IF_H */

/* End of file */
