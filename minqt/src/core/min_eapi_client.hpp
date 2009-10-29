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
 * @file:     min_eapi_client.hpp
 * @version:  1.00
 * @date:     25.08.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_EAPI_CLIENT_HPP
#define INCLUDED_MIN_EAPI_CLIENT_HPP

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
     * @class EapiClient
     * @brief Handles MIN Engine API protocol client 
     *
     */
	class EapiClient : public QObject
	{
		/** Copying is forbidden */
		Q_DISABLE_COPY(EapiClient);
		Q_OBJECT
		public:

		/** Destructor. */
		~EapiClient();
		/** Constructor */
		EapiClient (QTcpSocket *s);
		void init();
		void min_abort_case(int testrunid);
		void min_add_test_case_file(uint moduleid, 
					    const QString &testcasefile);
		void min_add_test_module(const QString &modulepath);
		void min_close();
		void min_fatal_error(const QString &what, 
				     const QString &errorstring);
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
		void min_error_report(const QString &error);

	public slots:
			
	private slots:
		void readFromSock();
		void sendToSock();

	private:
		/** Socket file descriptor */
		int fd_;
		/** Socket file descriptor */
		QTcpSocket *sock;
		/** List implementing socket write queue
		 */
		QList<QByteArray*> writeQueue_;
		
	};
   // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_EAPI_CLIENT_HPP

