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
 * @file:     min_sockthread.hpp
 * @version:  1.00
 * @date:     20.09.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_SOCKTHREAD_HPP
#define INCLUDED_MIN_SOCKTHREAD_HPP

// System includes
#include <QThread>
#include <QTcpSocket>

// Min includes
#include "min_object.h"
#include "min_singleton.hpp"

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class SocketThread
     * @brief Handles tcp traffic between MinQt GUI and MIN 
     *
     */
	class SocketThread: public QThread
	{
		/** Copying is forbidden */
		Q_DISABLE_COPY(SocketThread);
		Q_OBJECT
		public:

		/** Destructor. */
		~SocketThread();
		/** Constructor */
		SocketThread (int fd, QObject *parent);
		void run();
		void min_abort_case(int testrunid);
		void min_add_test_case_file(uint moduleid, 
					    const QString &testcasefile);
		void min_add_test_module(const QString &modulepath);
		void min_close();
		void min_fatal_error();
		void min_open();
		void min_pause_case(int testrunid);
		void min_query_test_files();
		void min_query_test_modules();
		void min_resume_case(int testrunid);
		void min_start_case(uint moduleid, uint caseid, uint groupid);

	signals:
		void min_case_msg(int testrunid, const QString &message);
		void min_case_paused(int testrunid);
		void min_case_result(int testrunid, 
				     int result, 
				     const QString &desc, 
				     int starttime, 
				     int endttime);
		void min_case_resumed(int testrunid);
		void min_case_started(uint moduleid, 
				      uint caseid, 
				      int testrunid);
		void min_module_ready(uint moduleid);
		void min_new_module(const QString &modulename, 
				    uint moduleid);
		void min_new_test_case(uint moduleid, 
				       uint caseid, 
				       const QString &casetitle);
		void min_no_module(const QString &modulename);
		void min_test_files(const QString &files);
		void min_test_modules(const QString &modules);

	public slots:
			
	private slots:
		
	private:
		/** socket file descriptor */
		int fd_;
		
		/** transaction id */
		int tid_;
		/** List implementing socket write queue
		 */
		QList<QByteArray*> writeQueue_;
		void readFromSock(QTcpSocket *sock);
		void sendOpenReq();
		
	};
   // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_SOCKTHREAD_HPP

