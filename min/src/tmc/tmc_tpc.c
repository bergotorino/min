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
 *  @file       tmc_tpc.c
 *  @version    0.1
 *  @brief      This file contains implementation of the TP supervising part
 *              of TMC.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "tmc.h"
#include "tmc_tpc.h"
#include <sched.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/** pointer to global TMC structure. Needed in signal handlers */
extern TMC_t   *ptmc;
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
LOCAL void      tp_timeout_handler (int signum);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Handles test timeout.
 *  @param signum [in] number of the signal the function is handling (SIGALRM)
 */
LOCAL void tp_timeout_handler (int signum)
{
        if (ptmc->tpc_.tp_status_ == TP_RUNNING) {

                if (--(ptmc->tpc_.tp_timeout_) == 0) {
                        ptmc->tpc_.tp_status_ = TP_TIMEOUT;
                        kill (ptmc->tpc_.tp_pid_, SIGKILL);
                        return;
                }
        }
        ualarm (100, 0);
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Initializes Test Process Controller
 *  @param tpc [in:out] address of the test proces controller.
 */
void tp_init (TestProcessController * tpc)
{
        tpc->tp_pid_ = 0;
        tpc->tp_status_ = TP_NONE;
        tpc->tp_timeout_ = 0;
}

/* ------------------------------------------------------------------------- */
/** Pauses an ongoing test process.
 *  @param tpc [in:out] adress of the test proces controller.
 */
void tp_pause (TestProcessController * tpc)
{
        int             retval = 0;
        if (tpc->tp_status_ == TP_RUNNING) {
                retval = kill (tpc->tp_pid_, SIGSTOP);
                if (retval == -1)
                        MIN_ERROR ("Pausing Test Proces failed");
                else {
                        tpc->tp_status_ = TP_PAUSED;
                        MIN_INFO ("Test Process paused");
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Resumes paused test process.
 *  @param tpc [in:out] adress of the test proces controller.
 */
void tp_resume (TestProcessController * tpc)
{
        int             retval = 0;
        if (tpc->tp_status_ == TP_PAUSED) {
                retval = kill (tpc->tp_pid_, SIGCONT);
                if (retval == -1)
                        MIN_ERROR ("Resuming Test Proces failed");
                else {
                        tpc->tp_status_ = TP_RUNNING;
                        MIN_INFO ("Test Process resumed");
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Aborts test process.
 *  @param tpc [in:out] adress of the test proces controller.
 */
void tp_abort (TestProcessController * tpc)
{
        int             retval = 1;
        if (tpc->tp_pid_ != 0) {
                tpc->tp_status_ = TP_ABORTED;
                retval = kill (tpc->tp_pid_, SIGKILL);
                if (retval == -1)
                        MIN_ERROR ("Aborting Test Proces failed");
                else {
                        tpc->tp_status_ = TP_ABORTED;
                        MIN_INFO ("Test Process aborted");
                }
        }
}

/* ------------------------------------------------------------------------- */
/** Sets timeout for test process.
 *  @param tpc [in:out] adress of the test proces controller.
 *  @param t [in] timeout value in seconds.
 */
void tp_set_timeout (TestProcessController * tpc, unsigned int t)
{
        tpc->tp_timeout_ = t;
}

/* ------------------------------------------------------------------------- */
/** Gets timeout for test process.
 *  @param tpc adress of the test proces controller.
 *  @return timeout value for the test process.
 */
unsigned int tp_timeout (const TestProcessController * tpc)
{
        return tpc->tp_timeout_;
}

/* ------------------------------------------------------------------------- */
/** Sets status of a test process.
 *  @param tpc [in:out] adress of the test proces controller.
 *  @param s [in] new status
 */
void tp_set_status (TestProcessController * tpc, MINTPStatus s)
{
        tpc->tp_status_ = s;
}

/* ------------------------------------------------------------------------- */
/** Gets status of a test process.
 *  @param tpc [in] adress of the test proces controller.
 *  @return the current status of the test process.
 */
MINTPStatus tp_status (const TestProcessController * tpc)
{
        return tpc->tp_status_;
}

/* ------------------------------------------------------------------------- */
/** Setter for the tp_pid_ field of TestProcessController
 *  @param tpc [in:out] adress of the test proces controller.
 *  @param pid [in] the pid.
 */
void tp_set_pid (TestProcessController * tpc, pid_t pid)
{
        tpc->tp_pid_ = pid;
}

/* ------------------------------------------------------------------------- */
/** Getter for the tp_pid_ field of TestProcessController
 *  @param tpc [in] adress of the test proces controller.
 *  @return pid of the test process.
 */
pid_t tp_pid (const TestProcessController * tpc)
{
        return tpc->tp_pid_;
}

/* ------------------------------------------------------------------------- */
/** Setter for the tp_pid_ field of TestProcessController
 */
void tp_set_timeout_handler ()
{
        sl_set_sighandler (SIGALRM, tp_timeout_handler);
}

/* ------------------------------------------------------------------------- */
/** Starts timeouting the test process if timeout is set.
 *  @param tpc [in] adress of the test proces controller.
 */
void tp_start_timeout (const TestProcessController * tpc)
{
        if (tpc->tp_timeout_ > 0)
                ualarm (100, 0);
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
