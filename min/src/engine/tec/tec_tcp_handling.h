/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Sampo Saaristo
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
 *  @file       tec_tcp_handling.h
 *  @version    0.1
 *  @brief      Header for tec tcp socket handling
 */

#ifndef TEC_TCP_HANDLING_H
#define TEC_TCP_HANDLING_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */

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
int ec_poll_sockets ();
/* ------------------------------------------------------------------------- */
int ec_create_listen_socket();
/* ------------------------------------------------------------------------- */
void socket_send_rcp (char *cmd, char *sender, char *rcvr, char* msg, int fd);
/* ------------------------------------------------------------------------- */
int allocate_ip_slave (char *slavetype);
/* ------------------------------------------------------------------------- */
#endif /* TEC_TCP_HANDLING_H */
/* End of file */
