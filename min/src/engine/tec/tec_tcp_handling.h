/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       tec_tcp_handling.h
 *  @version    0.1
 *  @brief      Header for tec tcp socket handling
 */

#ifndef TEC_TCP_HANDLING_H
#define TEC_TCP_HANDLING_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
#include <tec_rcp_handling.h>
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
/* None */

/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
void *ec_poll_sockets (void *arg);
/* ------------------------------------------------------------------------- */
void new_tcp_master (int socket);
/* ------------------------------------------------------------------------- */
int ec_create_listen_socket();
/* ------------------------------------------------------------------------- */
void socket_send_rcp (char *cmd, char *sender, char *rcvr, char* msg, int fd);
/* ------------------------------------------------------------------------- */
int allocate_ip_slave (char *slavetype, char *slavename, pid_t pid);
/* ------------------------------------------------------------------------- */
void tcp_slave_close (slave_info *slave);
/* ------------------------------------------------------------------------- */
int tcp_msg_handle_response (MinItemParser *mip);
/* ------------------------------------------------------------------------- */
void tcp_master_report (int run_id, int execution_result, 
			int test_result, char *desc);
/* ------------------------------------------------------------------------- */
int tcp_remote_run (char *module, char *casefile, int caseid, 
		    char *casetitle);
#endif /* TEC_TCP_HANDLING_H */
/* End of file */
