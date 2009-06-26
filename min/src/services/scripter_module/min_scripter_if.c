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
 *  @file       min_scripter_if.c
 *  @version    0.1
 *  @brief      This file contains implementation of MIN Scripter interface.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <sched.h>
#include <ctype.h>

#include <dllist.h>
#include <min_lego_interface.h>
#include <min_scripter_if.h>
#include <scripter_plugin.h>
#include <scripter_common.h>
#include <test_module_api.h>
#include <min_test_interference.h>
/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
extern int      scripter_init (minScripterIf * scripter_if);
/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* ------------------------------------------------------------------------- */
TSBool          stprun = ESFalse; /**< exit flag for scripted test process */
TSBool          ctprun = ESFalse; /**< exit flag for combined test process */
struct scripter_mod_ scripter_mod;
DLList         *tp_handlers = INITPTR;  /**< handles to tmapi fctions from TC*/
DLList         *allowedresults = INITPTR; /**< Allowed  result codes. */
DLList         *allowederrors = INITPTR;  /**< Allowed error codes. */
DLList         *defines = INITPTR;  /**< Contains defintions            */
DLList         *variables = INITPTR;/**< Contains scripter variables */
DLList         *interference_handles = INITPTR;/**<test interference "instances"*/
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/** Reports scripter run time error with one argumen 
 * @param __errstr__ the error string
 * @param __errarg__ error argument string
 */

#define SCRIPTER_RTERR_ARG(__errstr__,__errarg__) \
do {									   \
        MIN_ERROR (__errstr__,__errarg__);			           \
        snprintf (scripter_mod.script_tcr.desc_, MaxTestResultDescription, \
                  "%s:%s:%d - %s [%s]",	 __FILE__,__FUNCTION__, __LINE__,  \
                  __errstr__, __errarg__);		                   \
        scripter_mod.error_occured = ESTrue;				   \
        scripter_mod.script_tcr.result_ = TP_CRASHED;                      \
} while (0)

/** Reports scripter run time error
 * @param __errstr__ the error string
 */
#define SCRIPTER_RTERR(__errstr__) \
do {			                                                   \
        MIN_ERROR (__errstr__);			           \
        snprintf (scripter_mod.script_tcr.desc_, MaxTestResultDescription, \
                  "%s:%s:%d - %s",__FILE__, __FUNCTION__, __LINE__,        \
                  __errstr__);				                   \
        scripter_mod.error_occured = ESTrue;				   \
        scripter_mod.script_tcr.result_ = TP_CRASHED;                      \
} while (0)

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* ------------------------------------------------------------------------- */
struct scripter_mod_ {
        minScripterIf  min_scripter_if;/**< Interface to parser plugin     */
        DLList         *req_events;      /**< Local requested events.        */
        DLList         *tp_details;      /**< Global list of loaded modules. */
        int             mqid;            /**< Message Queue Id               */
        TSBool          script_finished; /**< Set when script end  noticed   */
        TSBool          tclass_exec;     /**< Set while class func running   */
        TSBool          paused;          /**< Set when paused from the cui   */
        unsigned long   blocking_timeout;/**< Timeout for blocking commands  */ 
        struct timeval  blocking_since;  /**< Used to handle block timeouts  */
        TSBool          extif_pending;   /**< Waiting resp to ext cont. msg  */
        TSBool          event_pending;   /**< Waiting for event ind          */
	TSBool          error_occured;   /**< Run time error flag            */
        unsigned int    extif_remote;    /**< number of remote test cases    */
        char           *testcomplete;    /**< Complete test flag             */
        TSBool          canceliferror;   /**< Cancels script if error in test*/
        unsigned long   sleep;           /**< Sleep enabled flag if this > 0 */
        struct timeval  time;            /**< Used for sleep realization     */
        TestCaseResult  script_tcr;      /**< Scripted test case result      */
        char           *expected_var;    /**< Variable we are expecting      */
        int             shm_id;          /**< Shared segment ID              */
};

typedef struct {
        char           *name_;
        TSBool          initialized_;
        char           *value_;
} ScriptVar;

