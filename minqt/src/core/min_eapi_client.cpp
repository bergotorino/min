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
 * @file:     min_eapi_client.cpp
 * @version:  1.00
 * @date:     25.08.2009
 * @author:   
 */

// Module include
#include "min_eapi_client.hpp"
#include "min_remotecontroll.hpp"
#include "min_eapi_protocol.h"
// System includes
#include <QDate>
#include <QTcpSocket>
#include <arpa/inet.h>

// -----------------------------------------------------------------------------
Min::EapiClient::EapiClient(QTcpSocket *s)
	:  sock (s)
{ }

Min::EapiClient::~EapiClient()
{ ; }
// -----------------------------------------------------------------------------
static int read32 (QByteArray *msg)
{
	unsigned char *buff = (unsigned char *)msg->data();

	int val = 
		buff [0] << 24 | buff [1] << 16 | buff [2] << 8  | buff [3];
	
	return val;
}
// -----------------------------------------------------------------------------
static void write32 (char *buff, int val)
{
	buff [0] = val >> 24;
	buff [1] = val >> 16;
	buff [2] = val >> 8;
	buff [3] = val;
}
// -----------------------------------------------------------------------------
static void write16 (char *buff, int val)
{
	buff [0] = val >> 8;
	buff [1] = val;
}
// -----------------------------------------------------------------------------
static void eapi_build_header (char *buff, char msg_type, 
			       unsigned short msg_len)
{
	buff [0] = msg_type;
	write16 (&buff[1], msg_len);
}
// -----------------------------------------------------------------------------
void Min::EapiClient::readFromSock()
{
 again:
        qDebug ("Bytes Available = %u", (unsigned)sock->bytesAvailable());
	if (sock->bytesAvailable() <= 0)
		return;
	/* Read message type */
	char msg_type;
	if (!sock->getChar (&msg_type)) { 
		qDebug ("can't read even 1 byte");
		return;
	}
	/* read message len */
	unsigned char len_buff [2];
	sock->read ((char *)len_buff, 2);
	qint64 msg_len = len_buff [0] << 8 | len_buff [1];

	/* read the rest of message */
	QByteArray tmp, msg = sock->read (msg_len);
	tmp = msg.toHex();
	qDebug ("RECEIVED(%u): %s", (unsigned)msg_len, tmp.data());
	
	/* act according to message type */
	switch (msg_type) {
		uint module_id, case_id, tid, len;
		int run_id, result, starttime, endtime;
		char *module_name, *case_title, *desc, *printout, *case_desc;
	case MIN_NEW_MOD_IND:
		qDebug ("EAPI: New Module indication");
		module_id = read32 (&msg);
		msg.remove(0, 4);
		module_name = msg.data();
		qDebug ("moduleid=%u modulename=%s", 
			module_id, module_name);
		emit (min_new_module(module_name, module_id));
		break;
	case MIN_NEW_CASE_IND:
		qDebug ("EAPI: New Case indication");
		module_id = read32 (&msg);
		msg.remove(0, 4);
		case_id = read32 (&msg);
		msg.remove(0, 4);
		case_title = msg.data();
		emit (min_new_test_case(module_id, case_id, case_title));
		break;
	case MIN_MOD_READY_IND:
		qDebug ("EAPI: Module Ready indication");
		module_id = read32 (&msg);
		msg.remove(0, 4);
		emit (min_module_ready(module_id));
		break;
	case MIN_CASE_STARTED_IND:
		qDebug ("EAPI: Case Started indication");
		module_id = read32 (&msg);
		msg.remove(0, 4);
		case_id = read32 (&msg);
		msg.remove(0, 4);
		run_id =  read32 (&msg);
		msg.remove(0, 4);
		emit (min_case_started (module_id, case_id, run_id));
		break;
	case MIN_CASE_PAUSED_IND:
		qDebug ("EAPI: Case Paused indication");
		run_id =  read32 (&msg);
		emit (min_case_paused (run_id));
		break;
	case MIN_CASE_RESUMED_IND:
		qDebug ("EAPI: Case Resumed indication");
		run_id =  read32 (&msg);
		emit (min_case_resumed (run_id));
		break;
	case MIN_CASE_RESULT_IND:
		qDebug ("EAPI: Case Result indication");
		run_id =  read32 (&msg);
		msg.remove(0, 4);
		result = read32 (&msg);
		msg.remove(0, 4);
		starttime = read32 (&msg);
		msg.remove(0, 4);
		endtime = read32 (&msg);
		msg.remove(0, 4);
		desc = msg.data();
		qDebug ("run_id=%u, result=%d, starttime=%d,"
			"endttime=%d, desc=%s",
			run_id, result, starttime, endtime, desc);
		emit (min_case_result (run_id, result, desc, 
				       starttime, endtime));
		
		break;
	case MIN_PRINTOUT_IND:
		qDebug ("EAPI: Printout indication");
		run_id =  read32 (&msg);
		msg.remove(0, 4);
		printout = msg.data();
		qDebug ("run_id=%u, printout=%s",
			run_id, printout);
		emit (min_case_msg (run_id, printout));
		break;
	case MIN_RESP:
		qDebug ("EAPI: Response");
		tid = read32 (&msg);
		msg.remove(0, 4);
		qDebug ("transaction id = %u", tid);
		break;
	case MIN_MODULE_LIST_IND:
		qDebug ("EAPI: Module List Indication");
		
		while (!msg.isEmpty()) {
			len = read32 (&msg);
			msg.remove (0, 4);
			tmp = msg.mid(0, len);
			emit (min_test_modules(tmp.data()));
			msg.remove (0, len);
		} 
		
		break;
	case MIN_FILE_LIST_IND:
		qDebug ("EAPI: File List Indication");
		while (!msg.isEmpty()) {
			len = read32 (&msg);
			msg.remove (0, 4);
			tmp = msg.mid(0, len);
			emit (min_test_files(tmp.data()));
			msg.remove (0, len);
		} 
		break;
	case MIN_ERROR_REPORT_IND:
		qDebug ("EAPI: Error Report Indication");
		printout = msg.data();
		emit (min_error_report (printout));
		break;

	case MIN_CASE_DESC_IND:
		qDebug ("EAPI: Case Description indication");
		module_id = read32 (&msg);
		msg.remove(0, 4);
		case_id = read32 (&msg);
		msg.remove(0, 4);
		case_desc = msg.data();
		emit (min_case_desc(module_id, case_id, case_desc));
		break;

	default:
		qDebug ("Unknow EAPI message type %02x", msg_type);
		break;
		
	}
	if (sock->bytesAvailable())
	        goto again;
  
}
// -----------------------------------------------------------------------------
void Min::EapiClient::sendToSock()
{
	QByteArray *msg = writeQueue_.first();
	sock->write(*msg);
	sock->waitForBytesWritten();
	QByteArray tmp = msg->toHex();
	qDebug ("SENT: %s", tmp.data());
	
	writeQueue_.removeFirst();

}
// -----------------------------------------------------------------------------
void Min::EapiClient::init()
{
	sock->setReadBufferSize(0);

}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_abort_case(int testrunid)
{
	QByteArray *msg = new QByteArray();
	unsigned msg_len = 4 + 4;
	char *p;

	msg->resize (MIN_HDR_LEN + msg_len);
	p = msg->data();
	eapi_build_header (p, MIN_ABORT_CASE_REQ, msg_len);
	p += MIN_HDR_LEN;
	write32 (p, 0); /* tid */
	p += 4;
	write32 (p, testrunid);

	writeQueue_.append(msg);
	sendToSock();
	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_add_test_case_file(uint moduleid, 
					     const QString &testcasefile)
{
	QByteArray *msg = new QByteArray();
	unsigned msg_len = 4 + 4 + testcasefile.size();
	char *p;
	
	msg->resize (MIN_HDR_LEN + msg_len);
	p = msg->data();
	eapi_build_header (p, MIN_ADD_CASE_FILE_REQ, msg_len);
	p += MIN_HDR_LEN;
	write32 (p, 0); 
	p += 4;
	write32 (p, moduleid);
	msg->insert (MIN_HDR_LEN + 8, testcasefile);

	writeQueue_.append(msg);
	sendToSock();
	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_add_test_module(const QString &modulepath)
{
	QByteArray *msg = new QByteArray();
	unsigned msg_len = 4 + modulepath.size();
	char *p;
	
	msg->resize (MIN_HDR_LEN + msg_len);
	p = msg->data();
	eapi_build_header (p, MIN_ADD_MOD_REQ, msg_len);
	p += MIN_HDR_LEN;
	write32 (p, 0); 
	msg->insert (MIN_HDR_LEN + 4, modulepath);

	writeQueue_.append(msg);
	sendToSock();
	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_close()
{
	QByteArray *msg = new QByteArray();
	unsigned msg_len = 4;
	char *p;

	msg->resize (MIN_HDR_LEN + msg_len);
	p = msg->data();
	eapi_build_header (p, MIN_CLOSE_REQ, msg_len);
	p += MIN_HDR_LEN;
	write32 (p, 0); /* tid */

	writeQueue_.append(msg);
	sendToSock();

	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_fatal_error(const QString &what, 
				      const QString &errorstring)
{
	QByteArray *msg = new QByteArray();
	unsigned msg_len = 4 + 4 + what.size() + errorstring.size();
	char *p;

	msg->resize (MIN_HDR_LEN + msg_len);
	p = msg->data();
	eapi_build_header (p, MIN_FATAL_ERR_REQ, msg_len);
	p += MIN_HDR_LEN;
	write32 (p, 0); /* tid */
	p += 4;
	write32 (p, what.size());
	msg->insert (MIN_HDR_LEN + 8, what);
	msg->insert (MIN_HDR_LEN + 8 + what.size(), errorstring);

	writeQueue_.append(msg);
	sendToSock();
	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_open()
{
	QByteArray *msg = new QByteArray();
	msg->resize (7);
	
	eapi_build_header (msg->data(), MIN_OPEN_REQ, 4);

	writeQueue_.append(msg);
	sendToSock();

	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_pause_case(int testrunid)
{
	QByteArray *msg = new QByteArray();
	unsigned msg_len = 4 + 4;
	char *p;

	qDebug ("Eapi: pause case %d", testrunid);
	msg->resize (MIN_HDR_LEN + msg_len);
	p = msg->data();
	eapi_build_header (p, MIN_PAUSE_CASE_REQ, msg_len);
	p += MIN_HDR_LEN;
	write32 (p, 0); /* tid */
	p += 4;
	write32 (p, testrunid);

	writeQueue_.append(msg);
	sendToSock();

	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_query_test_files()
{
	QByteArray *msg = new QByteArray();
	msg->resize (7);
	
	eapi_build_header (msg->data(), MIN_FILE_QUERY_REQ, 4);

	writeQueue_.append(msg);
	sendToSock();

	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_query_test_modules()
{
	QByteArray *msg = new QByteArray();
	msg->resize (7);
	
	eapi_build_header (msg->data(), MIN_MOD_QUERY_REQ, 4);

	writeQueue_.append(msg);
	sendToSock();


	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_resume_case (int testrunid)
{
	QByteArray *msg = new QByteArray();
	unsigned msg_len = 4 + 4;
	char *p;

	msg->resize (MIN_HDR_LEN + msg_len);
	p = msg->data();
	eapi_build_header (p, MIN_RESUME_CASE_REQ, msg_len);
	p += MIN_HDR_LEN;
	write32 (p, 0); /* tid */
	p += 4;
	write32 (p, testrunid);

	writeQueue_.append(msg);
	sendToSock();
	return;
}
// -----------------------------------------------------------------------------
void Min::EapiClient::min_start_case (uint moduleid, 
				      uint caseid, 
				      uint groupid)
{
	QByteArray *msg = new QByteArray();
	unsigned msg_len = 4 + 4 + 4 + 4;
	char *p;

	msg->resize (MIN_HDR_LEN + msg_len);
	p = msg->data();
	eapi_build_header (p, MIN_START_CASE_REQ, msg_len);
	p += MIN_HDR_LEN;
	write32 (p, 0); /* tid */
	p += 4;
	write32 (p, moduleid);
	p += 4;
	write32 (p, caseid);
	p += 4;
	write32 (p, groupid);

	writeQueue_.append(msg);
	sendToSock();

	return;
}
