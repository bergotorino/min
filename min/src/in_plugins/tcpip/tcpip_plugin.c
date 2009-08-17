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
 *  @file       tcpip_plugin.c
 *  @version    0.1
 *  @brief      This file contains implementation of tcp/ip plugin for MIN
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#include <tcpip_plugin.h>
#include <min_eapi_protocol.h>
#include <min_system_logger.h>
#include <min_plugin_interface.h>
#include <min_text.h>
#include <min_logger.h>
#include <tllib.h>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */


/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
eapiOut_t min_clbk;
int fd;
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
/* None*/
/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* ------------------------------------------------------------------------- */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL void eapi_message_send_resp (unsigned tid, char resp_code);
/* ------------------------------------------------------------------------- */
LOCAL void eapi_message_receive (char *msg, unsigned short len) ;
/* ------------------------------------------------------------------------- */
LOCAL void eapi_message_send (char *msg, unsigned short len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_add_mod_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_add_case_file_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_start_case_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_pause_case_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_resume_case_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_abort_case_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_abort_case_resp (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_open_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_close_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_mod_query_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_file_query_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL int handle_slave_reg_req (char *msg, unsigned short msg_len);
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_module (char *modulename, unsigned moduleid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_no_module (char *modulename);
/* ------------------------------------------------------------------------- */
LOCAL void pl_module_ready (unsigned moduleid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_started (unsigned moduleid,
			    unsigned caseid,
			    long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_result (long testrunid, int result, char *desc,
			   long starttime, long endtime);
/* ------------------------------------------------------------------------- */
LOCAL void pl_msg_print (long testrunid, char *message);
/* ------------------------------------------------------------------------- */
LOCAL void pl_error_report (char *error);
/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
LOCAL void write32 (char *buff, int val)
{
	buff [0] = val >> 24;
	buff [1] = val >> 16;
	buff [2] = val >> 8;
	buff [3] = val;
}
/* ------------------------------------------------------------------------- */
LOCAL void write16 (char *buff, int val)
{
	buff [0] = val >> 8;
	buff [1] = val;
}
/* ------------------------------------------------------------------------- */
LOCAL void eapi_build_header (char *buff, char msg_type, 
			      unsigned short msg_len)
{
	buff [0] = msg_type;
	write16 (&buff [1], htons (msg_len));
}
/* ------------------------------------------------------------------------- */
LOCAL void eapi_message_send_ind (char msg_type, 
				  unsigned short msg_len,
				  char *buff ) 
{
	eapi_build_header (buff, msg_type, msg_len);
	eapi_message_send (buff, msg_len + MIN_HDR_LEN);
}
/* ------------------------------------------------------------------------- */
LOCAL void eapi_message_send_resp (unsigned tid, char resp_code)
{
	char buff [8];

	eapi_build_header (buff, MIN_RESP, 5);
	write32 (&buff [MIN_HDR_LEN], tid);
	buff [7] = resp_code;
	eapi_message_send (buff, 8);
}
/* ------------------------------------------------------------------------- */
LOCAL void eapi_message_send (char *msg, unsigned short len) 
{
	int bytes_wrtn;
	
	bytes_wrtn = write (fd, msg, len);

}

/* ------------------------------------------------------------------------- */
/** Dispatcher for MIN Engine API messages towards the Engine. 
 *  @param msg protocol message
 *  @param len message len
 *
 *  Sends response after the message has been processed.
 */
LOCAL void eapi_message_receive (char *msg, unsigned short len) 
{
	char msg_type;
	char *pos;
	unsigned short msg_len = 0;
	unsigned tid = 0;
	int      res = 1;

	if (len < MIN_MSG_LENGHT_MIN) {
		MIN_WARN ("Too short EAPI message %u", len);
		return;
	}
	pos = msg;
	msg_type = *msg;
	pos ++;
	msg_len = *pos | (*(pos + 1) << 8);
	pos += 2;
	tid = *pos | (*(pos + 1) << 8) | (*(pos + 2) << 16) | 
		(*(pos + 3) << 24);
	pos += 4;
	switch (msg_type) {
	case MIN_ADD_MOD_REQ:
		res = handle_add_mod_req (pos, msg_len);
		break;
	case MIN_ADD_CASE_FILE_REQ:
		res = handle_add_case_file_req (pos, msg_len);
		break;
	case MIN_START_CASE_REQ:
		res = handle_start_case_req (pos, msg_len);
		break;
	case MIN_PAUSE_CASE_REQ:
		res = handle_pause_case_req (pos, msg_len);
		break;
	case MIN_RESUME_CASE_REQ:
		res = handle_resume_case_req (pos, msg_len);
		break;
	case MIN_ABORT_CASE_REQ:
		res = handle_abort_case_req (pos, msg_len);
		break;
	case MIN_FATAL_ERR_REQ:
		res = handle_abort_case_resp (pos, msg_len);
		break;
	case MIN_OPEN_REQ:
		res = handle_open_req (pos, msg_len);
		break;
	case MIN_CLOSE_REQ:
		res = handle_close_req (pos, msg_len);
		break;
	case MIN_MOD_QUERY_REQ:
		res = handle_mod_query_req (pos, msg_len);
		break;
	case MIN_FILE_QUERY_REQ:
		res = handle_file_query_req (pos, msg_len);
		break;
	case MIN_SLAVE_REG_REQ:
		res = handle_slave_reg_req (pos, msg_len);
		break;
	default:
		MIN_WARN ("Unknown message type %02x", msg_type); 
		res = MIN_PROTO_ERROR;
		break;
	}
	
	eapi_message_send_resp (tid, res);
}
/* ------------------------------------------------------------------------- */
/** Handles Add Module Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), 
 *          or MIN_PROTO_ERROR in case of protocol error
 */
LOCAL int handle_add_mod_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;
	
	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Add Test Case File Request message. 
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), 
 *          or MIN_PROTO_ERROR in case of protocol error
 */
LOCAL int handle_add_case_file_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Start Case Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), 
 *          or MIN_PROTO_ERROR in case of protocol error
 */
LOCAL int handle_start_case_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Pause Case Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), 
 *          or MIN_PROTO_ERROR in case of protocol error
 */
LOCAL int handle_pause_case_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Resume Case Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), 
 *          or MIN_PROTO_ERROR in case of protocol error
 */
LOCAL int handle_resume_case_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Abort Case Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), 
 *          or MIN_PROTO_ERROR in case of protocol error
 */
