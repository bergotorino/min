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
 *  @file       tmc_ipc.h
 *  @version    0.1
 *  @brief      This file contains header file of the TMC IPC part
 */

#ifndef TMC_IPC_H
#define TMC_IPC_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <tmc_common.h>
#include <min_ipc_mechanism.h>
#include <dllist.h>
#include <min_test_event_if.h>
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/** Usefull typedef */
typedef struct _TMCIPCInterface TMCIPCInterface;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/** TMC's interface to the IPC Mechanism */
struct _TMCIPCInterface {
        int             mqid_;  /**< Mesage Queue id */
        long            sender_;/**< PID of the message sende */
        long            receiver_; /**< PID of the recieving process */
        MsgBuffer       in_;    /**< Buffer for the incoming messages */
};
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
void            ip_init (TMCIPCInterface * tmcipi, int reciever);
/* ------------------------------------------------------------------------- */
int             ip_status (const TMCIPCInterface * tmcipi);
/* ------------------------------------------------------------------------- */
void            ip_send_ok (const TMCIPCInterface * tmcipi);
/* ------------------------------------------------------------------------- */
void            ip_send_ko (const TMCIPCInterface * tmcipi, int errnum,
                            const char *desc);
/* ------------------------------------------------------------------------- */
void            ip_send_tcd (const TMCIPCInterface * tmcipi, const char *file,
                             DLListIterator it, int module_version);
/* ------------------------------------------------------------------------- */
void            ip_send_ret (const TMCIPCInterface * tmcipi, int result,
                             const char *desc);
/* ------------------------------------------------------------------------- */
void            ip_send_eotcd (const TMCIPCInterface * tmcipi);
/* ------------------------------------------------------------------------- */
#endif                          /* TMC_IPC_H */
/* End of file */
