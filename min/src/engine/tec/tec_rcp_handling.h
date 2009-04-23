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
#ifndef TEC_RCP_HANDLING_H
#define TEC_RCP_HANDLING_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <sys/socket.h>
#include <netdb.h>
#include <min_text.h>
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

	/** name of the slave */
	Text           *slave_name_;

	/** type information e.g. "phone" */
	Text           *slave_type_;
	
	/** host address */
	struct          hostent he_;
	
	/** reserved flag */
#define SLAVE_STAT_FREE 0x0
#define SLAVE_STAT_RESERVED 0x1
#define SLAVE_STAT_RESULT 0x2
	int             status_;
	
	/** socket for communicating with the slave */
	int             fd_;
	
	/** socket write queue */
	DLList         *write_queue_;
} slave_info;


/** Function called when message received from external controller
 * @param message sting containing actual message
 * @param length length of message (this param will probobly be dropped
 * @return result of operation
*/
int             tec_extif_message_received (char *message, int length);

void            send_to_master (int tc_id, char *msg);

DLList         *EXTIF_received_data;

void            rcp_handling_init ();
 
void            rcp_handling_cleanup ();

int             tec_add_ip_slave_to_pool (struct hostent *he, char *slavetype);

int             tec_del_ip_slave_from_pool (struct hostent *he, 
					    char *slavetype);

#endif
/* End of file */
