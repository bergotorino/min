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
 *  @file       scripted_test_process.h
 *  @version    0.1
 *  @brief      This file contains header file of the Test Process of Scripter
 */

#ifndef SCRIPTED_TEST_PROCESS_H
#define SCRIPTED_TEST_PROCESS_H

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
void            stp_handle_message (const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
void            stp_handle_sigusr2 (int signum);
/* ------------------------------------------------------------------------- */
void            stp_handle_sigsegv (int signum);
/* ------------------------------------------------------------------------- */
void            stp_handle_sigabort (int signum);
/* ------------------------------------------------------------------------- */
void            stp_exit (void);
/* ------------------------------------------------------------------------- */
#endif                          /* SCRIPTED_TEST_PROCESS_H */
/* End of file */
