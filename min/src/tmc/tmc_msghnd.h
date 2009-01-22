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
 *  @file       tmc_msghnd.h
 *  @version    0.1
 *  @brief      This file contains header file of the TMC message handling part
 */

#ifndef TMC_MSGHND_H
#define TMC_MSGHND_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <stdio.h>
#include <sys/wait.h>

#include <tmc_common.h>
#include <tllib.h>
#include <tmc_tpc.h>
#include <tmc_ipc.h>

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
struct TMC_t;
/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
void            gu_read_message (TMC_t * tmc, MsgBuffer * input_buffer);
/* ------------------------------------------------------------------------- */
void            gu_handle_message (TMC_t * tmc, const MsgBuffer * msg);
/* ------------------------------------------------------------------------- */
#endif                          /* TMC_MSGHND_H */
/* End of file */
