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
 *  @file       combined_test_process.h
 *  @version    0.1
 *  @brief      This file contains header file of the Test Process of Combiner
 */

#ifndef COMBINED_TEST_PROCESS_H
#define COMBINED_TEST_PROCESS_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
/* ------------------------------------------------------------------------- */
#include <dllist.h>
#include <tmc.h>
#include <min_scripter_if.h>
#include <min_ipc_mechanism.h>
#include <min_item_parser.h>
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/** Handler for the SIGUSR2 signal. */
void            ctp_handle_sigusr2 (int signum);
/** Handler for the SIGTSTP signal. */
void            ctp_hande_sigtstp (int signo, siginfo_t * info,
                                   void *context);

/* ------------------------------------------------------------------------- */
#endif                          /* COMBINED_TEST_PROCESS_H */
/* End of file */
