/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 *  @file       scripted_test_process.c
 *  @version    0.1
 *  @brief      This file contains implementation of Scripted Test Process.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "scripted_test_process.h"

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* ------------------------------------------------------------------------- */
extern TMC_t   *ptmc;
extern DLList  *tp_handlers;
extern TSBool   stprun;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

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
LOCAL MsgBuffer       result;
LOCAL sig_atomic_t    fatal_sig_handled = 0;

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL int       _look4class (const void *a, const void *b);
/* ------------------------------------------------------------------------- */
LOCAL void      stp_handle_run (const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Used for looking for specified class name on the list of them. 
 */
LOCAL int _look4class (const void *a, const void *b)
{
        TestClassDetails *tcd = (TestClassDetails *) a;
        MIN_DEBUG ("%s classname %s", __FUNCTION__, tcd->classname_);
        return strcmp (tcd->classname_, (char *)b);
}
/* ------------------------------------------------------------------------- */
/** Handles MSG_RUN command.
 *  @param msg [in] message that comes through IPC.
 *
 *  This function runs a test function provided by the end user. The return
 *  value of that function is picked as a test result and is send through
 *  IPC in MSG_RET message.
 */
LOCAL void stp_handle_run (const MsgBuffer * msg)
{
        DLListIterator  it = DLListNULLIterator;
        TestClassDetails *tcd = INITPTR;
        MinItemParser  mip;
        int             ret = 0;
        int             mq = mq_open_queue ('a');

        if (mq < 0) {
                MIN_WARN ("mq_open_queue() FAILED");
                goto EXIT;
        }

        /* 1) look-up for the class on the list */
        it = dl_list_find (dl_list_head (tp_handlers)
                           , dl_list_tail (tp_handlers)
                           , _look4class, msg->message_);

        if (it == DLListNULLIterator) {
                MIN_ERROR("No class [%s] on list.List has %d elements",
                           msg->message_, dl_list_size (tp_handlers));
                goto EXIT;
        }

        /* 2) extract data from found item */
        tcd = (TestClassDetails *) dl_list_data (it);

        /* 3) extract data from IPC communicate */
        mip.item_line_section_ = NEW2 (char, strlen (msg->desc_) + 1);
        STRCPY (mip.item_line_section_, msg->desc_, strlen (msg->desc_) + 1);
        mip.item_skip_and_mark_pos_ = &mip.item_line_section_[0];
        mip.parsing_type_ = ENormalParsing;
        mip.get_methods_indicator_ = ESTrue;
        if (mip.item_line_section_ == INITPTR) {
                MIN_WARN ("MinItemParser is INITPTR");
                goto EXIT;
        }

        result.receiver_ = getppid ();
        result.sender_ = getpid ();
        result.type_ = MSG_RET;
        STRCPY (result.desc_, msg->desc_, MaxDescSize);
        STRCPY (result.message_, msg->desc_, MaxMsgSize);

        /* 4) call test function */
        ret = tcd->runtc_ (&mip);
	if (ret != ENOERR) {
		snprintf (result.message_, MaxMsgSize, "%s (retval %d)",
			  result.desc_, ret);
		result.desc_[MaxMsgSize - 1] = '\0';
		result.param_ = TP_FAILED;
	}
	else
		result.param_ = TP_PASSED;
		
        /* 5) when the test is finished we have to do something... */
        /*    sending result back via IPC looks like a good idea.  */
        result.special_ = 0;
        mq_send_message (mq, &result);
        DELETE (mip.item_line_section_);
      EXIT:
        return;
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** SIGUSR2 handler.
 *  @param signum not used. 
 */
void stp_handle_sigusr2 (int signum)
{
        stprun = ESFalse;
}
/* ------------------------------------------------------------------------- */
/** SIGSEGV handler 
 *  @param signum not used.
 */
void stp_handle_sigsegv (int signum)
{

        int             mq = mq_open_queue ('a');
        stprun = ESFalse;
	signal (SIGSEGV, SIG_DFL);

	if (fatal_sig_handled) {
		raise (SIGSEGV);
		return;
	}

        if (mq < 0) {
                MIN_WARN ("mq_open_queue() FAILED");
		return;
        }
        result.param_ = TP_CRASHED;
        result.special_ = 0;
        mq_send_message (mq, &result);
	fatal_sig_handled = 1;

	raise (SIGSEGV);
}
/* ------------------------------------------------------------------------- */
/** SIGABORT handler 
 *  @param signum not used.
 */
void stp_handle_sigabort (int signum)
{

        int             mq = mq_open_queue ('a');
        stprun = ESFalse;
	signal (SIGABRT, SIG_DFL);

	if (fatal_sig_handled) {
		raise (SIGABRT);
		return;
	}

        if (mq < 0) {
                MIN_WARN ("mq_open_queue() FAILED");
		return;
        }
        result.param_ = TP_CRASHED;
        result.special_ = 0;
        mq_send_message (mq, &result);
	fatal_sig_handled = 1;

	raise (SIGABRT);
}
/* ------------------------------------------------------------------------- */
/** Handles IPC message in the Scripted Test Process way. 
 *  @param msg [in] message to be handled.
 */
void stp_handle_message (const MsgBuffer * msg)
{
        switch (msg->type_) {
        case MSG_RUN:
                MIN_DEBUG ("Recieved message MSG_RUN from: %d, with"
                             " param: %d [%s] [%s]",
                             msg->sender_, msg->param_, msg->desc_,
                             msg->message_);
                stp_handle_run (msg);
                break;
        default:
                MIN_WARN ("Unhandled message from %d: to %d, type: %d",
                           msg->sender_, msg->receiver_, msg->type_);
        }
        return;
}

/* ------------------------------------------------------------------------- */
/** Called before exiting scripted test process 
 */
void stp_exit ()
{
        DLListIterator  it = DLListNULLIterator;
        TestClassDetails *tcd = INITPTR;
        
        /* 1) We need to clean the list with pointers to run function. */
        it = dl_list_tail (tp_handlers);

        while (it != DLListNULLIterator) {
                tcd = (TestClassDetails *) dl_list_data (it);
                if (tcd != INITPTR) {
                        dlclose (tcd->dllhandle_);
                        DELETE (tcd->classname_);
                        DELETE (tcd);
                        it->data_ = INITPTR;
                } else {
                        MIN_ERROR ("Unable to obtain TCD");
                }
                dl_list_remove_it (it);
                it = dl_list_tail (tp_handlers);
        }

        /* resend msgs from buffer, flush the mq buffer. */
        mq_resend_buffered ();
        mq_flush_msg_buffer ();

        MIN_TRACE ("Exiting");
        /* 2) At the end exit gracefully. */
        exit (TP_EXIT_SUCCESS);
        return;
}
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST

#endif                          /* MIN_UNIT_TEST */
/* End of file */
