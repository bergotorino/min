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
 *  @file       tmc_tpc.h
 *  @version    0.1
 *  @brief      This file contains header file of the TMC TP supervising part
 */

#ifndef TMC_TPC_H
#define TMC_TPC_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <sys/types.h>          /* pid_t */
#include <signal.h>
#include <min_logger.h>
#include <tmc_common.h>

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/** Usefull typedef */
typedef struct _TestProcessController TestProcessController;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* ------------------------------------------------------------------------- */
/** Structure that holds Test Process related stuff */
struct _TestProcessController {
        pid_t           tp_pid_;        /**< Test Process PID */
        MINTPStatus    tp_status_;     /**< Test Process status */
        int             tp_timeout_;    /**< Timeout of the Test Process */
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
void            tp_init (TestProcessController * tpc);
/* ------------------------------------------------------------------------- */
void            tp_pause (TestProcessController * tpc);
/* ------------------------------------------------------------------------- */
void            tp_resume (TestProcessController * tpc);
/* ------------------------------------------------------------------------- */
void            tp_abort (TestProcessController * tpc);
/* ------------------------------------------------------------------------- */
void            tp_set_timeout (TestProcessController * tpc, unsigned int t);
/* ------------------------------------------------------------------------- */
unsigned int    tp_timeout (const TestProcessController * tpc);
/* ------------------------------------------------------------------------- */
void            tp_set_status (TestProcessController * tpc, MINTPStatus s);
/* ------------------------------------------------------------------------- */
MINTPStatus    tp_status (const TestProcessController * tpc);
/* ------------------------------------------------------------------------- */
void            tp_set_pid (TestProcessController * tpc, pid_t pid);
/* ------------------------------------------------------------------------- */
pid_t           tp_pid (const TestProcessController * tpc);
/* ------------------------------------------------------------------------- */
void            tp_set_timeout_handler ();
/* ------------------------------------------------------------------------- */
void            tp_start_timeout (const TestProcessController * tpc);
/* ------------------------------------------------------------------------- */
#endif                          /* TMC_TPC_H */
/* End of file */
