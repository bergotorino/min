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
 *  @file       combined_test_process.c
 *  @version    0.1
 *  @brief      This file contains implementation of Combined Test Process.
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "combined_test_process.h"

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* ------------------------------------------------------------------------- */
extern TMC_t   *ptmc;
extern DLList  *tp_handlers;
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
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
void ctp_handle_sigusr2 (int signum)
{
        /* resend bufgfered, flush message buffer */
        mq_resend_buffered ();
        mq_flush_msg_buffer ();

        /* At the end exit gracefully. */
        exit (TP_EXIT_SUCCESS);
        return;
}


void ctp_hande_sigtstp (int signo, siginfo_t * info, void *context)
{
        int             sleeptime = info->si_int;
        int             secs;
        long            usecs;

        MIN_DEBUG ("SIGTSTP caught: sleep time %d",
                    sleeptime);
        if (sleeptime) {
                secs = sleeptime / 1000;
                usecs = (sleeptime * 1000) / 1000000;
                MIN_DEBUG ("Pausing for %d secs and %d usecs",
                            secs, usecs);

                if (secs)
                        sleep (secs);
                usleep (usecs);
        } else {
                kill (getpid (), SIGSTOP);
        }

}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
#ifdef MIN_UNIT_TEST

#endif                          /* MIN_UNIT_TEST */
/* End of file */
