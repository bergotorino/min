/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Juha Perala
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
 *  @file       min_eapi_protocol.h
 *  @version    0.1
 *  @brief      Defines MIN engine api protocol
 */

#ifndef MIN_EAPI_PROTOCOL_H
#define MIN_EAPI_PROTOCOL_H

/* ------------------------------------------------------------------------- */
/* INCLUDES */
/* None */
/* ----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 */
/* None */
/* ------------------------------------------------------------------------- */
/* MACROS */
#define MIN_MSG_LENGHT_MIN   7     /** Minimum length of message */
#define MIN_HDR_LEN          3     /** Lenght of MIN Header */
#define MIN_PROTO_ERROR      222   /** Protocol error */
/* MIN Engine API protocol message types */
/* Engine -> client */
#define MIN_NEW_MOD_IND       0x01 /** New Module Indication */
#define MIN_NO_MOD_IND        0x02 /** No Module Indication */
#define MIN_MOD_READY_IND     0x03 /** Module Ready Indication */
/** New Case Indication 
 *  +----------------+
 *  |      Test      | 3
 *  |     Module     | 4
 *  |       Id       | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |      Case      | 8
 *  |       Id       | 9
 *  |                | 10
 *  +----------------+ 
 *  |      Test      | 11
 *  |      Case      | ..
 *  |      Title     | N
 *  +----------------+
 */
#define MIN_NEW_CASE_IND      0x04 
/** New Case Indication 
 *  +----------------+
 *  |      Test      | 3
 *  |     Module     | 4
 *  |       Id       | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |      Case      | 8
 *  |       Id       | 9
 *  |                | 10
 *  +----------------+ 
 *  |      Test      | 11
 *  |      Run       | 12
 *  |       Id       | 13
 *  |                | 14
 *  +----------------+ 
 */
#define MIN_CASE_STARTED_IND  0x05 /** Case Started Indication */
#define MIN_CASE_PAUSED_IND   0x06 /** Case Paused Indication */
#define MIN_CASE_RESUMED_IND  0x07 /** Case Resumed Indication */
#define MIN_CASE_RESULT_IND   0x08 /** Case Result Indication */
#define MIN_PRINTOUT_IND      0x09 /** Printout Indication */
#define MIN_MODULE_LIST_IND   0x0a /** Test Module List Indication */
#define MIN_FILE_LIST_IND     0x0b /** Test Case File List Indication */
#define MIN_ERROR_REPORT_IND  0x0c /** Error Report Indication */
#define MIN_RCP_IND           0x0d /** RCP Protocol Message Indication */
#define MIN_RESP              0x0e /** Response for any request*/
/* client -> Engine */
#define MIN_ADD_MOD_REQ       0x20 /** Add Module Request */
#define MIN_ADD_CASE_FILE_REQ 0x21 /** Add Test Case File Request */
#define MIN_START_CASE_REQ    0x22 /** Start Case Request */
#define MIN_PAUSE_CASE_REQ    0x23 /** Pause Case Request */
#define MIN_RESUME_CASE_REQ   0x24 /** Resume Case Request */
#define MIN_ABORT_CASE_REQ    0x25 /** Abort Case Request */
#define MIN_FATAL_ERR_REQ     0x26 /** Fatal Error Request */
#define MIN_OPEN_REQ          0x27 /** MIN Open Request */
#define MIN_CLOSE_REQ         0x28 /** MIN Close Request */
#define MIN_MOD_QUERY_REQ     0x29 /** Test Module Files Query Request */
#define MIN_FILE_QUERY_REQ    0x2a /** Test Case Files Query Request */
#define MIN_SLAVE_REG_REQ     0x2b /** Register Slave Request */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/** MIN Engine api protocol message header 
 *  +----------------+
 *  |    msg type    | 0
 *  +----------------+
 *  |    Length      | 1
 *  |                | 2
 *  +----------------+ 
 */
typedef struct {
	unsigned char msg_type_; /** Protocol message type */
	unsigned short lenght_;  /** Length of message, excluding msg_type_ */
} min_eapi_hdr;

/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* STRUCTURES */
/* None */
/* ------------------------------------------------------------------------- */
/* FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
/* None */
#endif                          /* MIN_EAPI_PROTOCOL_H */
/* End of file */
