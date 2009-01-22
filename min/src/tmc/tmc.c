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
 *  @file       tmc.c
 *  @version    0.1
 *  @brief      This file contains implementation of the Test Module Controller
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "tmc.h"
#include "tmc_msghnd.h"

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
extern void     gu_handle_sigchld (int);
extern void     gu_handle_sigalrm (int);
/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ==================== LOCAL FUNCTIONS ==================================== */

/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Initializes the TMC_t structure and does the init phase of the TMC
 *  @param tmc adress of the TMC_t structure to be initialized
 *  @param argc number of arguments
 *  @param argv[] address of the table of Test Module COntroller's arguments
 *
 *  This function does initialization phase of the Test Module Controller:
 *  - opens log
 *  - parses program arguments
 *  - initializes message queue interface
 *  - initializes test library loader
 *  - sends status message
 */
void gu_init_tmc (TMC_t * tmc, int argc, char *argv[])
{
        int             i;
        char           *tmp = "";
        memset (tmc, 0x00, sizeof (*tmc));
        /* 1. open log */

        min_log_open ("TMC", 3);

        MIN_INFO ("TMC: Build date [%s %s]",__DATE__,__TIME__);
        MIN_DEBUG ("Process started; param: %s",
                     argc > 1 ? argv[1] : "NULL");
        MIN_DEBUG ("-- Test module: %s", argc > 1 ? argv[1] : "NULL");
        for (i = 2; i < argc; i++) {
                MIN_DEBUG ("-- Config file: %s", argv[i]);
        }

        /* Fill list of config files */
        tmc->config_list_ = dl_list_create ();
        if (argc > 2) {
                for (i = 2; i < argc; ++i) {
                        dl_list_add (tmc->config_list_, (void *)argv[i]);
                }
        }
        if (dl_list_size (tmc->config_list_) == 0) {
                dl_list_add (tmc->config_list_, (void *)&tmp);
        }

        /* set handler */
        sl_set_sighandler (SIGCHLD, gu_handle_sigchld);

        tp_init (&tmc->tpc_);
        ip_init (&tmc->tmcipi_, getppid ());

        /* 2. open MQ */
        if (ip_status (&tmc->tmcipi_) == -1) {
                MIN_FATAL ("Unable to open Message Queue: %s",
                          strerror (errno));
                exit (-3);
        }

        /* 3. load library and resolve functions */

        if (argc <= 0) {
                MIN_ERROR ("Incorrect number of arguments %d",argc);
                goto NOT_OK;
        }

        if (tl_open (&tmc->lib_loader_, argv[1])) {
                MIN_ERROR ("Library could not be loaded %s",argv[1]);
                goto NOT_OK;
        }

        /* 4. send status message */
        ip_send_ok (&tmc->tmcipi_);
        tmc->run_ = 1;
        return;
      NOT_OK:
        ip_send_ko (&tmc->tmcipi_, 0, "\0");
        return;
}

/* ------------------------------------------------------------------------- */
/** Main loop of the Test Module Controller
 *  @param tmc adress of the TMC_t structure.
 *
 *  It does:
 *  - reads message that comes from message queue
 *  - handles recieved message
 */
void gu_run_tmc (TMC_t * tmc)
{
        MsgBuffer       input_buffer;

        while (tmc->run_ == 1) {
                gu_read_message (tmc, &input_buffer);
                gu_handle_message (tmc, &input_buffer);
        }
}

/* ------------------------------------------------------------------------- */
/** Deinitialization of the Test Module Controller
 *  @param tmc adress of the TMC_t structure.
 *
 *  It does:
 *  - frees the list of config files
 *  - closes the test library loader
 *  - closes log
 *
 *  NOTE: Test Module Controller does not close the message queue.
 */
void gu_done_tmc (TMC_t * tmc)
{
        dl_list_free (&tmc->config_list_);
        tl_close (&tmc->lib_loader_);
        MIN_INFO ("Process ended");
        min_log_close ();
        mq_flush_msg_buffer ();
}

/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ================= TESTS FOR LOCAL FUNCTIONS ============================= */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */
