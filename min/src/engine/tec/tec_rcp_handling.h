/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Robert Galkowski
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
 *  @file       tec_rcp_handling.h
 *  @version    0.1
 *  @brief      This file contains header file for rcp handling
 */

/* ------------------------------------------------------------------------- */
/* INCLUDES */
/* None */
/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */


/** Structure for holding master/slave association data */
typedef struct {
        /** slave's device id*/
        int             slave_id_;
        /** name of slave given in allocate command*/
        char            slave_name_[128];
} slave_info;


/** Function called when message received from external controller
 * @param message sting containing actual message
 * @param length length of message (this param will probobly be dropped
 * @return result of operation
*/
int             tec_extif_message_received (char *message, int length);

void            send_to_master (int tc_id, char *msg);

DLList         *EXTIF_received_data;

/* End of file */
