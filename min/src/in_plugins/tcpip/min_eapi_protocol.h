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
#define MIN_MSG_LENGHT_MIN   4     /** Minimum length of message */
#define MIN_HDR_LEN          3     /** Lenght of MIN Header */
#define MIN_PROTO_ERROR      222   /** Protocol error */
/* MIN Engine API protocol message types */
/* Engine -> client */
/** New Module Indication 
 *  +----------------+
 *  |      Test      | 3
 *  |     Module     | 4
 *  |       Id       | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |     Module     | 8
 *  |      Name      | ..
 *  |                | Length
 *  +----------------+ 
 */
#define MIN_NEW_MOD_IND       0x01 
/** No Module Indication 
 *  +----------------+
 *  |      Test      | 3
 *  |     Module     | 4
 *  |      Name      | ..
 *  |                | Length
 *  +----------------+ 
 */
#define MIN_NO_MOD_IND        0x02 
/** Module Ready Indication 
 *  +----------------+
 *  |      Test      | 3
 *  |     Module     | 4
 *  |       Id       | 5
 *  |                | 6
 *  +----------------+
 */
#define MIN_MOD_READY_IND     0x03 
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
 *  |      Title     | Length
 *  +----------------+
 */
#define MIN_NEW_CASE_IND      0x04 
/** Case Started Indication 
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
#define MIN_CASE_STARTED_IND  0x05 
/** Case Paused Indication
 *  +----------------+ 
 *  |      Test      | 3
 *  |      Run       | 4
 *  |       Id       | 5
 *  |                | 6
 *  +----------------+ 
 */
#define MIN_CASE_PAUSED_IND   0x06 
/** Case Resumed Indication
 *  +----------------+ 
 *  |      Test      | 3
 *  |      Run       | 4
 *  |       Id       | 5
 *  |                | 6
 *  +----------------+ 
 */
#define MIN_CASE_RESUMED_IND  0x07 
/** Case Result Indication 
 *  +----------------+ 
 *  |      Test      | 3
 *  |      Run       | 4
 *  |       Id       | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |      Result    | 8
 *  |      Code      | 9
 *  |                | 10
 *  +----------------+
 *  |      Start     | 11
 *  |      Time      | 12
 *  |      Stamp     | 13
 *  |                | 14
 *  +----------------+
 *  |      End       | 15
 *  |      Time      | 16
 *  |      Stamp     | 17
 *  |                | 18
 *  +----------------+
 *  |      Test      | 19
 *  |     Result     | ..
 *  |    Description | Length
 *  +----------------+
 */
#define MIN_CASE_RESULT_IND   0x08 
/** Printout Indication
 *  +----------------+ 
 *  |      Test      | 3
 *  |      Run       | 4
 *  |       Id       | 5
 *  |                | 6
 *  +----------------+
 *  |                | 7
 *  |     Printout   | ..
 *  |                | Length
 *  +----------------+
 */
#define MIN_PRINTOUT_IND      0x09 
/** Test Module List Indication
 *  +----------------+ 
 *  |      Module    | 3
 *  |       Name     | 4
 *  |      Lenght    | 5
 *  |                | 6
 *  +----------------+
 *  |     Module     | 7
 *  |      Name      | ..
 *  |                | 7 + Module Name Lenght
 *  +----------------+
 *         ....
 */
#define MIN_MODULE_LIST_IND   0x0a
/** Test Case File List Indication
 *  +----------------+ 
 *  |    Case File   | 3
 *  |      Name      | 4
 *  |     Lenght     | 5
 *  |                | 6
 *  +----------------+
 *  |     Module     | 7
 *  |      Name      | ..
 *  |                | 7 + Case File Name Lenght
 *  +----------------+
 *         ....
 */
#define MIN_FILE_LIST_IND     0x0b 
/** Error Report Indication
 *  +----------------+ 
 *  |     Error      | 3
 *  |     Report     | ..
 *  |                | Lenght
 *  +----------------+
 */
#define MIN_ERROR_REPORT_IND  0x0c 
/** Add Module Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 *  |  Resp code     | 7 (0 = success, 222 = Protocol error)
 *  +----------------+
 */
#define MIN_RCP_IND           0x0d /** RCP Protocol Message Indication */
#define MIN_RESP              0x0e /** Response for any request*/
/** Case Description Indication 
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
 *  |   Description  | Length
 *  +----------------+
 */
#define MIN_CASE_DESC_IND      0x0f 
/* client -> Engine */
/** Add Module Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |     Module     | ..
 *  |      Path      | ..
 *  |                | Length
 *  +----------------+ 
 */
#define MIN_ADD_MOD_REQ       0x20
/** Add Test Case File Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |     Module     | 8
 *  |       Id       | 9
 *  |                | 10
 *  +----------------+
 *  |      Test      | 11
 *  |      Case      | ..
 *  |      File      | ..
 *  |      Name      | Lenght
 *  +----------------+ 
 */
#define MIN_ADD_CASE_FILE_REQ 0x21 
/** Start Case Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |     Module     | 8
 *  |       Id       | 9
 *  |                | 10
 *  +----------------+
 *  |      Test      | 11
 *  |      Case      | 12
 *  |       Id       | 13
 *  |                | 14
 *  +----------------+ 
 *  |      Test      | 15
 *  |      Group     | 16
 *  |       Id       | 17
 *  |                | 18
 *  +----------------+ 
 */
#define MIN_START_CASE_REQ    0x22 
/** Pause Case Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |      Run       | 8
 *  |       Id       | 9
 *  |                | 10
 *  +----------------+
 */
#define MIN_PAUSE_CASE_REQ    0x23 
/** Resume Case Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |      Run       | 8
 *  |      Id        | 9
 *  |                | 10
 *  +----------------+
 */
#define MIN_RESUME_CASE_REQ   0x24 
/** MIN Abort Case Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 *  |      Test      | 7
 *  |      Run       | 8
 *  |       Id       | 9
 *  |                | 10
 *  +----------------+
 */
#define MIN_ABORT_CASE_REQ    0x25 
/** MIN Fatal Error Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 *  |     "What"     | 7
 *  |     String     | 8
 *  |     Length     | 9
 *  |                | 10
 *  +----------------+
 *  |     "What"     | 11
 *  |     String     | 10 + "What" String Lenght
 *  +----------------+
 *  |     "Error"    | 11 + "What" String Lenght
 *  |     String     | .. 
 *  |                | Lenght
 *  +----------------+
 */
#define MIN_FATAL_ERR_REQ     0x26 
/** MIN Open Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 */
#define MIN_OPEN_REQ          0x27 
/** MIN Close Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 */
#define MIN_CLOSE_REQ         0x28 
/** Test Module Files Query Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 */
#define MIN_MOD_QUERY_REQ     0x29 
/** Test Test Case Files Query Request
 *  +----------------+
 *  |                | 3
 *  |  Transaction   | 4
 *  |   Identifier   | 5
 *  |                | 6
 *  +----------------+
 */
#define MIN_FILE_QUERY_REQ    0x2a 
#define MIN_SLAVE_REG_REQ     0x2b /** Register Slave Request */

/* ------------------------------------------------------------------------- */
/* DATA TYPES */
/** MIN Engine API protocol message header 
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