typedef struct {
        char                    name_[32];
        testInterference        *instance_;
} InterfHandle;

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Used for finding class name on list. */
LOCAL int       _findclass (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
/** Used for finding test process of specified pid. */
LOCAL int       _findpid (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
/** Used for finding test process of specified status. */
LOCAL int       _findstatus (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
/** Used for finding test process of specified testid. */
LOCAL int       _findid (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
/** Used for finding specified allowed result and also allowed error code */
LOCAL int       _findallowedresult (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
/** Fetches pointer to run_method from library and stores it in safe place.
 *  @param dllname [in] name of the library from which pointer is fetched.
 *  @param classname [in] mnemonic for that library.
 */
LOCAL void      fetch_ptr2run (const char *dllname, const char *classname);
/* ------------------------------------------------------------------------- */
/** Reads possible optional parameter for the RUN keyword. 
 *  @param mip [in] line with possible parameters. 
 *  @param ep [out] structure that contains extra parameters */
LOCAL void      read_optional_run_params (MinItemParser * mip,
                                          ExtraParams * ep);
/* ------------------------------------------------------------------------- */
/** Message handling function. Calls appropiate message handler.
 *  @param mqid [in] message queue id.
 *  @param msg [in] buffer that contains message to be handled.
 */
LOCAL void      uengine_handle_message (int mqid, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** Handles MSG_RET
 *  @param mqid [in] message queue id.
 *  @param param [in] test result code.
 *  @param message [in] description of the test result.
 */
LOCAL void      uengine_handle_ret (int mqid, int param, const char *message,
                                    long sender);
/* ------------------------------------------------------------------------- */
/** Handles indication event.
 *  @param mqid [in] message queue id.
 *  @param msg [in] buffer that contains message to be handled.
 */
LOCAL void      uengine_handle_event_ind (int mqid, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** Handles MSG_SNDRCV
 *  @param mqid [in] message queue id.
 *  @param msg [in] buffer that contains message to be handled.
 */
LOCAL void      uengine_handle_sndrcv (int mqid, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** Handles MSG_END message.
 *  @param mqid [in] message queue id.
 *  @param msg [in] buffer that contains message to be handled.
 */
LOCAL void      uengine_handle_end (int mqid, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** Handles MSG_PAUSE message.
 *  @param mqid [in] message queue id.
 *  @param msg [in] buffer that contains message to be handled.
 */
LOCAL void      uengine_handle_pause (int mqid, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** Handles MSG_RESUME message.
 *  @param mqid [in] message queue id.
 *  @param msg [in] buffer that contains message to be handled.
 */
LOCAL void      uengine_handle_resume (int mqid, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** Handles abort message.
 *  @param mqid [in] message queue id.
 *  @param msg [in] buffer that contains message to be handled.
 */
LOCAL void      uengine_handle_stop (int mqid, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** Handles external controller message.
 *  @param mqid [in] message queue id.
 *  @param msg [in] buffer that contains message to be handled.
 */
LOCAL void      uengine_handle_extif (int mqid, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** handles external controller message type EResponseSlave
 *  @param result [in] the result of the operation: 1 = Ok, 0 = Error 
 *  @param caseid [in] the test case id, if caseid > 0
 */
LOCAL void      uengine_handle_extif_response (int result, int caseid);
/* ------------------------------------------------------------------------- */
/** handles external controller message type ERemoteSlaveResponse
 *  @param testresult [in] test case result. 
 *  @param caseid [in] the test case id, if caseid > 0
 */
LOCAL void      uengine_handle_extif_remote_response (int testresult,
                                                      int caseid);
/* ------------------------------------------------------------------------- */
/** Handler for SIGCHLD
 *  @param sig signal numnber
 */
/*LOCAL void scripter_sigchld_handler( int sig );*/
/* ------------------------------------------------------------------------- */
/** Handler for SIGALRM
 *  @param sig signal numnber
 */
LOCAL void      scripter_sigalrm_handler (int sig);
/* ------------------------------------------------------------------------- */
/** Checks a few global flags to see if next line in script should be executed
 */
LOCAL TSBool    _execute_script (void);
/* ------------------------------------------------------------------------- */
/** Checks if there are test cases running 
 */
LOCAL TSBool    _pending_tests (void);
/* ------------------------------------------------------------------------- */
/** List find compare function
 *  @param a comparison lvalue
 *  @param b comparison rvalue
 */
LOCAL int       _look4event (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
/** Searches for event registration from a local list
 *  @param eventname name of the envent
 */
LOCAL minEventIf *find_event_reqistration (char *eventname);
/* ------------------------------------------------------------------------- */
/** Translates literal category to the enum representation.
 *  @param str [in] literal category to be translated
 *  @return enumerator value that correstonds to str @see TTestCategory
 */
LOCAL TTestCategory get_result_category (const char *str);
/* ------------------------------------------------------------------------- */
/** Assigns a value to script variable
 *  @param varname [in] variable name
 *  @param varval [in] variable value
 */
LOCAL void var_assign (const char *varname, const char *varval);
/* ------------------------------------------------------------------------- */
/** Updates the scriter internal variables 
 *  @param res [in] Test Process result
 */
LOCAL void update_variables (TPResult res);
/* ------------------------------------------------------------------------- */
/** Used for declaring scripter interal variables
 *  @param varname [in] variable name
 */
LOCAL int declare_internal_var (const char *varname);
/* ------------------------------------------------------------------------- */
/** Interpretes the variable value as integer and increments by one
 *  @param varname [in] variable name
 */
LOCAL void var_increment (const char *varname);
/* ------------------------------------------------------------------------- */
/** Sends scripter variables to test class
 */
LOCAL void send_variables ();
/* ------------------------------------------------------------------------- */
/** Receives scripter variables from test class
 */
LOCAL void receive_variables ();
/* ------------------------------------------------------------------------- */
/** Inspects all the results of the method, combiner calls made from the script
 *  and decides if the scripted test case is PASSED or FAILED
 *  @param tp_details [in] list of method / combiner calls
 *  @param tcr [in/out] the final result
 */
LOCAL void      scripter_final_verdict (DLList * tp_details,
                                        TestCaseResult * tcr);

/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
LOCAL int _findclass (const void *a, const void *b)
{
        ScriptedTestProcessDetails *stpd = (ScriptedTestProcessDetails *) a;
        return strcmp (stpd->testclass_, (char *)b);
}

/* ------------------------------------------------------------------------- */
LOCAL int _findpid (const void *a, const void *b)
{
        ScriptedTestProcessDetails *stpd = (ScriptedTestProcessDetails *) a;
        if (stpd->pid_ == *(long *)b)
                return 0;
        else
                return 1;
}

/* ------------------------------------------------------------------------- */
LOCAL int _findstatus (const void *a, const void *b)
{
        ScriptedTestProcessDetails *stpd = (ScriptedTestProcessDetails *) a;
        if (stpd->status_ == *(int *)b)
                return 0;
        else
                return 1;
}

/* ------------------------------------------------------------------------- */
LOCAL int _findid (const void *a, const void *b)
{
        ScriptedTestProcessDetails *stpd = (ScriptedTestProcessDetails *) a;
        return strcmp (stpd->options_.testid_, (char *)b);
}

/* ------------------------------------------------------------------------- */
LOCAL int _findallowedresult (const void *a, const void *b)
{
        if (*(int *)a == *(int *)b)
                return 0;
        else
                return 1;
}
/* ------------------------------------------------------------------------- */
LOCAL TSBool _pending_tests ()
{
        DLListIterator  it = DLListNULLIterator;
        int             desiredstatus = TP_RUNNING;

        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findstatus, &desiredstatus);

        if (it != DLListNULLIterator)
                return ESTrue;

        desiredstatus = TP_PAUSED;
        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findstatus, &desiredstatus);

        if (it != DLListNULLIterator)
                return ESTrue;

        if (scripter_mod.tclass_exec)
                return ESTrue;

        if (scripter_mod.extif_remote > 0)
                return ESTrue;

        return ESFalse;
}

/* ------------------------------------------------------------------------- */
LOCAL TSBool _execute_script ()
{
        struct timeval  res;
        struct timeval  now;
        unsigned long   elapsed = 0;    /* in [ms] */

	/* If an error has occured do not investigate further,
	 * otherwise we might get stuck (e.g. remote run + event)
	 */
	if (scripter_mod.error_occured)
                return ESTrue;


        /* Check if we are waiting for the 
         * response to external controller message 
	 * or en event indication 
	 */
        if (scripter_mod.extif_pending || scripter_mod.event_pending) {
		/* Check for timeout of blocking commands */
		/* .. if we have not set the timeout to none */
		if (scripter_mod.blocking_timeout == 0)
			return ESFalse;
                gettimeofday (&now, NULL);
                substract_timeval (&res, &now, &scripter_mod.blocking_since);

                elapsed = (unsigned long)res.tv_sec;
		MIN_INFO ("Elapsed = %u", elapsed);
		if (elapsed >= scripter_mod.blocking_timeout) {
			SCRIPTER_RTERR ("Timeout for blocking command");
		}
                return ESFalse;
	}

        /* Execute script only if its not finished */
        if (scripter_mod.script_finished)
                return ESFalse;

        /* ...and if a blocking method is not running */
        if (scripter_mod.tclass_exec)
                return ESFalse;

        /* ...and if we are not paused from the cui */
        if (scripter_mod.paused)
                return ESFalse;

        /* ...and if we are not waiting for the test to be completed */
        if (scripter_mod.testcomplete != INITPTR)
                return ESFalse;

        /* ...and we are not sleeping */
        if (scripter_mod.sleep > 0) {

                gettimeofday (&now, NULL);
                substract_timeval (&res, &now, &scripter_mod.time);

                elapsed = (unsigned long)res.tv_sec * (unsigned long)1000;
                elapsed += (unsigned long)res.tv_usec / (unsigned long)1000;

                if (elapsed >= scripter_mod.sleep) {
                        scripter_mod.sleep = 0;
                } else {
                        return ESFalse;
                }
        }

        return ESTrue;
}

/* ------------------------------------------------------------------------- */
LOCAL void fetch_ptr2run (const char *dllname, const char *classname)
{
        TestClassDetails *tcd = INITPTR;
        int               clen = 0;
        char             *tmp;

        if (dllname == INITPTR) {
                errno = EINVAL;
                SCRIPTER_RTERR ("library name missing");
                goto EXIT;
        }
        if (strlen (dllname) == 0) {
                errno = EINVAL;
                SCRIPTER_RTERR ("library name has 0 length");
                goto EXIT;
        }

        /* 0) Initialize some data */
        tcd = NEW (TestClassDetails);

        /* 1) Load dll library */
        tcd->dllhandle_ = tl_open_tc (dllname);
        if (tcd->dllhandle_ == INITPTR) {
                tmp = NEW2 (char, 
                            strlen ("Unable to load library ") + 
                            strlen (dllname) + 1);
                sprintf (tmp, "Unable to load library %s", dllname);
                SCRIPTER_RTERR_ARG (tmp, dlerror());
                DELETE (tmp);
                MIN_WARN ("Unable to load library [%s] because: %s",
                             dllname, dlerror ());
                DELETE (tcd);
                goto EXIT;
        }

        /* 2) Fetch pointer to the run function */
        tcd->runtc_ = dlsym (tcd->dllhandle_, "ts_run_method");
        if (tcd->runtc_ == NULL) {
                SCRIPTER_RTERR_ARG ("Unable to resolve ts_run_method",
                                    dlerror ());
                MIN_ERROR ("Unable to resolve ts_run_method: %s",
                             dlerror ());
                DELETE (tcd);
                goto EXIT;
        }

        /* 3) Store mnemonic for that library */
        clen = strlen (classname);
        tcd->classname_ = NEW2 (char, clen + 1);
        memset (tcd->classname_, 0x0, clen + 1);
        STRCPY (tcd->classname_, classname, clen + 1);

        /* 4) Save data on the list */
        dl_list_add (tp_handlers, tcd);
EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void read_optional_run_params (MinItemParser * mip, ExtraParams * ep)
{
        char           *token = INITPTR;
        int             token2 = 0;
        char           *parsing_position = INITPTR;
        int             tmp = 0;
        TParsingType    parsing = ENormalParsing;

        if (mip == INITPTR) {
                SCRIPTER_RTERR ("Item parser argument is missing");
                goto EXIT;
        }
        if (ep == INITPTR) {
                SCRIPTER_RTERR ("Extra params argument is missing");
                goto EXIT;
        }

        /* expect parameter */
        parsing_position = mip->item_skip_and_mark_pos_;
        tmp = mip_get_next_tagged_int (mip, "expect", &token2);
        if (tmp == ENOERR) {
                ep->expect_ = token2;
                token2 = 0;
        } else {
                ep->expect_ = ENOERR;
        }

        /* testid parameter */
        mip->item_skip_and_mark_pos_ = parsing_position;
        tmp = mip_get_next_tagged_string (mip, "testid", &token);
        if (tmp == ENOERR) {
                if (token != INITPTR) {
                        STRCPY (ep->testid_, token, 128);
                        DELETE (token);
                        token = INITPTR;
                }
        } else {
                STRCPY (ep->testid_, "\0", 128);
        }

        /* ini parameter */
        mip->item_skip_and_mark_pos_ = parsing_position;
        tmp = mip_get_next_tagged_string (mip, "ini", &token);
        if (tmp == ENOERR) {
                if (token != INITPTR) {
                        STRCPY (ep->ini_, token, 512);
                        DELETE (token);
                        token = INITPTR;
                }
        } else {
                STRCPY (ep->ini_, "\0", 512);
        }

        /* category parameter */
        mip->item_skip_and_mark_pos_ = parsing_position;
        tmp = mip_get_next_tagged_string (mip, "category", &token);
        if (tmp == ENOERR) {
                ep->category_ = get_result_category (token);
                DELETE (token);
                token = INITPTR;
        }

        /* timeout parameter */
        mip->item_skip_and_mark_pos_ = parsing_position;
        tmp = mip_get_next_tagged_string (mip, "timeout", &token);
        if (tmp == ENOERR) {
                ep->timeout_ = 0;
                DELETE (token);
                token = INITPTR;
        }

        /* title parameter */
        parsing = mip_get_parsing_type (mip);
        mip_set_parsing_type (mip, EQuoteStyleParsing);
        mip->item_skip_and_mark_pos_ = parsing_position;
        tmp = mip_get_next_tagged_string (mip, "title", &token);
        if (tmp == ENOERR) {
                if (token != INITPTR) {
                        STRCPY (ep->title_, token, 128);
                        DELETE (token);
                        token = INITPTR;
                }
        } else {
                STRCPY (ep->title_, "\0", 128);
        }
        mip_set_parsing_type (mip, parsing);
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_read_message (int mqid, MsgBuffer * input_buffer)
{
        int             retval = 0;

        retval = mq_read_message (mqid, getpid ()
                                  , input_buffer);
        if (retval == -1) {
                switch (errno) {
                case EINTR:
                        MIN_DEBUG ("Reading MQ interrupted by signal");
                        break;

                case EIDRM:
			SCRIPTER_RTERR("MQ id removed from the system");
                        MIN_FATAL ("MQ id removed from the system");
                        break;

                case E2BIG:
                        MIN_WARN ("Recieved message too big");
                        break;

                case EINVAL:
                        SCRIPTER_RTERR("Invalid value: mq_read_message");
                        MIN_ERROR ("Invalid value: mq_read_message");
                        break;
                }
        } else if (retval < 8) {
                /*ignore too short messages */
                MIN_NOTICE ("Recieved message is too small");
        }

}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_message (int mqid, const MsgBuffer * msg)
{
        MsgBuffer       out;

        switch (msg->type_) {
        case MSG_RET:
                MIN_DEBUG ("Recieved MSG_RET");
                uengine_handle_ret (mqid, msg->param_, msg->message_,
                                    msg->sender_);
                break;
        case MSG_USR:
                MIN_DEBUG ("Recieved MSG_USR");
                /* Forward message to upper level */
                memcpy (&out, msg, sizeof (MsgBuffer));
                out.sender_ = getpid ();
                out.receiver_ = getppid ();
                mq_send_message (mqid, &out);

                /*  */
                sched_yield ();

                break;
        case MSG_EVENT:
                MIN_DEBUG ("Recieved MSG_EVENT");
                memcpy (&out, msg, sizeof (MsgBuffer));
                out.sender_ = getpid ();
                out.receiver_ = getppid ();
                mq_send_message (mqid, &out);
                break;
        case MSG_EVENT_IND:
                MIN_DEBUG ("Recieved MSG_EVENT_IND");
                uengine_handle_event_ind (mqid, msg);
                break;
        case MSG_END:
                MIN_DEBUG ("Recieved MSG_END");
                uengine_handle_end (mqid, msg);
                break;
        case MSG_PAUSE:
                MIN_DEBUG ("Recieved MSG_PAUSE");
                uengine_handle_pause (mqid, msg);
                break;
        case MSG_RESUME:
                MIN_DEBUG ("Recieved MSG_RESUME");
                uengine_handle_resume (mqid, msg);
                break;
        case MSG_STOP:
                MIN_DEBUG ("Recieved MSG_STOP");
                uengine_handle_stop (mqid, msg);
                break;
        case MSG_EXTIF:
                MIN_DEBUG ("Recieved MSG_EXTIF");
                uengine_handle_extif (mqid, msg);
                break;
        case MSG_SNDRCV:
                MIN_DEBUG ("Recieved MSG_SNDRCV");
                uengine_handle_sndrcv (mqid, msg);
                break;
        default:
                MIN_WARN ("Unhandled message from %d: to %d, type: %d",
                             msg->sender_, msg->receiver_, msg->type_);
        }


}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_ret (int mqid, int param, const char *message,
                               long sender)
{
        DLListIterator  it = DLListNULLIterator;
        DLListIterator  it2 = DLListNULLIterator;
        int             desiredstatus = TP_RUNNING;
        ScriptedTestProcessDetails *stpd = INITPTR;
        int            *tmp = INITPTR;
        int             status;
        TestCaseResult *tcr;
        char            tmp2[12];
	TSBool		complete_used=ESFalse;

        if (message == INITPTR) {
                errno = EINVAL;
                SCRIPTER_RTERR ("empty message");
                goto EXIT;
        }

        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findpid, &sender);
        if (it == DLListNULLIterator) {
                sprintf (tmp2, "%lu", sender);
                SCRIPTER_RTERR_ARG  ("No test of given pid", tmp2);
                MIN_WARN ("There is no test of given pid [%d]", sender);
                goto EXIT;
        }
        stpd = (ScriptedTestProcessDetails *) dl_list_data (it);

        if (stpd->mod_type_ == EDLLTypeNormal) {
                /* We have TP created by RUN keyword (Combiner) 
                 * let go back to previous idea and send
                 * test result to the upper level 
                 */

                /* Check if we are waiting for this test to complete 
                 * if so then restart script execution. */
                if (scripter_mod.testcomplete != INITPTR &&
                    !strcmp (scripter_mod.testcomplete,
                             stpd->options_.testid_)) {
			complete_used=ESTrue;
                        DELETE (scripter_mod.testcomplete);
                        scripter_mod.testcomplete = INITPTR;
                } 
                
                /* set status */
                stpd->status_ = TP_ENDED;
		
		if(complete_used==ESTrue){
			test_complete(stpd->options_.testid_);
		}
		
                /* kill TP :) */
                kill (sender, SIGUSR2);
                waitpid (sender, &status, 0);

                /* set test result test result is checked 
                 * against expected optional parameter */
                tcr = NEW (TestCaseResult);
                if (param == stpd->options_.expect_) {
                        tcr->result_ = TP_PASSED;
                } else {
                        tcr->result_ = TP_FAILED;
                }
                update_variables (tcr->result_);

                STRCPY (tcr->desc_, message, MaxTestResultDescription);
                dl_list_add (stpd->tcr_list_, tcr);

                /* Check test result and canceliferror flag */
                if (param == TP_FAILED
                    && scripter_mod.canceliferror == ESTrue) {
                        MIN_DEBUG ("Test Failed, canceliferror ON");
                        stpd->status_ = TP_CANCELED;
                        update_variables (TP_NC);
                        scripter_mod.script_finished = ESTrue;
                        /* Now we have to 'end' rest of the tests */
                        it2 = dl_list_head (scripter_mod.tp_details);
                        do {
                                it = dl_list_find (it2,
                                                   dl_list_tail (scripter_mod.
                                                                 tp_details)
                                                   , _findstatus,
                                                   &desiredstatus);

                                if (it == DLListNULLIterator) {
                                        break;
                                }

                                /* set status to cancelled */
                                stpd = (ScriptedTestProcessDetails *)
                                    dl_list_data (it);
                                stpd->status_ = TP_CANCELED;
                                update_variables (TP_NC);
                                /* kill test process */
                                kill (stpd->pid_, SIGUSR2);
                                waitpid (stpd->pid_, &status, 0);

                                /* cause searching is in range [] */
                                it = dl_list_next (it);
                                it2 = it;

                        } while (it != DLListNULLIterator);
                }

        } else {
                /* We have TP created by callfunc keyword (Scripter) */

                /* check if the test class has modified scripter variables */
                if (dl_list_size (variables) > 0) {
                        receive_variables();
                }
                /* Check result against allowed test result */
                it = dl_list_find (dl_list_head (stpd->allowed_results_)
                                   , dl_list_tail (stpd->allowed_results_)
                                   , _findallowedresult,
                                   (const void *)&param);
                if (it == DLListNULLIterator) {
                        param = TP_FAILED;
                } else {
                        param = TP_PASSED;
                }
                update_variables (param);
                /* Now we can destroy the list. No need to keep allowed
                 * result any more. */
                it = dl_list_head (stpd->allowed_results_);
                while (it != DLListNULLIterator) {
                        tmp = (int *)dl_list_data (it);
                        DELETE (tmp);
                        dl_list_remove_it (it);
                        it = dl_list_head (stpd->allowed_results_);
                }
                dl_list_free (&stpd->allowed_results_);

                scripter_mod.tclass_exec = 0;

                /* Do result reporting stuff */
                tcr = NEW (TestCaseResult);
                tcr->result_ = param;
                STRCPY (tcr->desc_, message, MaxTestResultDescription);

                dl_list_add (stpd->tcr_list_, tcr);
        }
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_end (int mqid, const MsgBuffer * msg)
{
        /* Not needed on this sprint */
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_pause (int mqid, const MsgBuffer * msg)
{
        /* Not needed on this sprint */
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_resume (int mqid, const MsgBuffer * msg)
{
        /* Not needed on this sprint */
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_stop (int mqid, const MsgBuffer * msg)
{
        /* Not needed on this sprint */
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_extif (int mqid, const MsgBuffer * msg)
{
        switch (msg->extif_msg_type_) {
        case EResponseSlave:
                MIN_DEBUG ("EResponseSlave");
                uengine_handle_extif_response (msg->param_, msg->special_);
                break;
        case ERemoteSlaveResponse:
                MIN_DEBUG ("ERemoteSlaveResponse");
                uengine_handle_extif_remote_response (msg->param_,
                                                      msg->special_);
                break;
        default:
                MIN_WARN ("Unknown EXTIF command response: [%d]",
                             msg->param_);
        }
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_extif_response (int result, int caseid)
{
        ScriptedTestProcessDetails *stpd = INITPTR;
        int            *tmp = INITPTR;
        /* check if na error occured - if there is an error then
         * break script execution, if not then resume it. */
        if (result != ENOERR) {
		MIN_WARN ("remote run error: %d", result); 
		SCRIPTER_RTERR ("Remote run error");
		scripter_mod.extif_pending = ESFalse;

                goto EXIT;
        }

        /* special field can carry an test id - if > 0 
         * caseid is > 0 only when response is for REMOTE RUN */
        if (caseid > 0) {

                scripter_mod.extif_remote++;
		MIN_DEBUG ("scripter_mod.extif_remote=%d",
			   scripter_mod.extif_remote);
			   
                stpd = NEW (ScriptedTestProcessDetails);
                stpd->tcr_list_ = dl_list_create ();
                STRCPY (stpd->testclass_, "", 1);
                stpd->pid_ = 0;
                STRCPY (stpd->dllname_, "", 1);
                STRCPY (stpd->cfgfile_, "", 1);
                stpd->mod_type_ = EDLLTypeNormal;
                stpd->options_.expect_ = ENOERR;
                sprintf (stpd->options_.testid_, "%d", caseid); /* caseid */
                STRCPY (stpd->options_.ini_, "", 1);
                stpd->options_.category_ = ECategoryNormal;
                stpd->options_.timeout_ = 0;
                STRCPY (stpd->options_.title_, "", 1);
                stpd->status_ = TP_RUNNING;

                /* Handle result allowing. */
                if (allowedresults == INITPTR) {
                        stpd->allowed_results_ = dl_list_create ();
                        tmp = NEW (int);
                        *tmp = ENOERR;
                        dl_list_add (stpd->allowed_results_, (void *)tmp);
                } else {
                        stpd->allowed_results_ = allowedresults;
                        allowedresults = INITPTR;
                }

                dl_list_add (scripter_mod.tp_details, (void *)stpd);
        }

        /* resume execution */
        scripter_mod.extif_pending = ESFalse;
      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_extif_remote_response (int testresult, int caseid)
{
        DLListIterator  it = DLListNULLIterator;
        char            id[128];
        ScriptedTestProcessDetails *stpd = INITPTR;
        TestCaseResult *tcr;
        scripter_mod.extif_remote--;
	MIN_DEBUG ("scripter_mod.extif_remote=%d",
		   scripter_mod.extif_remote);

        sprintf (id, "%d", caseid);

        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findid, id);

        if (it == DLListNULLIterator) {
                SCRIPTER_RTERR_ARG ("No test of specified caseid", id);
                MIN_WARN ("There is no test of specified caseid [%d]", caseid);
                goto EXIT;
        }
        
        update_variables (testresult);

        stpd = (ScriptedTestProcessDetails *) dl_list_data (it);

        /* set status */
        stpd->status_ = TP_ENDED;
        
        /* set test result */
        tcr = NEW (TestCaseResult);
        tcr->result_ = testresult;
        STRCPY (tcr->desc_, "", MaxTestResultDescription);
        dl_list_add (stpd->tcr_list_, tcr);



      EXIT:
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_event_ind (int mqid, const MsgBuffer * msg)
{
        scripter_mod.event_pending = ESFalse;
                
        if (msg->param_ != EventStatOK) {
                SCRIPTER_RTERR ("Event error");
                MIN_ERROR ("Event error occured");
        }
}

/* ------------------------------------------------------------------------- */
LOCAL void uengine_handle_sndrcv (int mqid, const MsgBuffer * msg)
{

        if (!strcmp (scripter_mod.expected_var, msg->message_)) {
                scripter_mod.extif_pending = ESFalse;
                var_assign (msg->message_, msg->desc_);
        } else {
                SCRIPTER_RTERR_ARG ("Received unexpected  variable",
                                    msg->message_);
                MIN_ERROR ("Expecting variable "
                            "%s, received %s",
                            scripter_mod.expected_var, msg->message_);
        }
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void update_variables (TPResult res)
{
        switch (res) {
        case TP_CRASHED:
                var_increment ("CRASH_COUNT");
                var_increment ("ERROR_COUNT");
                break;
        case TP_TIMEOUTED:
                var_increment ("TOUT_COUNT");
                var_increment ("ERROR_COUNT");
                break;
        case TP_PASSED:
                break;
        case TP_FAILED:
                var_increment ("FAIL_COUNT");
                var_increment ("ERROR_COUNT");
                break;
        case TP_NC:
        case TP_LEAVE:
                var_increment ("ABORT_COUNT");
                var_increment ("ERROR_COUNT");
                break;
        default:
                SCRIPTER_RTERR ("Unexpected result");
                break;
        }
        var_increment ("TOTAL_COUNT");
}

/* ------------------------------------------------------------------------- */
/*LOCAL void scripter_sigchld_handler( int sig )
{
        min_info("Scripter Test process received SIGCHLD");
}*/
/* ------------------------------------------------------------------------- */
LOCAL void scripter_sigalrm_handler (int sig)
{
        DLListIterator  it = DLListNULLIterator;
        DLListIterator  it2 = DLListNULLIterator;
        int             desired = TP_RUNNING;
        int             status = 0;
        ScriptedTestProcessDetails *stpd = INITPTR;

        /* Timeout! Stop execution and end all ongoing tests */
        it2 = dl_list_head (scripter_mod.tp_details);
        do {
                /* find running test */
                it = dl_list_find (it2, dl_list_tail (scripter_mod.tp_details)
                                   , _findstatus, &desired);

                if (it == DLListNULLIterator) {
                        break;
                }

                /* set status to timeout */
                stpd = (ScriptedTestProcessDetails *) dl_list_data (it);
                stpd->status_ = TP_TIMEOUT;

                /* kill test process */
                kill (stpd->pid_, SIGUSR2);
                waitpid (stpd->pid_, &status, 0);

                /* cause searching is in range [] */
                it = dl_list_next (it);
                it2 = it;
        } while (it != DLListNULLIterator);
}

/* ------------------------------------------------------------------------- */
LOCAL int _look4event (const void *a, const void *b)
{
        minEventIf    *e = (minEventIf *) a;
        return strcmp (e->Name (e), (char *)b);
}

/* ------------------------------------------------------------------------- */
LOCAL minEventIf *find_event_reqistration (char *eventname)
{
        DLListIterator  it = DLListNULLIterator;
        it = dl_list_find (dl_list_head (scripter_mod.req_events)
                           , dl_list_tail (scripter_mod.req_events)
                           , _look4event, eventname);

        if (it == DLListNULLIterator) {
                MIN_WARN ("There is no event of given name: [%s]",eventname);
                return INITPTR;
        }
        return (minEventIf *) dl_list_data (it);
}

/* ------------------------------------------------------------------------- */
LOCAL TTestCategory get_result_category (const char *str)
{
        TTestCategory   retval = ECategoryUnknown;

        if (str == INITPTR) {
                errno = EINVAL;
                SCRIPTER_RTERR("no string");
                goto EXIT;
        }

        if (strcmp ("normal", str) == 0) {
                retval = ECategoryNormal;
        } else if (strcmp ("leave", str) == 0) {
                retval = ECategoryLeave;
        } else if (strcmp ("panic", str) == 0) {
                retval = ECategoryPanic;
        } else if (strcmp ("exception", str) == 0) {
                retval = ECategoryException;
        } else if (strcmp ("timeout", str) == 0) {
                retval = ECategoryTimeout;
        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
LOCAL ScriptVar *var_find (const char *varname)
{
        DLListIterator  it;
        ScriptVar      *var;

        it = dl_list_head (variables);
        while (it != INITPTR) {
                var = dl_list_data (it);
                if (!strcmp (var->name_, varname)) {
                        return var;
                }
                it = dl_list_next (it);
        }
        return INITPTR;
}

/* ------------------------------------------------------------------------- */
LOCAL const char *var_value (const char *varname)
{
        ScriptVar      *var;

        var = var_find (varname);
        if (var != INITPTR) {
                if (var->initialized_ == ESFalse) {
                        MIN_WARN ("variable %s not initialized", varname);
                        return "UNINITIALIZED";
                }
                return var->value_;
        }

        return varname;
}

/* ------------------------------------------------------------------------- */
LOCAL void var_assign (const char *varname, const char *varval)
{
        ScriptVar      *var;

        var = var_find (varname);
        if (var != INITPTR) {
                var->initialized_ = ESTrue;
                if (var->value_ != INITPTR)
                        DELETE (var->value_);
                var->value_ = NEW2 (char, strlen (varval) + 1);
                STRCPY (var->value_, varval, strlen (varval) + 1);
                MIN_DEBUG ("%s=%s", varname, varval);
        } else {
                SCRIPTER_RTERR_ARG ("Trying to assign undeclared variable",
                                    varname);
                MIN_WARN ("Trying to assign undeclared variable %s",
                           varname);
        }

        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void var_increment (const char *varname)
{
        ScriptVar      *var;
        long           intval;

        var = var_find (varname);
        if (var == INITPTR || var->initialized_ == ESFalse) {
                SCRIPTER_RTERR_ARG ("Trying to increment undeclared variable",
                                    varname);
                
        }
        intval = strtol (var->value_, NULL, 10);
        DELETE (var->value_);
        intval ++;
        var->value_ = NEW2 (char, 32);
        sprintf (var->value_, "%ld", intval);

        return;
}


/* ------------------------------------------------------------------------- */
LOCAL void send_variables ()
{
        DLListIterator   it;
        ScriptVar       *var;
        Text            *vars;
        void            *sh_mem_handle = NULL, *tmp;

        vars = tx_create (INITPTR);

        for (it = dl_list_head(variables); it != INITPTR; 
             it = dl_list_next(it)) {
                var = dl_list_data (it);
                tx_c_append (vars, var->name_);
                if (var->initialized_) {
                        tx_c_append (vars, "=");
                        tx_c_append (vars, var->value_);
                }
                tx_c_append (vars, " ");
        }

        sh_mem_handle = sm_attach (scripter_mod.shm_id);
        
        if ((vars->size_ + sizeof (unsigned int)) >  
            sm_get_segsz (scripter_mod.shm_id)) {
                SCRIPTER_RTERR ("Variables do not fit into SHM");
                tx_destroy (&vars);
                return;
        }
        
        sm_write (sh_mem_handle,
                  (void *)&vars->size_,
                  sizeof (unsigned int));
        tmp = sh_mem_handle + sizeof (unsigned int);
        sm_write (tmp,
                  (void *)(tx_share_buf (vars)),
                  vars->size_);

        MIN_DEBUG ("SENT VARIABLES: %s", tx_share_buf(vars));
        sm_detach (sh_mem_handle);
        
        tx_destroy (&vars);
}
/* ------------------------------------------------------------------------- */
LOCAL void receive_variables ()
{
        void *shmaddr;
        unsigned int size;
        char *var_buff = INITPTR, *variable_str, *p;
        MinItemParser *mip;

        shmaddr = sm_attach (scripter_mod.shm_id);
        if (shmaddr == INITPTR) {
                SCRIPTER_RTERR ("Could not attach to shared memory segment");
                return;
        }

        if ( sm_read (shmaddr, &size, sizeof (unsigned int)) != ENOERR) {
                SCRIPTER_RTERR ("Read from shared memory segment failed");
                goto exit;
        } else if (size == 0) {
                goto exit;
        }
        /*
        ** Read the variable buffer from shared memory
        */
        var_buff = NEW2 (char, size + 1);
        sm_read (shmaddr + sizeof (unsigned int), var_buff, size); 
        var_buff [size] = '\0';
        MIN_DEBUG ("RECEIVED from test class: %s", var_buff);
        /*
        ** Create Item Parser for variable parsing
        */
        mip = mip_create (var_buff, 0, size);
        if (mip == INITPTR) {
                SCRIPTER_RTERR ("Failed to create item parser from variables");
                goto exit;
        }
        /*
        ** Parse the variables and save them to variable list
        */
        mip_get_string (mip, INITPTR, &variable_str);
        while (variable_str != INITPTR) {
                if ((p =  strrchr (variable_str, '=')) == NULL) {  
                        if (var_find (variable_str) == INITPTR) {
                                SCRIPTER_RTERR_ARG ("Undeclared variable from"
                                            "testclass", variable_str);
                                DELETE (variable_str);
                                goto exit;
                        }
                } else {
                        /* 
                        ** test class may have set the variable
                        */
                        *p = '\0';
                        p++;
                        
                        if (strlen (p) > 0) {
                                /*
                                ** try to assign the new value
                                */
                                var_assign (variable_str, p);
                                DELETE (variable_str);
                        } else if (var_find (variable_str) == INITPTR) {
                                /*
                                ** Check that the variable is declared
                                */
                                SCRIPTER_RTERR_ARG ("undeclared variable from "
                                                    "testclass", variable_str);
                                DELETE (variable_str);
                                goto exit;
                        
                        }
                }
                if (mip_get_next_string (mip, &variable_str) != ENOERR)
                        break;
        } 

exit:
        if (mip != INITPTR)
                mip_destroy (&mip);
        if (var_buff != INITPTR)
                DELETE (var_buff);

        sm_detach (shmaddr);

        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void scripter_final_verdict (DLList * tp_details, TestCaseResult * tcr)
{
        ScriptedTestProcessDetails *stpd;
        DLListIterator  tpit, reit;
        TestCaseResult *res;
        int             num_fail, num_ok, cancelled;
        num_fail = num_ok = cancelled = 0;
        char *result_str [] = {"CRASHED",
                               "TIMEOUTED",
                               "TP_PASSED",
                               "FAILED",
                               "NOT COMPLETE",
                               "LEFT"};
        
        for (tpit = dl_list_head (tp_details);
             tpit != DLListNULLIterator; tpit = dl_list_next (tpit)) {
                stpd = dl_list_data (tpit);
                switch (stpd->status_) {
                case TP_ENDED:
                case TP_ABORTED:
                        break;
                case TP_CANCELED:
                        snprintf (tcr->desc_, 
                                  MaxTestResultDescription - 1, 
                                  "Run %s(%d) cancelled  ",
                                  stpd->dllname_,
                                  stpd->tc_id_
                                );
                        cancelled = 1;
                        continue;
                default:
                        MIN_WARN ("Unexpected status of scripter "
                                  "test case %d", stpd->status_);
                        snprintf (tcr->desc_, 
                                  MaxTestResultDescription - 1,
                                  "Unexpected status (%d) of scripter"
                                  "test case", stpd->status_);
                                  
                        num_fail++;
                        continue;
                        
                }
                for (reit = dl_list_head (stpd->tcr_list_);
                     reit != DLListNULLIterator; reit = dl_list_next (reit)) {
                        res = dl_list_data (reit);
                        switch (res->result_) {
                        case TP_CRASHED:
                        case TP_FAILED:
                        case TP_TIMEOUTED:
                        case TP_LEAVE:
                                num_fail++;
                                if (strlen (tcr->desc_) > 0) 
                                        break;
                                if (stpd->mod_type_ == EDLLTypeClass)
                                        snprintf (tcr->desc_, 
                                                  MaxTestResultDescription - 1,
                                                  "Test Method call \"%s.%s\""
                                                  "  %s",
                                                  stpd->testclass_, 
                                                  res->desc_,
                                                  result_str 
                                                  [res->result_ + 2]);
                                else {
                                        snprintf (tcr->desc_,
                                                  MaxTestResultDescription - 1, 
                                                  "Run %s(%d) %s: %s ",
                                                  stpd->dllname_,
                                                  stpd->tc_id_,
                                                  result_str 
                                                  [res->result_ + 2],
                                                  res->desc_
                                                );
                                }
                                break;
                        case TP_PASSED:
                                num_ok++;
                                break;
                        case TP_NC:
                                MIN_DEBUG ("TP_NC .. unxepected result code");
                                break;
                                num_fail++;
                                break;
                        default:
                                MIN_DEBUG ("unknown result code %d",
                                             tcr->result_);
                                break;
                        }
                }
        }

        if (cancelled) {
                tcr->result_ = TP_NC;
        } else if (num_fail > 0) {
                if (num_fail > 1)
                        sprintf (tcr->desc_, "%s (%d other errors/failures)",
                                 tcr->desc_, num_fail - 1);
                tcr->result_ = TP_FAILED;
        } else {
                tcr->result_ = TP_PASSED;
                sprintf (tcr->desc_, "Scripted test case passed");
        }
        MIN_DEBUG ("returning from scripter with desc %s",
                   tcr->desc_);
        
}


/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int substract_timeval (struct timeval *result, struct timeval *x,
                       struct timeval *y)
{
        /* Perform the carry for the later subtraction by updating y. */
        if (x->tv_usec < y->tv_usec) {
                int             nsec =
                    (y->tv_usec - x->tv_usec) / 1000000 + 1;
                y->tv_usec -= 1000000 * nsec;
                y->tv_sec += nsec;
        }
        if (x->tv_usec - y->tv_usec > 1000000) {
                int             nsec = (x->tv_usec - y->tv_usec) / 1000000;
                y->tv_usec += 1000000 * nsec;
                y->tv_sec -= nsec;
        }

        /* Compute the time remaining to wait.
         * tv_usec is certainly positive. */
        result->tv_sec = x->tv_sec - y->tv_sec;
        result->tv_usec = x->tv_usec - y->tv_usec;

        /* Return 1 if result is negative. */
        return x->tv_sec < y->tv_sec;
}

/* ------------------------------------------------------------------------- */
int add_timeval (struct timeval *result, struct timeval *x, struct timeval *y)
{
        /* add it */
        result->tv_usec = x->tv_usec + y->tv_usec;
        result->tv_sec = x->tv_sec + y->tv_sec;

        /* round it */
        while (result->tv_usec < 1000000) {
                result->tv_usec -= 1000000;
                result->tv_sec++;
        }


        return 1;
}

/* ------------------------------------------------------------------------- */
int testclass_create (filename_t dllName, char *className)
{
        int             retval = ENOERR;
        pid_t           pid = -1;
        char           *dllpath = NEW2 (char, strlen (dllName) + 3 + 1);
        ScriptedTestProcessDetails *stpd = INITPTR;
        MsgBuffer       input_buffer;

        if (strlen (dllName) == 0) {
                errno = EINVAL;
                retval = -1;
                SCRIPTER_RTERR ("Library is 0 lenght");
                goto EXIT;
        }
        if (className == INITPTR) {
                errno = EINVAL;
                SCRIPTER_RTERR ("class name missing");
                retval = -1;
                goto EXIT;
        }

        STRCPY (dllpath, dllName, strlen (dllName));
        STRCPY (&dllpath[strlen (dllName)], ".so", 4);

        pid = fork ();
        if (pid == 0) {
                /* Child process, lets enter event loop */
                stprun = ESTrue;
                sched_yield ();
                min_log_open ("ScriptedTP", 3);
                mq_init_buffer ();
                signal (SIGUSR2, stp_handle_sigusr2);
                
                fetch_ptr2run (dllpath, className);

                while (stprun) {
                        if (mq_peek_message (scripter_mod.mqid, getpid())) {
                                retval = mq_read_message (scripter_mod.mqid,
                                                          getpid ()
                                                          , &input_buffer);
                                stp_handle_message (&input_buffer);
                        }
                        usleep (10000);
                }
                stp_exit();
        } else if (pid > 0) {
                /* TestProcess details are stored on the list */
                MIN_INFO ("Scripted Test Process created, pid: %d",getpid());
                stpd = NEW (ScriptedTestProcessDetails);
                stpd->tcr_list_ = dl_list_create ();
                stpd->pid_ = pid;
                stpd->mod_type_ = EDLLTypeClass;
                stpd->status_ = TP_NONE;
                STRCPY (stpd->testclass_, className, 128);
                STRCPY (stpd->dllname_, dllpath, MaxFileName);
                dl_list_add (scripter_mod.tp_details, (void *)stpd);
        } else {
                /* Error */
                SCRIPTER_RTERR ("Cannot create Scripted Test Process!");
                MIN_ERROR ("Cannot create Scripted Test Process!");
        }
        DELETE (dllpath);
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int testclass_call_function (char *className, MinItemParser * mip)
{
        int             retval = ENOERR;
        int            *tmp = INITPTR;
        DLListIterator  it = DLListNULLIterator;
        ScriptedTestProcessDetails *stpd = INITPTR;
        MsgBuffer       msg;
        char           *token = INITPTR;

	memset(&msg, 0x0, sizeof(MsgBuffer));

        if (className == INITPTR || strlen (className) == 0) {
                errno = EINVAL;
                SCRIPTER_RTERR ("class name missing");
                retval = -1;
                goto EXIT;
        }

        /* 1)  look for classname on list */

        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findclass, (void *)className);

        if (it == DLListNULLIterator) {
                errno = ENODATA;
                retval = -1;
                SCRIPTER_RTERR_ARG ("Class not found on list", className);
                MIN_ERROR ("Class not found on list [%s]", className);
                goto EXIT;
        }

        stpd = (ScriptedTestProcessDetails *) dl_list_data (it);

        /* 2) Fill msg structure with data */
        msg.receiver_ = stpd->pid_;
        msg.sender_ = getpid ();
        msg.type_ = MSG_RUN;
        msg.param_ = 0;

        msg.desc_[0] = '\0';
        if (dl_list_size (variables) == 0) {
                STRCPY (msg.desc_, mip->item_skip_and_mark_pos_, MaxDescSize);
		if (msg.desc_ [MaxDescSize - 1] != '\0' &&
                    msg.desc_ [MaxDescSize - 1] != 0x0){
			SCRIPTER_RTERR_ARG ("Method call too long", className);
			msg.desc_[MaxDescSize-1] = '\0';
		}
        } else {
                while (mip_get_next_string (mip, &token) == ENOERR) {
                        if (strlen (msg.desc_) + strlen (var_value (token)) 
                            >= MaxDescSize) {
                                SCRIPTER_RTERR_ARG ("Method call too long", 
                                                    className);
                                msg.desc_[MaxDescSize-1] = '\0';
                        
                        } else
                                sprintf (msg.desc_, "%s %s", msg.desc_,
                                         var_value (token));
                }
                DELETE (token);
                send_variables ();
                msg.special_ = scripter_mod.shm_id;
        }

        STRCPY (msg.message_, className, MaxMsgSize);
        msg.special_ = 0;
        /* 3) Handle result allowing. */
        if (allowedresults == INITPTR) {
                stpd->allowed_results_ = dl_list_create ();
                tmp = NEW (int);
                *tmp = ENOERR;
                dl_list_add (stpd->allowed_results_, (void *)tmp);
        } else {
                stpd->allowed_results_ = allowedresults;
                allowedresults = INITPTR;
        }

        /* 4) send MSG_RUN command */
        mq_send_message (scripter_mod.mqid, &msg);

        /* 5) Block the script execution until we get result back */
        scripter_mod.tclass_exec = 1;
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int test_run (const char *modulename, const char *configfile, unsigned int id,
              MinItemParser * mip)
{
        ScriptedTestProcessDetails *stpd = INITPTR;
        int             retval = ENOERR;
        int             tmp = 0;
        test_libl_t     testlib;
        DLList         *cases = INITPTR;
        DLListIterator  it = DLListNULLIterator;
        struct sigaction pause_action;

        TestCaseInfo   *tci = INITPTR;
        TestCaseResult  tcr;
        if (modulename == INITPTR) {
                errno = EINVAL;
                retval = -1;
                goto EXIT;
        }
        if (configfile == INITPTR) {
                errno = EINVAL;
                retval = -1;
                goto EXIT;
        }
        if (mip == INITPTR) {
                errno = EINVAL;
                retval = -1;
                goto EXIT;
        }

        stpd = NEW (ScriptedTestProcessDetails);
        stpd->tcr_list_ = dl_list_create ();

        /* 1) Fetch possible optional parameters. */
        read_optional_run_params (mip, &stpd->options_);

        testlib.test_library_ = tl_open_tc (modulename);
        strncpy (testlib.fname_, modulename, 255);
        testlib.get_cases_fun_ = (ptr2case) dlsym (testlib.test_library_,
                                                   "tm_get_test_cases");
        testlib.run_case_fun_ = (ptr2run) dlsym (testlib.test_library_,
                                                 "tm_run_test_case");

        if (tl_is_ok (&testlib) == 0) {
                retval = -1;
                SCRIPTER_RTERR_ARG ("Error opening", modulename);
                goto EXIT;
        }
        /* Now we can execute a test. If test case title is
         * specified then we have to use it, otherwise the 
         * id can be used */
        if (strlen (stpd->options_.title_) > 0) {
                /* Use title, so first we have to find out
                 * what is the id of the case of given name. */
                cases = dl_list_create ();
                testlib.get_cases_fun_ (configfile, &cases);
                it = dl_list_head (cases);
                id = 1;
                while (it != DLListNULLIterator) {
                        tci = (TestCaseInfo *) dl_list_data (it);
                        tmp = strcmp (tci->name_, stpd->options_.title_);
                        if (tmp == 0) {
                                break;
                        }
                        it = dl_list_next (it);
                        id++;
                }
                dl_list_free_data (&cases);
                dl_list_free (&cases);
        }

        /* 3) Run a test */
        stpd->pid_ = fork ();
        if (stpd->pid_ > 0) {
                /* Parent */
                STRCPY (stpd->testclass_, "\0", 1);
                STRCPY (stpd->dllname_, modulename, MaxFileName);
                STRCPY (stpd->cfgfile_, configfile, MaxFileName);
                stpd->tc_id_ = id;
                stpd->mod_type_ = EDLLTypeNormal;
                stpd->status_ = TP_RUNNING;
                scripter_mod.testcomplete = INITPTR;
                dl_list_add (scripter_mod.tp_details, (void *)stpd);
        } else if (stpd->pid_ == 0) {
                /* Child */
                ctprun = ESTrue;
                min_log_open ("Combined Test Process", 3);
                MIN_INFO ("Combined Test Process created, pid: %d",getpid());
                mq_init_buffer ();
                sched_yield ();
                signal (SIGUSR2, ctp_handle_sigusr2);

                pause_action.sa_sigaction = ctp_hande_sigtstp;
                sigfillset (&pause_action.sa_mask);
                pause_action.sa_flags = SA_NOCLDSTOP | SA_SIGINFO;
                sigaction (SIGTSTP, &pause_action, NULL);
                testlib.run_case_fun_ (id, configfile, &tcr);
                mq_send_message2 (scripter_mod.mqid, getppid ()
                                  , MSG_RET, tcr.result_, tcr.desc_);
                while (ctprun) {
                        usleep (10000);
                }               /* Wait for death */
                /* resend buffered, flush message buffer */
                mq_resend_buffered ();
                mq_flush_msg_buffer ();
                /* At the end exit gracefully. */
                exit (TP_EXIT_SUCCESS);
        } else {
                SCRIPTER_RTERR ("Test Process NOT created");
                MIN_ERROR ("Combined Test Process NOT created");
                goto EXIT;
        }

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int testclass_destroy (char *className)
{
        DLListIterator  it;
        int             retval = ENOERR;
        ScriptedTestProcessDetails *stpd = INITPTR;

        if (className == INITPTR) {
                errno = EINVAL;
                SCRIPTER_RTERR ("No class name");
                goto EXIT;
        }
        if (strlen (className) == 0) {
                errno = EINVAL;
                SCRIPTER_RTERR ("Class name has 0 lenght");
                retval = -1;
                goto EXIT;
        }

        /* clean-up code */
        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findclass, (void *)className);
        if (it == DLListNULLIterator) {
                errno = ENODATA;
                retval = -1;
                SCRIPTER_RTERR_ARG("Class not found on list]", className);
                MIN_ERROR ("Class not found on list");
                goto EXIT;
        }

        stpd = (ScriptedTestProcessDetails *) dl_list_data (it);

        stpd->status_ = TP_ENDED;
        retval = kill (stpd->pid_, SIGUSR2);
        if (retval == -1)
                MIN_WARN ("Cannot kill %d", stpd->pid_);
        else
                MIN_DEBUG ("Killed %d", stpd->pid_);
        sleep (1);

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int event_request (char *eventname, int is_state)
{
        int             retval = ENOERR;
        minEventIf    *event;

        /*
         ** We can't be registered for the same event more than once 
         */
        event = find_event_reqistration (eventname);
        if (event != INITPTR) {
                SCRIPTER_RTERR_ARG ("Process already registered for event",
                                    eventname);
                MIN_NOTICE ("Process already registered for event %s",
                             eventname);
                return -1;
        }

        /*
         ** Create and request the event
         */
        event =
            min_event_create (eventname, is_state ? EState : EIndication);
        event->dont_block_ = ESTrue;
        scripter_mod.event_pending = ESTrue;
        gettimeofday (&scripter_mod.blocking_since, NULL);
        event->SetType (event, EReqEvent);
        Event (event);

        /*
         ** Add event to the local list of requested event or destroy
         */
        dl_list_add (scripter_mod.req_events, event);
        retval = ENOERR;

        return retval;
}

/* ------------------------------------------------------------------------- */
int event_wait (char *eventname)
{
        int             retval = ENOERR;
        minEventIf    *event;

        /*
         ** We must be registered for the event in order to wait
         */
        event = find_event_reqistration (eventname);
        if (event == INITPTR) {
                SCRIPTER_RTERR_ARG ("No registration for event", eventname);
                MIN_WARN ("No registarion for event %s", eventname);
                return -1;
        }

        /*
         ** Wait for the event
         */
        event->SetType (event, EWaitEvent);
        scripter_mod.event_pending = ESTrue;
        gettimeofday (&scripter_mod.blocking_since, NULL);
        Event (event);

        if (event->event_status_ == EventStatOK) {
                retval = ENOERR;
        } else {
                SCRIPTER_RTERR_ARG ("Event() returned with error", eventname);
                MIN_WARN ("Event():%s returned with error", eventname);
                retval = -1;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
int event_release (char *eventname)
{
        int             retval = ENOERR;
        minEventIf    *event;

        /*
         ** We must be registered for the event in order release it
         */
        event = find_event_reqistration (eventname);
        if (event == INITPTR) {
                SCRIPTER_RTERR_ARG ("No registration for event %s", eventname);
                MIN_WARN ("No registration for event %s", eventname);
                return -1;
        }

        /*
         ** Release the event
         */
        event->SetType (event, ERelEvent);
        scripter_mod.event_pending = ESTrue;
        gettimeofday (&scripter_mod.blocking_since, NULL);
        Event (event);

        if (event->event_status_ == EventStatOK) {
                retval = ENOERR;
        } else {
                SCRIPTER_RTERR_ARG ("Event() returned with error", eventname);
                MIN_WARN (" Event():%s returned with error", eventname);
                retval = -1;
        }

        return retval;
}

/* ------------------------------------------------------------------------- */
int event_set (char *eventname, int is_state)
{
        int             retval = ENOERR;
        minEventIf    *event;
        event =
            min_event_create (eventname, is_state ? EState : EIndication);
        event->dont_block_ = ESTrue;
        event->SetType (event, ESetEvent);
        scripter_mod.event_pending = ESTrue;
        gettimeofday (&scripter_mod.blocking_since, NULL);
        Event (event);
        if (event->event_status_ == EventStatOK) {
                retval = ENOERR;
        } else {
                SCRIPTER_RTERR_ARG ("Event() returned with error", eventname);
                MIN_WARN ("Event():%s returned with error", eventname);
                retval = -1;
        }

        min_event_destroy (event);

        return retval;
}

/* ------------------------------------------------------------------------- */
int event_unset (char *eventname)
{
        int             retval = ENOERR;
        minEventIf    *event;

        event = min_event_create (eventname, EState);
        event->dont_block_ = ESTrue;
        event->SetType (event, EUnsetEvent);
        scripter_mod.event_pending = ESTrue;
        gettimeofday (&scripter_mod.blocking_since, NULL);
        Event (event);
        if (event->event_status_ == EventStatOK) {
                retval = ENOERR;
        } else {
                SCRIPTER_RTERR_ARG ("Event() returned with error", eventname);
                MIN_WARN ("Event():%s returned with error", eventname);
                retval = -1;
        }

        min_event_destroy (event);

        return retval;
}

/* ------------------------------------------------------------------------- */
int script_finish (void)
{
        int             retval = ENOERR;
        scripter_mod.script_finished = 1;
        return retval;
}

/* ------------------------------------------------------------------------- */
int tm_run_test_case (unsigned int id, const char *cfg_file,
                      TestCaseResult * result)
{
        int             retval = ENOERR;
        MinParser      *sp = INITPTR;
        DLListIterator  prev_it, it = DLListNULLIterator;
        MinSectionParser *msp = INITPTR;
        TSBool          msg_pending = ESFalse;
        pid_t           pid = getpid ();
        char           *path = INITPTR;
        char           *c = INITPTR;
        MsgBuffer       input_buffer;
        ScriptVar      *var;
        ScriptedTestProcessDetails *stpd;
        Text           *cfgdefault;
        char *dir = INITPTR;
        char *file = INITPTR;

        if (cfg_file == INITPTR) {
                errno = EINVAL;
                retval = -1;
                SCRIPTER_RTERR("config file missing");
                goto EXIT;
        }
        if (result == INITPTR) {
                errno = EINVAL;
                SCRIPTER_RTERR("result argument missing");
                retval = -1;
                goto EXIT;
        }
        if (id < 1) {
                errno = EINVAL;
                SCRIPTER_RTERR("invalid test case id");
                retval = -1;
                goto EXIT;
        }

        /* This function is called by TMC just after fork, so it is good
         * place to initialize scripter internal structures so that they
         * can be independent. */

        /* 1) Initialize Parser */
        path = NEW2 (char, strlen (cfg_file) + 1);
        STRCPY (path, cfg_file, strlen (cfg_file) + 1);
        c = strrchr (path, '/');
        if (c == NULL) {
                cfgdefault = tx_create(getenv("HOME"));
                tx_c_append(cfgdefault,"/.min");
                c = tx_share_buf(cfgdefault);
                dir = c;
                file=path;
        }
        else {
                *c = '\0';
                c++;
                dir=path;
                file=c;
        }

        sp = mp_create (dir, file, ENoComments);
        if (sp == INITPTR) {
		SCRIPTER_RTERR_ARG("Min Parser failed to create", c);
                MIN_ERROR ("Min Parser failed to initialize, params: [%s][%s]",
                            path, c);
                retval = -1;
                goto EXIT;
        }

        msp = mp_section (sp, "[Test]", "[Endtest]", id);
        if (msp == INITPTR) {
		SCRIPTER_RTERR("Min Parser failed to create");
                MIN_ERROR ("Min Section Parser failed to initialize,"
                            " params: [%s][%s]",path, c);
                retval = -1;
                goto EXIT;
        }

        /* 2) Create Lego Snake */
        mli_create (msp);

        /* 3) Initialize scripter and events stuff and shm */
        scripter_mod.tp_details = dl_list_create ();
        if (scripter_mod.tp_details == INITPTR) {
                retval = -1;
                SCRIPTER_RTERR("list creation failed");
                goto EXIT;
        }

        scripter_mod.req_events = dl_list_create ();
        scripter_mod.mqid = mq_open_queue ('a');
        scripter_mod.script_finished = 0;
        scripter_mod.tclass_exec = 0;
        scripter_mod.extif_pending = ESFalse;
        scripter_mod.extif_remote = 0;
        scripter_mod.testcomplete = INITPTR;
        scripter_mod.canceliferror = ESFalse;
	scripter_mod.error_occured = ESFalse;
        scripter_mod.sleep = 0;
        scripter_mod.expected_var = INITPTR;
	scripter_mod.blocking_timeout = DEFAULT_BLOCKING_TIMEOUT;
        scripter_mod.shm_id = shmget (getpid(), 4096, 
                                      IPC_CREAT | 0660);

        if (scripter_mod.shm_id < 0) {
                MIN_ERROR ("Failed to create shared mem segment: %s",
                          strerror (errno));
                goto EXIT;
        }
        MIN_DEBUG ("shmid = %d", scripter_mod.shm_id); 
        /* 4) Initialize scripter (the callbacks) */
        scripter_init (&scripter_mod.min_scripter_if);

        /* 5) Set test result to default (Not Complete) */
        scripter_mod.script_tcr.result_ = TP_NC;
        memset (scripter_mod.script_tcr.desc_, 0x0, MaxTestResultDescription);

        /* 6) Set SIGCHLD and SIGALRM signal handlers */
        /* signal (SIGCHLD, scripter_sigchld_handler); */
        signal (SIGCHLD, SIG_IGN);
        /* 7) The uEngine main loop */
        while (!scripter_mod.script_finished || _pending_tests ()) {

                /* Check if any messages to us. */
                msg_pending = mq_peek_message (scripter_mod.mqid, pid);

                /* If message is waiting then read it and handle. */
                if (msg_pending || scripter_mod.script_finished) {

                        uengine_read_message (scripter_mod.mqid,
                                              &input_buffer);
                        uengine_handle_message (scripter_mod.mqid,
                                                &input_buffer);
                }

                /* Execute line of a script. */
                if (_execute_script ()) {
                        interpreter_next ();
                } else {
                        /*sleep to avoid eating up too much cpu time
                          smaller usleep times are known to cause
                          problems in some hardwares*/
                        usleep(10000);
                }

        }

        /* 8) Set test case result */
	if (!scripter_mod.error_occured)
		scripter_final_verdict (scripter_mod.tp_details,
					&scripter_mod.script_tcr);
      EXIT:
        /* set test case result, default is passed. */
        *result = scripter_mod.script_tcr;

        /* Do some cleanup */

        if (scripter_mod.expected_var != INITPTR) {
                DELETE (scripter_mod.expected_var);
        }

        it = dl_list_tail (defines);
        while (it != DLListNULLIterator) {
                free (dl_list_data (it));
                prev_it = dl_list_prev (it);
                dl_list_remove_it (it);
                it = prev_it;
        }

        it = dl_list_tail (variables);
        while (it != DLListNULLIterator) {
                var = dl_list_data (it);
                DELETE (var->name_);
                if (var->value_ != INITPTR)
                        DELETE (var->value_);
                DELETE (var);
                prev_it = dl_list_prev (it);
                dl_list_remove_it (it);
                it = prev_it;
        }

        mli_destroy ();

        it = dl_list_tail (scripter_mod.tp_details);
        while (it != DLListNULLIterator) {
                stpd = dl_list_data (it);
                dl_list_foreach (dl_list_head (stpd->tcr_list_),
                                 dl_list_tail (stpd->tcr_list_), free);
                dl_list_free (&stpd->tcr_list_);
                DELETE (stpd);
                prev_it = dl_list_prev (it);
                dl_list_remove_it (it);
                it = prev_it;
        }

        if (sp != INITPTR) {
                mp_destroy (&sp);
        }
        if (msp != INITPTR) {
                mmp_destroy (&msp);
        }
        DELETE (path);

        if (scripter_mod.shm_id > 0)
            sm_destroy (scripter_mod.shm_id);

        return retval;
}

/* ------------------------------------------------------------------------- */
int tm_get_test_cases (const char *cfg_file, DLList ** cases)
{
        int             retval = ENOERR;
        MinParser     *sp = INITPTR;
        MinSectionParser *msp = INITPTR;
        char           *cfg_file_backup = INITPTR;
        char           *c = INITPTR;
        unsigned int    section_number = 1;
        char           *test_case_name = INITPTR, *tmp;
        Text *homemindir = INITPTR;
        char *dir = INITPTR;
        char *file = INITPTR;
       
        /* 0) First call initializes global list of libraries and ptrs
         *    to api functions from them. Deallocating of this list is
         *    unfortunatelly quite problematical.
         */
        if (tp_handlers == INITPTR) {
                tp_handlers = dl_list_create ();
        }


        if (defines == INITPTR) {
                defines = dl_list_create ();
        }


        if (variables == INITPTR) {
                variables = dl_list_create ();
                declare_internal_var ("FAIL_COUNT");
                declare_internal_var ("CRASH_COUNT");
                declare_internal_var ("TOUT_COUNT");
                declare_internal_var ("ABORT_COUNT");
                declare_internal_var ("ERROR_COUNT");
                declare_internal_var ("TOTAL_COUNT");
        }


        /* 1) Perform POST */
        if (cfg_file == INITPTR) {
                retval = -1;
                errno = EINVAL;
                SCRIPTER_RTERR ("Configuration file missing");
                goto EXIT;
        }
        if (*cases == INITPTR) {
                retval = -1;
                errno = EINVAL;
                SCRIPTER_RTERR ("Cases list missing");
                goto EXIT;
        }

        /* 2) Initialize the scripter plugin */
        scripter_init (&scripter_mod.min_scripter_if);

        /* 3) Validate script syntax.
         *    This is connected to returning test cases. If test case is
         *    valid then its title is added to the *cases list, if not then
         *    title is not added and warning is logged */

        cfg_file_backup = NEW2 (char, strlen (cfg_file) + 1);
        STRCPY (cfg_file_backup, cfg_file, strlen (cfg_file) + 1);

        c = strrchr (cfg_file_backup, '/');
        if (c == NULL) {
                c = getenv("HOME");
                if (c) {
                        homemindir = tx_create("");
                        tx_c_append (homemindir,c);
                        tx_c_append (homemindir,"/.min");
                        dir = tx_share_buf(homemindir);
                        file = cfg_file_backup;
                } else c = cfg_file_backup;
        } else {
                *c = '\0';
                c++;
                dir = cfg_file_backup;
                file = c;
        }

        sp = mp_create (dir, file, ENoComments);

        if (sp == INITPTR) {
		tmp = NEW2 (char, strlen (cfg_file_backup) + strlen (c) + 
                            strlen ("MinParser not created with "
                                    "params:[%s][%s]") +1 );
		sprintf (tmp, "MinParser not created with params:"
			 "[%s][%s]", cfg_file_backup, c);
		SCRIPTER_RTERR_ARG(tmp, strerror (errno));
		DELETE (tmp);
                MIN_ERROR ("MinParser failed to be created, params:"
                            "[%s][%s]: errno: %d",
                            cfg_file_backup, c, errno);
                retval = -1;
                goto EXIT;
        }


        /* Lets validate DEFINE section if present */
        msp = mp_section (sp, "[Define]", "[Enddefine]", 1);
        if (msp != INITPTR) {
                if (validate_define (msp) == ESFalse) {
                        /* define section is not correct --> no sense 
                           to process further */
                        mmp_destroy (&msp);
                        mp_destroy (&sp);
                        DELETE (cfg_file_backup);
                        dl_list_free (&tp_handlers);
                        goto EXIT;
                }
        }

        mmp_destroy (&msp);

        /* Let's validate TEST CASES */
        msp = mp_section (sp, "[Test]", "[Endtest]", section_number);
        while (msp != INITPTR) {

                /* validate syntax. In return we get TC name or INITPTR */
                test_case_name = validate_test_case (msp);
                if (test_case_name == NULL)
                        test_case_name = INITPTR;

                /* add TC to list */
                if (test_case_name != INITPTR) {
                        ENTRY2 (*cases  /* List of cases */
                                , test_case_name        /* TC name       */
                                , section_number);      /* TC id         */
                        DELETE (test_case_name);
                } else {
                        MIN_WARN ("Test case is not valid! [%d]",
                                   section_number);

                }

                /* fetch next TC from file */
                section_number++;
                mmp_destroy (&msp);
                msp = mp_section (sp, "[Test]", "[Endtest]", section_number);
        }

        tx_destroy(&homemindir);
        mp_destroy (&sp);
        DELETE (cfg_file_backup);
        MIN_INFO ("Number of cases %d", dl_list_size (*cases));
      EXIT:

        return retval;
}

/* ------------------------------------------------------------------------- */
int testclass_print (MinItemParser * mip)
{
        int             retval = ENOERR;
        char           *token = INITPTR;
        Text           *txt;
        
        txt = tx_create (INITPTR);

        while (mip_get_next_string (mip, &token) == ENOERR) {
                tx_c_append (txt, var_value (token));
                tx_c_append (txt, " ");
        }
        DELETE (token);
        tx_c_prepend (txt, "script: ");
        tm_printf (5, "script", tx_share_buf (txt));

        tx_destroy (&txt);
        return retval;
}

/* ------------------------------------------------------------------------- */
int testclass_allownextresult (int nextresult, MinItemParser * mip)
{
        int             retval = ENOERR;
        int            *result = INITPTR;

        if (allowedresults == INITPTR) {
                allowedresults = dl_list_create ();
        }

        /* Add one mandatory result and continue if there are more of them */
        do {
                result = NEW (int);
                *result = nextresult;
                dl_list_add (allowedresults, (void *)result);
                retval = mip_get_next_int (mip, &nextresult);
        } while (retval == ENOERR);

        /* At this point function return no error */
        retval = ENOERR;
        return retval;
}

/* ------------------------------------------------------------------------- */
int testclass_allowerrorcodes (int nexterrorcode, MinItemParser * mip)
{
        int             retval = ENOERR;
        int            *error = INITPTR;

        if (allowederrors == INITPTR) {
                allowederrors = dl_list_create ();
        }

        /* Add one mandatory error and continue if there are more of them */
        do {
                error = NEW (int);
                *error = nexterrorcode;
                dl_list_add (allowederrors, (void *)error);
                retval = mip_get_next_int (mip, &nexterrorcode);
        } while (retval == ENOERR);

        /* At this point function return no error */
        retval = ENOERR;
        return retval;
}

/* ------------------------------------------------------------------------- */
int test_complete (const char *testid)
{
        int             retval = ENOERR;
        DLListIterator  it = DLListNULLIterator;
        ScriptedTestProcessDetails *stpd;
	int 		tc_not_ended=0;

        if (testid == INITPTR) {
                retval = -1;
                errno = EINVAL;
                SCRIPTER_RTERR ("test id missing");
                goto EXIT;
        }

        /* Check if test of given testid exists */
        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findid, (void *)testid);
        if (it == DLListNULLIterator) {
                SCRIPTER_RTERR_ARG ("No test of specified testid", testid);
                MIN_WARN ("No test of specified testid [%s]", testid);
                retval = -1;
                goto EXIT;
        }

        /* Check if test with given testid has already completed */
        while (it != DLListNULLIterator) {
                stpd = dl_list_data (it);
                if (stpd->status_ != TP_ENDED) {
			tc_not_ended = tc_not_ended + 1;
                }
                it = dl_list_next (it);
                if (it == DLListNULLIterator)
                        break;
                it = dl_list_find (it, dl_list_tail (scripter_mod.tp_details),
                                   _findid, (void *)testid);
        }
	if(tc_not_ended==0){
                MIN_WARN ("All tests with testid= %s already completed", testid);
                goto EXIT;
	}
        /* Set 'wait for complete' flag */
        scripter_mod.testcomplete = NEW2 (char, strlen (testid) + 1);
        STRCPY (scripter_mod.testcomplete, testid, strlen (testid) + 1);

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int test_pause (char *testid, MinItemParser * mip)
{
        int             retval = ENOERR;
        union sigval    val;
        int             time;
        DLListIterator  it = INITPTR;
        ScriptedTestProcessDetails *stpd = INITPTR;
        char            tmp[12];

        if (testid == INITPTR || strlen (testid) == 0) {
                errno = EINVAL;
                retval = -1;
                SCRIPTER_RTERR("test id missing");
                goto EXIT;
        }

        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findid, (void *)testid);

        if (it == DLListNULLIterator) {
                errno = ENODATA;
                retval = -1;
		SCRIPTER_RTERR_ARG ("Testid not found", testid);
                MIN_ERROR ("Testid not found on list");
                goto EXIT;
        }

        val.sival_int = 0;
        if (mip_get_next_tagged_int (mip, "time", &time) == ENOERR)
                val.sival_int = time;

        stpd = (ScriptedTestProcessDetails *) dl_list_data (it);

        if (stpd->status_ == TP_RUNNING) {
                sched_yield ();
                retval = sigqueue (stpd->pid_, SIGTSTP, val);
                if (retval == -1) {
                        sprintf (tmp, "%u", stpd->pid_);
                        SCRIPTER_RTERR_ARG ("Cannot pause", tmp);
                        MIN_WARN ("Cannot pause %d", stpd->pid_);
                }
                else {
                        stpd->status_ = TP_PAUSED;
                        MIN_INFO ("paused %d", stpd->pid_);
                }
        }

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int test_resume (char *testid)
{
        int             retval = ENOERR;
        DLListIterator  it = INITPTR;;
        ScriptedTestProcessDetails *stpd = INITPTR;
        char            tmp [12];

        if (testid == INITPTR || strlen (testid) == 0) {
                errno = EINVAL;
                retval = -1;
                SCRIPTER_RTERR ("Test id missing");
                goto EXIT;
        }

        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findid, (void *)testid);

        if (it == DLListNULLIterator) {
                errno = ENODATA;
                retval = -1;
		SCRIPTER_RTERR_ARG ("Testid not found", testid);
                MIN_ERROR ("Testid not found on list");
                goto EXIT;
        }

        stpd = (ScriptedTestProcessDetails *) dl_list_data (it);

        if (stpd->status_ == TP_PAUSED) {
                retval = kill (stpd->pid_, SIGCONT);
                if (retval == -1) {
                        sprintf (tmp, "%u", stpd->pid_);
                        SCRIPTER_RTERR_ARG ("Cannot resume", tmp);
                        MIN_WARN ("Cannot resume %d", stpd->pid_);
                }
                else {
                        stpd->status_ = TP_RUNNING;
                        MIN_INFO ("Resumed %d", stpd->pid_);
                }
        }

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int test_cancel (char *testid)
{
        int             retval = ENOERR;
        DLListIterator  it = INITPTR;;
        ScriptedTestProcessDetails *stpd = INITPTR;
        char            tmp [12];

        if (testid == INITPTR || strlen (testid) == 0) {
                errno = EINVAL;
                retval = -1;
                SCRIPTER_RTERR ("Test id missing");
                goto EXIT;
        }

        it = dl_list_find (dl_list_head (scripter_mod.tp_details)
                           , dl_list_tail (scripter_mod.tp_details)
                           , _findid, (void *)testid);

        if (it == DLListNULLIterator) {
                errno = ENODATA;
                retval = -1;
		SCRIPTER_RTERR_ARG ("Testid not found", testid);
                MIN_ERROR ("Testid not found on list");
                goto EXIT;
        }

        stpd = (ScriptedTestProcessDetails *) dl_list_data (it);

        retval = kill (stpd->pid_, SIGKILL);
        if (retval == -1) {
                sprintf (tmp, "%u", stpd->pid_);
                SCRIPTER_RTERR_ARG ("Cannot cancel", tmp);
                MIN_WARN ("Cannot cancel %d", stpd->pid_);
        }
        else {
                stpd->status_ = TP_ABORTED;
                update_variables (TP_NC);
                MIN_INFO ("cancelled %d", stpd->pid_);
        }

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int test_allocate_slave (const char *slave_type, const char *slave_name)
{
        int             retval = ENOERR;
        MsgBuffer       msg;
        msg.receiver_ = getppid ();
        msg.sender_ = getpid ();
        msg.type_ = MSG_EXTIF;
        msg.param_ = 0;
        msg.extif_msg_type_ = EAllocateSlave;

        if (slave_type == INITPTR) {
                retval = -1;
                errno = EINVAL;
                SCRIPTER_RTERR ("slave type missing");
                goto EXIT;
        }
        if (slave_name == INITPTR || strlen (slave_name) == 0) {
                retval = -1;
                errno = EINVAL;
                SCRIPTER_RTERR ("slave name missing");
                goto EXIT;
        }

	STRCPY (msg.desc_, slave_type, MaxDescSize);
	STRCPY (msg.message_, slave_name, MaxMsgSize);
	mq_send_message (scripter_mod.mqid, &msg);

	/* Stop execution till response comes */
	scripter_mod.extif_pending = ESTrue;
        gettimeofday (&scripter_mod.blocking_since, NULL);

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int test_free_slave (const char *slave_name)
{
        int             retval = ENOERR;
        MsgBuffer       msg;
        msg.receiver_ = getppid ();
        msg.sender_ = getpid ();
        msg.type_ = MSG_EXTIF;
        msg.param_ = 0;
        msg.extif_msg_type_ = EFreeSlave;

        if (slave_name == INITPTR || strlen (slave_name) == 0) {
                retval = -1;
                errno = EINVAL;
                SCRIPTER_RTERR ("slave_name missing");
                goto EXIT;
        }

        STRCPY (msg.message_, slave_name, MaxMsgSize);
        mq_send_message (scripter_mod.mqid, &msg);

        /* Stop execution till response comes */
        scripter_mod.extif_pending = ESTrue;
        gettimeofday (&scripter_mod.blocking_since, NULL);

      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int test_remote_exe (const char *slave_name, MinItemParser * mip)
{
        MsgBuffer       msg;
        int             retval = ENOERR;
        int             send_extif = 1;
        char           *kw = INITPTR;
        char           *token = INITPTR;
        char           *token2 = INITPTR;
        char           *p;

        if (slave_name == INITPTR || strlen (slave_name) == 0) {
                retval = -1;
                errno = EINVAL;
                SCRIPTER_RTERR ("slave name missing");
                goto EXIT;
        }

        if (mip == INITPTR) {
                retval = -1;
                errno = EINVAL;
                SCRIPTER_RTERR ("item parser argument missing");
                goto EXIT;
        }

        /* save in case we need to send */
        STRCPY (msg.message_, slave_name, MaxMsgSize);
        STRCPY (msg.desc_, mip->item_skip_and_mark_pos_, MaxDescSize);

        mip_get_next_string (mip, &kw);

        switch (get_keyword (kw)) {
        case EKeywordRequest:
                /*
                 ** Create a local "shadow" event for the remote
                 */
                mip_get_next_string (mip, &token);
                mip_get_next_string (mip, &token2);
                if (token2 != INITPTR && !strcmp (token2, "state"))
                        event_request (token, 1);
                else
                        event_request (token, 0);
                break;
        case EKeywordRelease:
                mip_get_next_string (mip, &token);
                event_release (token);
                break;
        case EKeywordWait:
                mip_get_next_string (mip, &token);
                send_extif = 0;
                event_wait (token);
                break;
        case EKeywordRun:
                /* Stop execution till response comes */
                scripter_mod.extif_pending = ESTrue;
		gettimeofday (&scripter_mod.blocking_since, NULL);
                break;
        case EKeywordExpect:
                scripter_mod.extif_pending = ESTrue;
		gettimeofday (&scripter_mod.blocking_since, NULL);
                mip_get_next_string (mip, &scripter_mod.expected_var);
                MIN_DEBUG ("master expecting variable %s",
                             scripter_mod.expected_var);
                break;
        case EKeywordSendreceive:
                mip_get_next_string (mip, &token);
                p = strchr (token, '=');
                *p = '\0';
                p++;
                snprintf (msg.desc_, MaxDescSize, "sendreceive %s=%s",
                          token, var_value(p));

                break;
        default:
                MIN_WARN ("Unknown keyword [%s]", kw);
                break;
        }

        if (send_extif) {
                msg.receiver_ = getppid ();
                msg.sender_ = getpid ();
                msg.type_ = MSG_EXTIF;
                msg.param_ = 0;
                msg.extif_msg_type_ = ERemoteSlave;
                mq_send_message (scripter_mod.mqid, &msg);
        }
      EXIT:
        return retval;
}

/* ------------------------------------------------------------------------- */
int test_canceliferror ()
{
        scripter_mod.canceliferror = ESTrue;
        return ENOERR;
}

/* ------------------------------------------------------------------------- */
int testclass_test_timeout (unsigned long interval)
{
        unsigned long   timeout = 0;
        int             retval = ENOERR;

        /* ualarm fails when timeout greater that 1000^2 useconds, we have to
         * use either alarm or ualarm.
         * alarm for timeouts >= 1 [sec] 
         * ualarm for timeouts < 1 [sec] 
         */
        if (interval > 999) {
                timeout = interval / (unsigned long)1000;
                retval = alarm (timeout);
                signal (SIGALRM, scripter_sigalrm_handler);
        } else {
                timeout = interval * (unsigned long)1000;
                retval = ualarm (timeout, 0);
                signal (SIGALRM, scripter_sigalrm_handler);
        }

        if (retval == -1) {
		SCRIPTER_RTERR_ARG ("Error in setting the timeout", 
                                    strerror (errno));
                MIN_ERROR ("Error in setting the timeout: [%s]",
                            strerror (errno));
        } else {
                MIN_DEBUG ("Starting timeout: [%d] miliseconds", interval);
                retval = ENOERR;
        }
        return retval;
}

/* ------------------------------------------------------------------------- */
int testclass_test_sleep (unsigned long interval)
{
        scripter_mod.sleep = interval;
        gettimeofday (&scripter_mod.time, NULL);
        MIN_DEBUG ("Starting sleep: [%d] miliseconds", interval);
        return ENOERR;
}
/* ------------------------------------------------------------------------- */
int  set_block_timeout (unsigned long timeout)
{
	scripter_mod.blocking_timeout = timeout;
	return ENOERR;
}

/* ------------------------------------------------------------------------- */
int declare_var (char *name, TSBool initialize, char *val)
{

        ScriptVar      *var;

        if ((var = var_find (name)) != INITPTR) {
                if (initialize == ESFalse) {
                        SCRIPTER_RTERR_ARG ("Variable already declared", name);
                        return -1;
                }
                var_assign (name, val);
                return ENOERR;
        }
        if (var_find (name) != INITPTR) {
                MIN_WARN ("variable %s already declared", name);
                return -1;
        }
        var = NEW (ScriptVar);
        var->name_ = name;
        var->initialized_ = initialize;
        if (initialize)
                var->value_ = val;
        else
                var->value_ = INITPTR;
        dl_list_add (variables, var);
        MIN_DEBUG ("name=%s init. val=%s", name, initialize ? val : "none");
        return ENOERR;
}

/* ------------------------------------------------------------------------- */
LOCAL int declare_internal_var (const char *name)
{
        char *var_name, *var_value;
        var_value = NEW2(char, 32);
        var_name = NEW2(char, strlen (name) + 1);

        sprintf (var_name, "%s", name);
        sprintf (var_value, "%d", 0);
        declare_var (var_name, 1, var_value);
        return ENOERR;
}

/* ------------------------------------------------------------------------- */
int sendreceive_slave_send (char *variable, char *value)
{
        MsgBuffer       msg;

        MIN_DEBUG ("sendreceive var=%s value=%s", variable, value);
        msg.receiver_ = getppid ();
        msg.sender_ = getpid ();
        msg.type_ = MSG_SNDRCV;
        STRCPY (msg.message_, variable, MaxMsgSize);
        STRCPY (msg.desc_, var_value (value), MaxDescSize);
        mq_send_message (scripter_mod.mqid, &msg);

        return ENOERR;
}

/* ------------------------------------------------------------------------- */
int sendreceive_slave_expect (char *variable)
{
        MsgBuffer       msg;
        msg.receiver_ = getppid ();
        msg.sender_ = getpid ();
        msg.type_ = MSG_EXTIF;
        msg.extif_msg_type_ = ERemoteSlave;

        if (strlen (variable) >= MaxDescSize - strlen ("expect ")) {
                SCRIPTER_RTERR ("too long variable name for expect!");
                MIN_ERROR ("Too long variable name!");
                return -1;
        }

        scripter_mod.expected_var = variable;
        scripter_mod.extif_pending = ESTrue;
        gettimeofday (&scripter_mod.blocking_since, NULL);

        STRCPY (msg.message_, "master", MaxMsgSize);
        sprintf (msg.desc_, "expect %s", variable);
        mq_send_message (scripter_mod.mqid, &msg);

        return ENOERR;
}
/* ------------------------------------------------------------------------- */
int test_interference(MinItemParser* args)
{
        char*   command = INITPTR;
        char*   name = INITPTR;
        char*   typename = INITPTR;
        int     value = 0;
        int     idle_time = 0;
        int     busy_time = 0;
        TInterferenceType       type = ECpuLoad;
        InterfHandle*           handle = INITPTR;
        DLListIterator          work_inter;
        
        mip_get_next_string(args,&name);
        mip_get_next_string(args,&command);

        if (strcmp(command,"start") == 0) {
                if (interference_handles == INITPTR)
                          interference_handles = dl_list_create();
                mip_get_next_string(args,&typename);
                mip_get_next_int(args,&value);
                mip_get_next_int(args,&idle_time);
                mip_get_next_int(args,&busy_time);

                if (strcmp(typename,"cpuload") == 0) type = ECpuLoad;
                else if (strcmp(typename,"memload") == 0) type = EMemLoad;
                else if (strcmp(typename,"ioload") == 0) type = EIOLoad;
                /*validation was successful so there is no point in 
                handling other cases here*/

                handle = NEW(InterfHandle);
                sprintf(handle->name_,"%s",name);
                handle->instance_ = ti_start_interference_timed (type, 
                                                                 idle_time,
                                                                 busy_time,
                                                                 value);
                dl_list_add(interference_handles,(void*)handle);
        }
        /*if validation was successful (and we asume so) 
          then command is "stop"*/
        else {
                work_inter = dl_list_head(interference_handles);
                while (work_inter != DLListNULLIterator){
                        handle = (InterfHandle*)dl_list_data(work_inter);
                        if (strcmp(handle->name_,name) == 0) break;
                        work_inter = dl_list_next(work_inter);
                }
                ti_stop_interference(handle->instance_);
                dl_list_remove_it(work_inter);
                DELETE(handle);
        }
        return 0;
}

/* ------------------------------------------------------------------------- */

TSBool eval_if (char *condition) {
        int i,isnumber = ESTrue;
        const char *value;

        MIN_DEBUG ("condition = %s", condition);

        value = var_value (condition);
        for (i = 0; i < strlen (value); i++) {
                if (!isxdigit(value[i])) {
                        isnumber = ESFalse;
                        break;
                }
        }
        if (isnumber == ESTrue && strtol (value, NULL, 16) == 0) {
                return ESFalse;
        }
        if (isnumber == ESFalse && strcasecmp (value, "false") == 0) {
                return ESFalse;
        }

        return ESTrue;
}
                


/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST
#include "min_scripter_if.tests"
#endif                          /* MIN_UNIT_TEST */
/* End of file */