LOCAL int handle_abort_case_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Fatal Error Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), or MIN_PROTO_ERROR 
 *          in case of protocol error
 */
LOCAL int handle_abort_case_resp (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles MIN Open Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), 
 *          or MIN_PROTO_ERROR in case of protocol error
 */
LOCAL int handle_open_req (char *msg, unsigned short msg_len)
{
	int ret;
	
	ret = min_clbk.min_open();
	
	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles MIN Close Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), 
 *          or MIN_PROTO_ERROR in case of protocol error
 */
LOCAL int handle_close_req (char *msg, unsigned short msg_len)
{
	int ret;

	ret = min_clbk.min_close();

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Test Module Files Query Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), or MIN_PROTO_ERROR 
 *          in case of protocol error
 */
LOCAL int handle_mod_query_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Register Slave Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), or MIN_PROTO_ERROR 
 *          in case of protocol error
 */
LOCAL int handle_file_query_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Register Slave Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), 
 *          or MIN_PROTO_ERROR in case of protocol error
 */
LOCAL int handle_slave_reg_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Engine calls this for each module it is configured with
 *  @param modulename name of the module
 *  @param moduleid module id
 */ 
LOCAL void pl_new_module (char *modulename, unsigned moduleid)
{
	
	unsigned short msg_len;
	char *p, *buff;
	msg_len = 4 + strlen (modulename);
	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	
	p = &buff [MIN_HDR_LEN];
	write32 (p, moduleid);
	p += 4;
	memcpy (p, modulename, strlen (modulename));

	eapi_message_send_ind (MIN_NEW_MOD_IND, 
			       msg_len,
			       buff);

	return;
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when module adding fails
 *  @param modulename name of the module
 */ 
LOCAL void pl_no_module (char *modulename)
{

	return;
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when all the cases for module are reported
 *  @param moduleid module id
 */ 
LOCAL void pl_module_ready (unsigned moduleid)
{
	unsigned short msg_len;
	char *p, *buff;
	msg_len = 4;
	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	
	p = &buff [MIN_HDR_LEN];
	write32 (p, moduleid);
	p += 4;

	eapi_message_send_ind (MIN_NEW_MOD_IND, 
			       msg_len,
			       buff);

}
/* ------------------------------------------------------------------------- */
/** Engine calls this for each new test case
 *  @param moduleid id of the module this test case belongs to
 *  @param caseid id of the test case
 *  @param casetitle test case title
 */ 
LOCAL void pl_new_case (unsigned moduleid, unsigned caseid, char *casetitle)
{
	
	unsigned short msg_len;
	char *p, *buff;
	msg_len = 4 + 4 + strlen (casetitle);
	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	
	p = &buff [MIN_HDR_LEN];
	write32 (p, moduleid);
	p += 4;
	write32 (p, caseid);
	p += 4;
	memcpy (p, casetitle, strlen (casetitle));

	eapi_message_send_ind (MIN_NEW_CASE_IND, 
			       msg_len,
			       buff);
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when test case has been started
 *  @param moduleid id of the module this test case belongs to
 *  @param caseid id of the test case
 *  @param testrunid identifier for the test run
 */ 
LOCAL void pl_case_started (unsigned moduleid,
			    unsigned caseid,
			    long testrunid)
{
	
	unsigned short msg_len;
	char *p, *buff;
	msg_len = 4 + 4 + 4;
	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	
	p = &buff [MIN_HDR_LEN];
	write32 (p, moduleid);
	p += 4;
	write32 (p, caseid);
	p += 4;
	write32 (p, testrunid);
	p += 4;

	eapi_message_send_ind (MIN_NEW_CASE_IND, 
			       msg_len,
			       buff);

}
/* ------------------------------------------------------------------------- */
/** Engine calls this when it when test case has finished
 *  @param testrunid identifier for the test run
 *  @param result test case result
 *  @param desc test result description
 *  @param starttime starting timestamp
 *  @param endtime time the test case has finnished
 */ 
LOCAL void pl_case_result (long testrunid, int result, char *desc,
			   long starttime, long endtime){
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when it when test case has sent print data
 *  @param testrunid identifier for the test run
 *  @param message the test case message
 */ 
LOCAL void pl_msg_print (long testrunid, char *message)
{
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when it when test case/module sends error message
 *  @param error the message
 */ 
LOCAL void pl_error_report (char *error) {
}

/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Plugin attach function, sets the callbacks.
 *  @param out_callback callbacks called by the engine
 *  @param in_callback callbacks towards the engine
 */
void pl_attach_plugin (eapiIn_t **out_callback, eapiOut_t *in_callback)
{
        /* Binds the callbacks */
        memcpy (&min_clbk,in_callback,sizeof(eapiOut_t));

        (*out_callback)->case_result            = pl_case_result;
        (*out_callback)->case_started           = pl_case_started;
        (*out_callback)->case_paused            = NULL;
        (*out_callback)->case_resumed           = NULL;
        (*out_callback)->module_prints          = pl_msg_print;
        (*out_callback)->new_module             = pl_new_module;
        (*out_callback)->no_module              = pl_no_module;
        (*out_callback)->module_ready           = pl_module_ready;
        (*out_callback)->new_case               = pl_new_case;
        (*out_callback)->error_report           = pl_error_report;

        return;
}
/* ------------------------------------------------------------------------- */
/** Plugin open function 
 *  @param opts contains options struct for TCP/IP plugin
 */
void pl_open_plugin (void *opts)
{
	tcpip_opts *pl_opts;
	
	pl_opts = (tcpip_opts *)opts;
	
	fd = pl_opts->fd_;
        return;
}
/* ------------------------------------------------------------------------- */
