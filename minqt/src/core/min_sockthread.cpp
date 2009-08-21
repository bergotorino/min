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
 * @file:     min_sockthread.cpp
 * @version:  1.00
 * @date:     20.08.2009
 * @author:   
 */

// Module include
#include "min_sockthread.hpp"
#include "min_remotecontroll.hpp"
#include "min_eapi_protocol.h"
// System includes
#include <QDate>
#include <QTcpSocket>
#include <arpa/inet.h>

// -----------------------------------------------------------------------------
Min::SocketThread::SocketThread(int fd, QObject *parent)
  : QThread(parent)
  , fd_(fd)
{ ; }

Min::SocketThread::~SocketThread()
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
void Min::SocketThread::sendOpenReq ()
{
	QByteArray *msg = new QByteArray();
	msg->resize (7);
	
	eapi_build_header (msg->data(), MIN_OPEN_REQ, 4);

	writeQueue_.append(msg);
}
// -----------------------------------------------------------------------------
void Min::SocketThread::readFromSock(QTcpSocket *sock)
{
	/* Read message type */
	char msg_type;
	if (!sock->getChar (&msg_type)) 
		return;
	/* read message len */
	char len_buff [2];
	sock->read (len_buff, 2);
	qint64 msg_len = len_buff [0] << 8 | len_buff [1];

	/* read the rest of message */
	QByteArray tmp, msg = sock->read (msg_len);
	tmp = msg.toHex();
	qDebug ("RECEIVED: %s", tmp.data());
	
	/* act according to message type */
	switch (msg_type) {
		uint module_id, case_id;
		int run_id, result, starttime, endtime;
		char *module_name, *case_title, *desc;
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
		emit (min_case_result (run_id, result, desc, 
				       starttime, endtime));
		
		break;
	}


  
}
// -----------------------------------------------------------------------------
void Min::SocketThread::run()
{
	
	QTcpSocket *tcpsock = new QTcpSocket();
	 
	if (!tcpsock->setSocketDescriptor(fd_)) {
		// emit error(sock.error());
		return;
	}
	tcpsock->setReadBufferSize(0);
	sendOpenReq ();
	Min::RemoteControll &rc = Min::RemoteControll::getInstance();
	qDebug ("SocketThread::run(): here - 3");
	
	while (tcpsock->state() == QAbstractSocket::ConnectedState) {
		 tcpsock->waitForReadyRead(1000);
		 if (tcpsock->bytesAvailable () > MIN_HDR_LEN) {
			 readFromSock(tcpsock);
		 }
		 /** if write queue */
		 if (!writeQueue_.isEmpty()) {
			 QByteArray *msg = writeQueue_.first();
			 tcpsock->write(*msg);
			 QByteArray tmp = msg->toHex();
			 qDebug ("SENT: %s", tmp.data());
			 
			 writeQueue_.removeFirst();
		 }
	}
}
// -----------------------------------------------------------------------------
void Min::SocketThread::min_abort_case(int testrunid)
{
	return;
}
// -----------------------------------------------------------------------------
void Min::SocketThread::min_add_test_case_file(uint moduleid, 
			    const QString &testcasefile)
{
	return;
}
// -----------------------------------------------------------------------------
void Min::SocketThread::min_add_test_module(const QString &modulepath)
{
	return;
}
// -----------------------------------------------------------------------------
void Min::SocketThread::min_close()
{
	return;
}
void Min::SocketThread::min_fatal_error()
{
	return;
}
// -----------------------------------------------------------------------------
void Min::SocketThread::min_open()
{
	return;
}
// -----------------------------------------------------------------------------
void Min::SocketThread::min_pause_case(int testrunid)
{
	return;
}
// -----------------------------------------------------------------------------
void Min::SocketThread::min_query_test_files()
{
	return;
}
// -----------------------------------------------------------------------------
void Min::SocketThread::min_query_test_modules()
{
	return;
}
void Min::SocketThread::min_resume_case (int testrunid)
{
	return;
}
// -----------------------------------------------------------------------------
void Min::SocketThread::min_start_case (uint moduleid, 
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

	return;
}
