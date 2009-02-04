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
 *  @file       tmc_msghnd.c
 *  @version    0.1
 *  @brief      This file contains implementation of the TMC message handling.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <tmc.h>
#include <tmc_msghnd.h>
#include <min_common.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/** pointer to global TMC structure. Needed in signal handlers */
extern TMC_t   *ptmc;
extern TestCaseResult  globaltcr;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
TSBool tp_exit_flag = ESFalse;
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
/** MSG_GTC handler
 *  @param tmc adress of the TMC entity.
 *
 *  Sends MSG_TCD messages to the engine.
 */
void            gu_handle_gtc (TMC_t * tmc, TSBool send);
/* ------------------------------------------------------------------------- */
/** MSG_EXE handler
 *  @param tmc adress of the TMC entity.
 *  @param id test case id
 *  @param cfg_file name of the configuration file.
 *
 *  Executes a test case of a given id.
 */
LOCAL void      gu_handle_exe (TMC_t * tmc, int id, const char *cfg_file);
/* ------------------------------------------------------------------------- */
/** MSG_RET handler
 *  @param tmc adress of the TMC entity.
 *  @param result test case result
 *  @param desc description of a test case result
 *
 *  It forwards test case result to the engine via MQ.
 */
LOCAL void      gu_handle_ret (TMC_t * tmc, int result, const char *desc);
/* ------------------------------------------------------------------------- */
/** MSG_END handler
 *  @param tmc adress of the TMC entity.
 *
 *  Exits from the main event loop.
 */
LOCAL void      gu_handle_end (TMC_t * tmc);
/* ------------------------------------------------------------------------- */
/** MSG_PAUSE handler
 *  @param tmc adress of the TMC entity.
 *
 *  Pauses Test Process - if any.
 */
LOCAL void      gu_handle_pause (TMC_t * tmc);
/* ------------------------------------------------------------------------- */
/** MSG_RESUME handler
 *  @param tmc adress of the TMC entity.
 *
 *  Resumes Test Process - if paused.
 */
LOCAL void      gu_handle_resume (TMC_t * tmc);
/* ------------------------------------------------------------------------- */
/** MSG_STOP handler
 *  @param tmc adress of the TMC entity.
 *
 *  Abort Test Process - if any.
 */
LOCAL void      gu_handle_stop (TMC_t * tmc);
/* ------------------------------------------------------------------------- */
/** MSG_USR handler
 *  @param tmc adress of the TMC entity.
 *  @param msg recieved message from test process which is forwarded to engine.
 *
 *  Forwards message to the engine.
 */
