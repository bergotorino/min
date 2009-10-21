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
#include <limits.h>
#include <errno.h>

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
eapiOut_t min_clbk;   /** callbacks towards engine */
int fd;               /** socket */
int run = 1;          /** Exit flag */
DLList *write_queue;  /** socket write queue */
/** Mutex to protect the write_queue */
pthread_mutex_t tcpip_mutex_ = PTHREAD_MUTEX_INITIALIZER;

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
LOCAL void write32 (char *buff, int val);
/* ------------------------------------------------------------------------- */
LOCAL void write16 (char *buff, int val);
/* ------------------------------------------------------------------------- */
LOCAL short read16 (unsigned char *buff);
/* ------------------------------------------------------------------------- */
LOCAL int read32 (unsigned char *buff);
/* ------------------------------------------------------------------------- */
LOCAL void read_from_socket (int fd);
/* ------------------------------------------------------------------------- */
LOCAL void write_to_socket (int fd);
/* ------------------------------------------------------------------------- */
/*LOCAL void eapi_message_send_resp (unsigned tid, char resp_code); */
/* ------------------------------------------------------------------------- */
LOCAL void eapi_message_receive (char msg_type, unsigned short len,
	                         char *msg);
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
LOCAL int handle_fatal_err_req (char *msg, unsigned short msg_len);
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
LOCAL void pl_case_paused (long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_resumed (long testrunid);
/* ------------------------------------------------------------------------- */
LOCAL void pl_case_result (long testrunid, int result, char *desc,
			   long starttime, long endtime);
/* ------------------------------------------------------------------------- */
LOCAL void pl_msg_print (long testrunid, char *message);
/* ------------------------------------------------------------------------- */
LOCAL void pl_error_report (char *error);
/* ------------------------------------------------------------------------- */
LOCAL void pl_test_modules (char *modulelist);
/* ------------------------------------------------------------------------- */
LOCAL void pl_test_files (char *modulelist);
/* ------------------------------------------------------------------------- */

/* MODULE DATA STRUCTURES */
typedef struct {
	char *msg_;
	unsigned short msg_len_;
} eapi_msg_container; /** Local container for EAPI messages */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/** Read MIN Engine API protocol message from socket
 *  @param fd socket file desciptor
 */
LOCAL void read_from_socket (int fd)
{
	int bytes_read, total_read = 0;
	unsigned short len;
	char hdr_buff [MIN_HDR_LEN], *buff = INITPTR;

	/* read the header to know the message len */
	bytes_read = read (fd, hdr_buff, MIN_HDR_LEN);
	if (bytes_read != MIN_HDR_LEN) {
		MIN_WARN ("can't read the MIN header");
		goto err_out;
		return;
	}
	len = read16 ((unsigned char *)&hdr_buff[1]);
	if (len >= (INT_MAX - 1)) {
		MIN_WARN ("Invalid msg len %d", len);
		goto err_out;
		return;
	}
	MIN_DEBUG ("message len %u", len);
	buff = NEW2 (char, len);
	while (total_read < len) {
		bytes_read = read (fd, buff + total_read, len - total_read);
		if (bytes_read == -1) {
			MIN_WARN ("read() error %s", strerror (errno));
			goto err_out;
		}
		total_read += bytes_read;
	}
	eapi_message_receive (hdr_buff [0], len, buff);
	DELETE (buff);
	return;
err_out:
	if (buff != INITPTR)
		DELETE (buff);
	close(fd);
	fd = -1;

	return;
}
/* ------------------------------------------------------------------------- */
/** Write message from write queue to socket
 *  @param fd socket file desciptor
 */
LOCAL void write_to_socket (int fd)
{
	int bytes_wrtn = 0, wrtn;
	DLListIterator it;
	eapi_msg_container *cont;
	int i;
	char buff [4096];

	pthread_mutex_lock (&tcpip_mutex_);
	it = dl_list_head (write_queue);
	if (it == DLListNULLIterator) {
		pthread_mutex_unlock (&tcpip_mutex_);
		return;
	}
	cont = dl_list_data (it);
	
	while (bytes_wrtn < cont->msg_len_) {
		wrtn = write (fd, cont->msg_ + bytes_wrtn, cont->msg_len_ - 
			      bytes_wrtn);
		if (wrtn == -1) {
			switch (errno) {
			case EAGAIN:
			case EINTR:
				continue;
				break;
			default:
				MIN_WARN ("write failed %s", strerror (errno));
				pthread_mutex_unlock (&tcpip_mutex_);
				return;
				break;
			}
		}
		bytes_wrtn += wrtn;
	}
	MIN_DEBUG ("wrote %u bytes", bytes_wrtn);
	memset (buff, 0x0, 4096);
	for (i = 0; i  < cont->msg_len_; i++) {
		sprintf (buff, "%s%02x", buff, (unsigned char)cont->msg_[i]);
	}
	MIN_DEBUG ("%s", buff);

	DELETE (cont->msg_);
	DELETE (cont);
	dl_list_remove_it (it);
	pthread_mutex_unlock (&tcpip_mutex_);

	return;
}
/* ------------------------------------------------------------------------- */
/** Write 32 bit number to buffer.
 *  @param buff [out] the buffer to write into.
 *  @param val the value to write.
 */
LOCAL void write32 (char *buff, int val)
{
	buff [0] = val >> 24;
	buff [1] = val >> 16;
	buff [2] = val >> 8;
	buff [3] = val;
}
/* ------------------------------------------------------------------------- */
/** Write 16 bit number to buffer.
 *  @param buff [out] the buffer to write into.
 *  @param val the value to write.
 */
LOCAL void write16 (char *buff, int val)
{
	buff [0] = val >> 8;
	buff [1] = val;
}
/* ------------------------------------------------------------------------- */
/** Read 16 bit integer from buffer
 *  @param buff the buffer to read from
 *  @return two first bytes from argument as interger
 */
LOCAL short read16 (unsigned char *buff)
{
	unsigned short val;

	val = buff [0] << 8 | buff [1];

	return val;
}
/* ------------------------------------------------------------------------- */
/** Read 32 bit integer from buffer
 *  @param buff the buffer to read from
 *  @return four first bytes from argument as interger
 */
LOCAL int read32 (unsigned char *buff)
{
	unsigned val;

	val = buff [0] << 24 | buff [1] << 16 | buff [2] << 8  | buff [3];

	return val;
}
/* ------------------------------------------------------------------------- */
/** Construct MIN Engine API protocol header.
 *  @param buff [out] the buffer header will be written to.
 *  @param msg_type Message type code.
 *  @param msg_len Message lenght field value.
 */
LOCAL void eapi_build_header (char *buff, char msg_type, 
			      unsigned short msg_len)
{
	buff [0] = msg_type;
	write16 (&buff [1], msg_len);
}
/* ------------------------------------------------------------------------- */
/** Send MIN Engine API message of type indication
 *  @param msg_type Message type code.
 *  @param msg_len Message lenght field value.
 *  @param buff Message buffer
 */
LOCAL void eapi_message_send_ind (char msg_type, 
				  unsigned short msg_len,
				  char *buff ) 
{
	eapi_build_header (buff, msg_type, msg_len);
	eapi_message_send (buff, msg_len + MIN_HDR_LEN);
}
/* ------------------------------------------------------------------------- */
/** Send MIN Engine API message of type response
 *  @param tid transaction identifier
 *  @param resp_code response code value
LOCAL void eapi_message_send_resp (unsigned tid, char resp_code)
{
	char *buff, *p;
	unsigned short msg_len = 5;

	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	p = buff + MIN_HDR_LEN;
	write32 (p, tid);
	p += 4;
	*p = resp_code;
	
	eapi_build_header (buff, MIN_RESP, msg_len);
	eapi_message_send (buff, msg_len + MIN_HDR_LEN);
}
 */

/* ------------------------------------------------------------------------- */
/** Send MIN Engine API protocol message.
 *  @param msg the message
 *  @len full lenght of message
 * 
 *  Stores the message to write_queue.
 */
LOCAL void eapi_message_send (char *msg, unsigned short len) 
{
	eapi_msg_container *cont;
	cont = NEW (eapi_msg_container);
	cont->msg_ = msg;
	cont->msg_len_ = len;
	pthread_mutex_lock (&tcpip_mutex_);
	dl_list_add (write_queue, cont);
	pthread_mutex_unlock (&tcpip_mutex_);
}

/* ------------------------------------------------------------------------- */
/** Dispatcher for MIN Engine API messages towards the Engine. 
 *  @param msg_type MIN protocol message type
 *  @param msg_len message len
 *  @param msg rest of message
 *
 *  Sends response after the message has been processed.
 */
LOCAL void eapi_message_receive (char msg_type, unsigned short msg_len,
	                         char *msg) 
{
	char *pos;
	unsigned tid = 0;
	int      res = 1;

	if (msg_len < MIN_MSG_LENGHT_MIN) {
		MIN_WARN ("Too short EAPI message %u", msg_len);
		return;
	}
	pos = msg;
	tid = read32 ((unsigned char *)pos);
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
		res = handle_fatal_err_req (pos, msg_len);
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
	
	/* eapi_message_send_resp (tid, res); */
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
	char *modpath;

	if (msg_len == 0)
		return ret;

	modpath = NEW2 (char, msg_len + 1);
	strncpy (modpath, msg, msg_len);
	modpath [msg_len] = '\0';

	ret = min_clbk.add_test_module (modpath);

	DELETE (modpath);

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
	unsigned moduleid;
	char *pos;
	char *casefile;
	
	if (msg_len < 5)
		return ret;
	
	pos = msg;

	moduleid = read32 ((unsigned char *)pos);
	pos += 4;
	
	casefile = NEW2 (char, msg_len - 4 + 1);
	strncpy (casefile, pos, msg_len - 4);
	casefile[msg_len - 4] = '\0';
	ret = min_clbk.add_test_case_file (moduleid, casefile);
	
	DELETE (casefile);
	
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
	unsigned char *pos;
	unsigned moduleid, caseid, groupid;
	
	pos = (unsigned char *)msg;

	moduleid = read32 (pos);
	pos += 4;
	caseid = read32 (pos);
	pos += 4;
	groupid = read32 (pos);

	ret = min_clbk.start_case (moduleid, caseid, groupid);
	
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
	unsigned run_id;

	run_id = read32 ((unsigned char *)msg);
	ret = min_clbk.pause_case (run_id);
	
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
	unsigned run_id;

	run_id = read32 ((unsigned char *)msg);
	ret = min_clbk.resume_case (run_id);

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
	unsigned test_run_id;

	test_run_id = read32 ((unsigned char *)msg);
	ret = min_clbk.abort_case (test_run_id);

	return ret;
}
/* ------------------------------------------------------------------------- */
/** Handles Fatal Error Request message.
 *  @param msg Message without the header
 *  @param msg_len length of message without the header
 *  @return the return value of engine callback (0/1), or MIN_PROTO_ERROR 
 *          in case of protocol error
 */
LOCAL int handle_fatal_err_req (char *msg, unsigned short msg_len)
{
	int ret = MIN_PROTO_ERROR;
	unsigned what_len;
	char *what, *err;
	char *pos;

	if (msg_len < 6)
		return ret;

	pos = msg;
	what_len = read32 ((unsigned char *)pos);
	pos += 4;
	if (what_len > (msg_len - 4 + 1))
		return ret;

	what = NEW2 (char, what_len + 1);
	strncpy (what, pos, what_len);
	what [what_len] = '\0';
	pos += what_len;
	
	err = NEW2 (char, msg_len - 4 - what_len + 1);
	strncpy (err, pos, msg_len - 4 - what_len);
	err [msg_len - 4 - what_len] = '\0';

	ret = min_clbk.fatal_error (what, err);
	
	DELETE (what);
	DELETE (err);

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
	MIN_DEBUG ("MIN Open Request");
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

	run = 0; /* let us exit */
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
	char *modules = INITPTR;

        ret = min_clbk.query_test_modules(&modules);
	pl_test_modules (modules);

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
	char *files = INITPTR;

        ret = min_clbk.query_test_files(&files);
	pl_test_files (files);
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
	p = buff + MIN_HDR_LEN;
	write32 (p, moduleid);
	p += 4;
	memcpy (p, modulename, strlen (modulename));

	eapi_message_send_ind (MIN_NEW_MOD_IND, 
			       msg_len,
			       buff);
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

	eapi_message_send_ind (MIN_MOD_READY_IND, 
			       msg_len + MIN_HDR_LEN,
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

	eapi_message_send_ind (MIN_CASE_STARTED_IND, 
			       msg_len,
			       buff);

}
/* ------------------------------------------------------------------------- */
/** Engine calls this when test case has been paused by user
 *  @param testrunid identifier for the test run
 */ 
LOCAL void pl_case_paused (long testrunid)
{
	unsigned short msg_len;
	char *p, *buff;
	msg_len = 4 + 4 + 4;
	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	
	p = &buff [MIN_HDR_LEN];
	write32 (p, testrunid);

	eapi_message_send_ind (MIN_CASE_PAUSED_IND, 
			       msg_len,
			       buff);

}
/* ------------------------------------------------------------------------- */
/** Engine calls this when a paused test case has been resumed
 *  @param testrunid identifier for the test run
 */ 
LOCAL void pl_case_resumed (long testrunid)
{
	unsigned short msg_len;
	char *p, *buff;
	msg_len = 4 + 4 + 4;
	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	
	p = &buff [MIN_HDR_LEN];
	write32 (p, testrunid);

	eapi_message_send_ind (MIN_CASE_RESUMED_IND, 
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
			   long starttime, long endtime)
{
	unsigned short msg_len;
	char *p, *buff;
	msg_len = 4 + 4 + 4 + 4 + strlen (desc);
	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	
	p = &buff [MIN_HDR_LEN];
	write32 (p, testrunid);
	p += 4;	
	write32 (p, result);
	p += 4;
	write32 (p, starttime);
	p += 4;
	write32 (p, endtime);
	p += 4;
	memcpy (p, desc, strlen (desc));
	
	eapi_message_send_ind (MIN_CASE_RESULT_IND, 
			       msg_len,
			       buff);
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when it when test case has sent print data
 *  @param testrunid identifier for the test run
 *  @param message the test case message
 */ 
LOCAL void pl_msg_print (long testrunid, char *message)
{
	unsigned short msg_len;
	char *p, *buff;

	msg_len = 4 + strlen (message);
	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	
	p = &buff [MIN_HDR_LEN];
	write32 (p, testrunid);
	p += 4;	
	memcpy (p, message, strlen (message));
	
	eapi_message_send_ind (MIN_PRINTOUT_IND, 
			       msg_len,
			       buff);
}
/* ------------------------------------------------------------------------- */
/** Engine calls this when it when test case/module sends error message
 *  @param error the message
 */ 
LOCAL void pl_error_report (char *error) 
{
	unsigned short msg_len;
	char *p, *buff;

	msg_len = strlen (error);
	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	p = &buff [MIN_HDR_LEN];
	memcpy (p, error, strlen (error));

	eapi_message_send_ind (MIN_ERROR_REPORT_IND, 
			       msg_len,
			       buff);
}
/* ------------------------------------------------------------------------- */
/** Called to send list of modules found in Engine search paths to client 
 *  @param modulelist list of modules separated by '\0'
 */
LOCAL void pl_test_modules (char *modulelist)
{
	char *p, *pos, *buff;
	unsigned msg_len = 0, module_count = 0;

	/*
	** Calculate the total lenght needed
	*/
	p = &modulelist[0];
	while (*p != '\0') {
		module_count++;
		msg_len += strlen(p);
		p += strlen(p);
		p++;
	}
	if (module_count == 0)
		return;

	msg_len += module_count * 4; /* space for lenght fields */

	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	pos = &buff [MIN_HDR_LEN];
	/*
	** Loop again, this time write to message 
	*/
	p = &modulelist[0];
	while (*p != '\0') {
		write32 (pos, strlen(p));
		pos += 4;
		strncpy (pos, p, strlen(p));
		pos += strlen (p);
		p += strlen(p);
		p++;
	}
	
	eapi_message_send_ind (MIN_MODULE_LIST_IND, 
			       msg_len,
			       buff);
}
/* ------------------------------------------------------------------------- */
/** Called to send list of test case files found in Engine search paths 
 *  to client.
 *  @param modulelist list of modules separated by '\0'
 */
LOCAL void pl_test_files (char *files)
{
	char *p, *pos, *buff;
	unsigned msg_len = 0, file_count = 0;

	/*
	** Calculate the total lenght needed
	*/
	p = &files[0];
	while (*p != '\0') {
		file_count++;
		msg_len += strlen(p);
		p += strlen(p);
		p++;
	}
	if (file_count == 0)
		return;

	msg_len += file_count * 4; /* space for lenght fields */

	buff = NEW2 (char, MIN_HDR_LEN + msg_len);
	pos = &buff [MIN_HDR_LEN];
	/*
	** Loop again, this time write to message 
	*/
	p = &files[0];
	while (*p != '\0') {
		write32 (pos, strlen(p));
		pos += 4;
		strncpy (pos, p, strlen(p));
		pos += strlen (p);
		p += strlen(p);
		p++;
	}
	
	eapi_message_send_ind (MIN_FILE_LIST_IND, 
			       msg_len,
			       buff);
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
        (*out_callback)->case_paused            = pl_case_paused;
        (*out_callback)->case_resumed           = pl_case_resumed;
        (*out_callback)->module_prints          = pl_msg_print;
        (*out_callback)->new_module             = pl_new_module;
        (*out_callback)->no_module              = pl_no_module;
        (*out_callback)->module_ready           = pl_module_ready;
        (*out_callback)->new_case               = pl_new_case;
        (*out_callback)->error_report           = pl_error_report;

	(*out_callback)->test_modules           = pl_test_modules; 
	(*out_callback)->test_files             = pl_test_files; 

        return;
}
/* ------------------------------------------------------------------------- */
/** Plugin open function. 
 *  @param opts contains options struct for TCP/IP plugin
 *
 *  Read & write EAPI Protocol messages and act accordingly, until explicitly
 *  closed with MIN_CLOSE_REQ or the connection breaks.
 */
void pl_open_plugin (void *opts)
{
	tcpip_opts *pl_opts;
	fd_set rd, wr, er;
	struct timeval tv;
        int ret;

        min_log_open ("tpcipPlugin", 3);

	pl_opts = (tcpip_opts *)opts;
	write_queue = dl_list_create();
	fd = pl_opts->fd_;
	MIN_DEBUG ("fd = %d", fd);
	while (fd > 0 && run == 1) {
		FD_ZERO (&rd);
		FD_ZERO (&wr);
		FD_ZERO (&er);
		FD_SET (fd, &rd);
		FD_SET (fd, &wr);
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		ret = select (fd + 1, &rd, &wr, &er, &tv);
		if (ret == -1) {
			MIN_WARN ("select() error %s", strerror (errno));
			break;
		}
		if (FD_ISSET (fd, &rd))
			read_from_socket (fd);
		if (FD_ISSET (fd, &wr))
		        write_to_socket (fd);
		usleep (100000);
	}
	if (run == 1)
		ret = min_clbk.min_close();
	
	if (fd > 0)
		close(fd);

	fd = -1;

        return;
}
/* ------------------------------------------------------------------------- */
