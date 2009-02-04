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
 *  @file       tmc.h
 *  @version    0.1
 *  @brief      This file contains header file of the Test Module Controller
 */

#ifndef TMC_H
#define TMC_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include <tmc_common.h>
#include <tmc_tpc.h>
#include <tmc_ipc.h>
#include <tllib.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/** Structure that holds all Test Module Controller needed stuff */
typedef struct {
        /** Message Queue Communication interface */
        TMCIPCInterface tmcipi_;

        /** Exit flag */
        int             run_;

        /** Test process supervising part */
        TestProcessController tpc_;

        /** Test Library Loader */
        TestLibraryLoader lib_loader_;

        /** List of config files associated with test library */
        DLList         *config_list_;

        /** Flag stating that we should send the MSG_RET */
        TSBool  send_ret_;
} TMC_t;

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
void            gu_init_tmc (TMC_t * tmc, int argc, char *argv[]);
/* ------------------------------------------------------------------------- */
void            gu_run_tmc (TMC_t * tmc);
/* ------------------------------------------------------------------------- */
void            gu_done_tmc (TMC_t * tmc);
/* ------------------------------------------------------------------------- */
#endif                          /* TMC_H */
/* End of file */