void            gu_handle_usr (TMC_t * tmc, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** MSG_EVENT handler
 *  @param tmc adress of the TMC entity.
 *
 *  Forwards message to the engine.
 */
LOCAL void      gu_handle_event (TMC_t * tmc, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** SIGCHLD handler
 *  @param sig number of the signal
 *
 *  Handles Test Process termination event. Called async. by the system.
 */
void            gu_handle_sigchld (int sig);
/* ------------------------------------------------------------------------- */
/** SIGSEGV handler
 *  @param sig number of the signal
 *
 *  Handles Test Process crashing event. Called async. by the system.
 */
void            gu_handle_sigsegv (int sig);
/* ------------------------------------------------------------------------- */
/** SIGUSR2 handler
 *  @param sig number of the signal
 *
 *  Handles SIGUSR2 sent by the TMC. The SIGUSR2 is used instead of SIGKILL
 *  for killing the Test Process, because SIGKILL is not always received
 *  by the TP (FFS).
 */
void            gu_handle_sigusr2 (int sig);
/* ------------------------------------------------------------------------- */
/** Creates a test process
 *  @param tmc adress of the test module controller
 *  @param id id of the test case to be executed
 *  @param cfg_file config file
 */
LOCAL void      gu_create_tp (TMC_t * tmc, int id, const char *cfg_file);
/* ------------------------------------------------------------------------- */
/** MSG_EXT handler 
 *  @param tmc adress of the test module controller
 *  @param msg [in] message that came through IPC 
 */
LOCAL void      gu_handle_extif (TMC_t * tmc, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** MSG_SNDRCV handler 
 *  @param tmc adress of the test module controller
 *  @param msg [in] message that came through IPC 
 */
LOCAL void      gu_handle_sndrcv (TMC_t * tmc, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
/** Dummy sigchld handler*/
void            dummy_handler (int sig);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL void gu_handle_exe (TMC_t * tmc, int id, const char *cfg_file)
{
        long            tmp = -1;
        /*if( id == 2 ) */ tp_set_timeout (&tmc->tpc_, 0);
        tp_set_timeout_handler ();
        tmp = fork ();
        if (tmp == 0) {
                min_log_open ("TestProcess", 3);
                sl_set_sighandler (SIGCHLD, dummy_handler);
                usleep (50000);
                gu_create_tp (tmc, id, cfg_file);
        } else if (tmp > 0) {
                MIN_INFO ("Test Process created [%d]", tmp);
                tp_set_pid (&tmc->tpc_, tmp);
                tp_set_status (&tmc->tpc_, TP_RUNNING);
                tp_start_timeout (&tmc->tpc_);
        } else {
                MIN_ERROR ("Test Process not created");
        }
}

/* ------------------------------------------------------------------------- */
LOCAL void gu_handle_ret (TMC_t * tmc, int result, const char *desc)
{
        /*tp_set_status( &tmc->tpc_, TP_ENDED ); */

        ip_send_ret (&tmc->tmcipi_, result, desc);

        /*retval = kill( tp_pid( &tmc->tpc_ ), SIGKILL );
           if( retval == -1 ) min_warn("RET: Cannot kill child process"); */
}

/* ------------------------------------------------------------------------- */
LOCAL void gu_handle_end (TMC_t * tmc)
{
        if (tp_status (&tmc->tpc_) == TP_ENDED) {
                tp_set_status (&tmc->tpc_, TP_NONE);
        } else if (tp_status (&tmc->tpc_) != TP_NONE) {
                tp_abort (&tmc->tpc_);
                tp_set_status (&tmc->tpc_, TP_NONE);
        }
        while (!(tp_status (&tmc->tpc_) == TP_NONE)) {
                usleep (50000);
        }
        tmc->run_ = 0;
}

/* ------------------------------------------------------------------------- */
LOCAL void gu_handle_pause (TMC_t * tmc)
{
        tp_pause (&tmc->tpc_);
}

/* ------------------------------------------------------------------------- */
LOCAL void gu_handle_resume (TMC_t * tmc)
{
        tp_resume (&tmc->tpc_);
}

/* ------------------------------------------------------------------------- */
LOCAL void gu_handle_stop (TMC_t * tmc)
{
        tp_abort (&tmc->tpc_);
}

/* ------------------------------------------------------------------------- */
LOCAL void gu_handle_event (TMC_t * tmc, const MsgBuffer * msg)
{
        int             retval = -1;
        MsgBuffer      *tmp = (MsgBuffer *) msg;
        tmp->receiver_ = getppid ();
        tmp->sender_ = getpid ();
        retval = mq_send_message (tmc->tmcipi_.mqid_, tmp);
        if (retval == -1)
                MIN_WARN ("EVENT: Cannot send message");
}

/* ------------------------------------------------------------------------- */
LOCAL void gu_handle_extif (TMC_t * tmc, const MsgBuffer * msg)
{
        MsgBuffer send_msg;

        if (msg == INITPTR) {
                return;
        }
        memcpy (&send_msg,msg, sizeof (MsgBuffer));

        send_msg.sender_ = getpid ();

        /* Action depends on type of external controller command */
        switch (msg->extif_msg_type_) {
        case EAllocateSlave:
        case EFreeSlave:
        case ERemoteSlave:
                MIN_DEBUG ("EXTIF command: Scripter --> Engine: [%d]",
                             msg->extif_msg_type_);
                send_msg.receiver_ = getppid ();
                mq_send_message (tmc->tmcipi_.mqid_, &send_msg);
                break;
        case EResponseSlave:
        case ERemoteSlaveResponse:
                MIN_DEBUG ("EXTIF command: Engine --> Scripter: [%d]",
                             msg->extif_msg_type_);
                send_msg.receiver_ = tmc->tpc_.tp_pid_;
                mq_send_message (tmc->tmcipi_.mqid_, &send_msg);
                break;
        default:
                MIN_WARN ("Unknown type of EXTIF command: [%d]",
                             msg->extif_msg_type_);
        }
}

/* ------------------------------------------------------------------------- */
LOCAL void gu_handle_sndrcv (TMC_t * tmc, const MsgBuffer * msg)
{
        int             retval = -1;
        MsgBuffer      *tmp = (MsgBuffer *) msg;
        if (msg->sender_ == tmc->tpc_.tp_pid_)
                tmp->receiver_ = getppid ();
        else
                tmp->receiver_ = tmc->tpc_.tp_pid_;
        tmp->sender_ = getpid ();
        retval = mq_send_message (tmc->tmcipi_.mqid_, tmp);
        if (retval == -1)
                MIN_WARN ("SNDRCV: Cannot send message");
}

/* ------------------------------------------------------------------------- */
void gu_handle_sigchld (int sig)
{
        int             status;
        int             reason = TP_NONE;
        int             pid;
        while ((pid = waitpid (-1, &status, 0)) > 0) {
                tp_set_pid (&ptmc->tpc_, 0);
                reason = tp_status (&ptmc->tpc_);
                tp_set_status (&ptmc->tpc_, TP_NONE);
        }
        if (reason == TP_TIMEOUT) {
                globaltcr.result_ = TP_TIMEOUTED;
                strcpy (globaltcr.desc_, "Timeouted");
        } else if (reason == TP_ABORTED) {
                globaltcr.result_ = TP_NC;
                strcpy (globaltcr.desc_, "Aborted");
        } 
        ptmc->send_ret_ = ESTrue;

        return;
}

/* ------------------------------------------------------------------------- */
void dummy_handler (int sig)
{
        ;
}

/* ------------------------------------------------------------------------- */
void gu_handle_sigsegv (int sig)
{
        ip_send_ret (&ptmc->tmcipi_, TP_CRASHED, "Crashed");
        sleep (1);
        exit (TP_EXIT_FAILURE);
        return;
}

/* ------------------------------------------------------------------------- */
void gu_handle_sigusr2 (int sig)
{
        tp_exit_flag = ESTrue;
        return;
}

/* ------------------------------------------------------------------------- */
LOCAL void gu_create_tp (TMC_t * tmc, int id, const char *cfg_file)
{
        TestCaseResult  tcr;
        ip_init (&tmc->tmcipi_, getppid ());

        sl_set_sighandler (SIGSEGV, gu_handle_sigsegv);
        sl_set_sighandler (SIGUSR2, gu_handle_sigusr2);
        tmc->lib_loader_.run_case_fun_ (id, cfg_file, &tcr);

        ip_send_ret (&tmc->tmcipi_, tcr.result_, tcr.desc_);
        mq_resend_buffered ();
        while (tp_exit_flag == ESFalse) {
                usleep (50000);
        }
        mq_flush_msg_buffer ();
        exit (TP_EXIT_SUCCESS);
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */

/** Message queue listener.
 *  @param tmc adress of the TMC entity.
 *  @param input_buffer output parameter for the recieved message.
 */
void gu_read_message (TMC_t * tmc, MsgBuffer * input_buffer)
{
        int             retval = 0;
        TSBool          reported = ESFalse;

        while (1) {
                retval = mq_read_message (tmc->tmcipi_.mqid_, getpid ()
                                          , input_buffer);
                if (retval == -1) {
                        switch (errno) {
                        case EINTR:
                                MIN_DEBUG
                                    ("Reading MQ interrupted by signal");
                                break;

                        case EIDRM:
                                if (reported == ESFalse) {
                                        reported = ESTrue;
                                        MIN_ERROR ("MQ id removed");
                                }
                                break;

                        case E2BIG:
                                if (reported == ESFalse) {
                                        reported = ESTrue;
                                        MIN_DEBUG ("Recieved message "
                                                     "too big");
                                }
                                break;

                        case EINVAL:
                                if (reported == ESFalse) {
                                        reported = ESTrue;
                                        MIN_WARN ("Invalid value: "
                                                   "mq_read_message");
                                }
                                break;
                        }
                } else if (retval < 8) {
                        /*ignore too short messages */
                        MIN_DEBUG ("Recieved message is too small");
                } else {
                        break;
                }
        }
        return 1;
}

/* ------------------------------------------------------------------------- */
/** Main message handler
 *  @param tmc adress of the TMC entity.
 *  @param msg recieved message.
 *
 *  It calls appropiate handler for each type of message.
 */
void gu_handle_message (TMC_t * tmc, const MsgBuffer * msg)
{
        int             retval;
        switch (msg->type_) {
        case MSG_GTC:
                gu_handle_gtc (tmc, msg->special_);
                break;

        case MSG_EXE:
                gu_handle_exe (tmc, msg->param_, msg->message_);
                break;

        case MSG_RET:
                /*gu_handle_ret(tmc, msg->param_,msg->message_); */
                globaltcr.result_ = msg->param_;
                STRCPY (globaltcr.desc_, msg->message_,
                        MaxTestResultDescription + 1);

                retval = kill (tp_pid (&tmc->tpc_), SIGUSR2);
                if (retval == -1)
                        MIN_ERROR ("RET: Cannot kill child process");
                else
                        MIN_DEBUG ("RET: Killed child process");
                break;

        case MSG_END:
                gu_handle_end (tmc);
                break;

        case MSG_PAUSE:
                gu_handle_pause (tmc);
                break;

        case MSG_RESUME:
                gu_handle_resume (tmc);
                break;

        case MSG_STOP:
                gu_handle_stop (tmc);
                break;

        case MSG_USR:
                gu_handle_usr (tmc, msg);
                break;

        case MSG_EVENT:
                gu_handle_event (tmc, msg);
                break;

        case MSG_EXTIF:
                gu_handle_extif (tmc, msg);
                break;

        case MSG_SNDRCV:
                gu_handle_sndrcv (tmc, msg);
                break;

        default:
                MIN_WARN ("Unhandled message from %d: to %d, type: %d",
                             msg->sender_, msg->receiver_, msg->type_);
        }
}

/* ------------------------------------------------------------------------- */
void gu_handle_gtc (TMC_t * tmc, TSBool send)
{
        DLList         *list = INITPTR;
        DLListIterator  cfgit = DLListNULLIterator;
        DLListIterator  it = DLListNULLIterator;
        char           *cfgfile = "";

        cfgit = dl_list_head (tmc->config_list_);
        while (cfgit != DLListNULLIterator) {
                cfgfile = (char *)dl_list_data (cfgit);
                list = dl_list_create ();
                if (list != INITPTR) {
                        tmc->lib_loader_.get_cases_fun_ (cfgfile, &list);
                        it = dl_list_head (list);
                        if (send == ESTrue) {
                                ip_send_tcd (&tmc->tmcipi_, cfgfile, it);
                        }
                        dl_list_foreach (dl_list_head (list),
                                         dl_list_tail (list), free);
                        dl_list_free (&list);
                }
                cfgit = dl_list_next (cfgit);
        }

        ip_send_eotcd (&tmc->tmcipi_);
}

/* ------------------------------------------------------------------------- */
void gu_handle_usr (TMC_t * tmc, const MsgBuffer * msg)
{
        int             retval = -1;
        MsgBuffer      *tmp = (MsgBuffer *) msg;
        tmp->receiver_ = getppid ();
        tmp->sender_ = getpid ();
        retval = mq_send_message (tmc->tmcipi_.mqid_, tmp);
        if (retval == -1)
                MIN_WARN ("USR: Cannot send message");
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
