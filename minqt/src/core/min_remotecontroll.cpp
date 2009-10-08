/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Konrad Marek Zapalowicz
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
 * @file:     min_remotecontroll.cpp
 * @version:  1.00
 * @date:     27.03.2009
 * @author:   
 */

// Module include
#include "min_remotecontroll.hpp"
#include "min_eapi_client.hpp"
// System includes
#include <QDate>
#include <QTcpSocket>
#include <arpa/inet.h>

// Min includes
#include "min_database.hpp"
#include "min_statusbarprovider.hpp"
#include "min_descriptionprovider.hpp"
#include "min_common.h"
#include "tmc_common.h"

// -----------------------------------------------------------------------------
Min::RemoteControll::RemoteControll()
    : obj_(0)
    , testCaseFiles_()
    , exeRequest_()
    , sock_(NULL)
    , remote_(false)
    , closed_(true)
{ ; }
// -----------------------------------------------------------------------------
Min::RemoteControll::~RemoteControll()
{
    if (!closed_&&obj_) obj_->min_close();
    if (obj_) delete obj_;
}
// -----------------------------------------------------------------------------
bool Min::RemoteControll::isValid() const
{
    //if (obj_->isValid()) return true;
    //else return false;
    return true;
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::handleSockError(QAbstractSocket::SocketError socketError)
 {
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
	 qDebug ("Remote host closed");
         break;
     case QAbstractSocket::HostNotFoundError:
	 qDebug ("Host not found");
         break;
     case QAbstractSocket::ConnectionRefusedError:
	 qDebug ("Connection refused");
         break;
     default:
	 qDebug ("Error: %s", sock_->errorString().toStdString().c_str());

     }
 }

void Min::RemoteControll::handleSockConnected()
{
	qDebug ("Socket connected succesfully");
	eapiClient_ = new EapiClient (sock_, this);
	connect (eapiClient_,SIGNAL(min_case_msg(int, const QString &)),
		 this,SLOT(minCaseMsg(int, const QString &)));
	  
	connect (eapiClient_,SIGNAL(min_case_paused(int)),
		 this,SLOT(minCasePaused(int)));
	
	connect (eapiClient_,SIGNAL(min_case_result(int, int, const QString &,
					     int, int)),
		 this,SLOT(minCaseResult(int, int, const QString &,
					 int, int)));
	connect (eapiClient_,SIGNAL(min_case_resumed(int)),
		 this,SLOT(minCaseResumed(int)));
	
	connect (eapiClient_,SIGNAL(min_case_started(uint, uint, int)),
		 this,SLOT(minCaseStarted(uint, uint, int)));
	
	connect (eapiClient_,SIGNAL(min_module_ready(uint)),
		 this,SLOT(minModuleReady(uint)));
	
	connect (eapiClient_,SIGNAL(min_new_module(const QString &, uint)),
		 this,SLOT(minNewModule(const QString &, uint)));
	
	connect (eapiClient_,SIGNAL(min_new_test_case(uint, uint, const QString &)),
		 this,SLOT(minNewTestCase(uint, uint, const QString &)));
	
	connect (eapiClient_,SIGNAL(min_no_module(const QString &)),
		 this,SLOT(minNoModule(const QString &)));
	
	connect (eapiClient_,SIGNAL(min_test_files(const QString &)),
		 this,SLOT(minTestFiles(const QString &)));
	
	connect (eapiClient_,SIGNAL(min_test_modules(const QString &)),
		 this,SLOT(minTestModules(const QString &)));

	connect (eapiClient_,SIGNAL(min_error_report(const QString &)),
		 this,SLOT(minErrorReport(const QString &)));

	connect (sock_, SIGNAL(readyRead()), eapiClient_,
		 SLOT(readFromSock()));

	eapiClient_->init();
	eapiClient_->min_open();
	eapiClient_->min_query_test_files();
	eapiClient_->min_query_test_modules();

	return;
}

// -----------------------------------------------------------------------------
void Min::RemoteControll::open(const QString &address)
{
    if (address=="") return;

    // Open connection to the DBus
    if (address.contains("localhost") ||
	address.contains("127.0.0.1")) {
	  if (address.contains(':')) {
	    QString dbusAddress = address;
	    dbusAddress.replace(":",",port=");
	    dbusAddress.prepend("tcp:host=");
	    QDBusConnection bus =
	      QDBusConnection::connectToBus(dbusAddress,
                                            "org.maemo.MIN");
	    if (!bus.isConnected()) {
	      qDebug("Bus Error: %s",bus.lastError().message().toStdString().c_str());
	      return;
        }
	    // 3. Open plugin
	    obj_= new MinObject("org.maemo.MIN","/Min",bus);
	  } else {
	    QDBusConnection bus =
	      QDBusConnection::connectToBus(QDBusConnection::SessionBus,
                                            "org.maemo.MIN");
	    if (!bus.isConnected()) {
	      qDebug("Bus Error: %s",bus.lastError().message().toStdString().c_str());
	      return;
	    }
	    // 3. Open plugin
	    obj_= new MinObject("org.maemo.MIN","/Min",bus);
	  }
	  
	  // 3.2. Connect signals
	  connect (obj_,SIGNAL(min_case_msg(int, const QString &)),
		   this,SLOT(minCaseMsg(int, const QString &)));
	  
	  connect (obj_,SIGNAL(min_case_paused(int)),
		   this,SLOT(minCasePaused(int)));
	  
	  connect (obj_,SIGNAL(min_case_result(int, int, const QString &,
                                            int, int)),
		   this,SLOT(minCaseResult(int, int, const QString &,
					   int, int)));
	  
	  connect (obj_,SIGNAL(min_case_resumed(int)),
		   this,SLOT(minCaseResumed(int)));
	  
	  connect (obj_,SIGNAL(min_case_started(uint, uint, int)),
		   this,SLOT(minCaseStarted(uint, uint, int)));
	  
	  connect (obj_,SIGNAL(min_module_ready(uint)),
		   this,SLOT(minModuleReady(uint)));
	  
	  connect (obj_,SIGNAL(min_new_module(const QString &, uint)),
		   this,SLOT(minNewModule(const QString &, uint)));
	  
	  connect (obj_,SIGNAL(min_new_test_case(uint, uint, const QString &)),
		   this,SLOT(minNewTestCase(uint, uint, const QString &)));
	  
	  connect (obj_,SIGNAL(min_no_module(const QString &)),
		   this,SLOT(minNoModule(const QString &)));
	  
	  connect (obj_,SIGNAL(min_test_files(const QString &)),
		   this,SLOT(minTestFiles(const QString &)));
	  
	  connect (obj_,SIGNAL(min_test_modules(const QString &)),
		   this,SLOT(minTestModules(const QString &)));

	  connect (obj_,SIGNAL(min_error_report(const QString &)),
		   this,SLOT(minErrorReport(const QString &)));


	  // 3.3 Open MinDBusPlugin
	  obj_->min_open();
	  return;
	}
	// Open connection to remote entity
	remote_ = true;
	sock_ = new QTcpSocket();
	connect(sock_, SIGNAL(connected()),
		this, SLOT(handleSockConnected()));
	connect(sock_, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(handleSockError(QAbstractSocket::SocketError)));

	sock_->abort();
	sock_->connectToHost (address, MIN_EAPI_LISTEN_PORT);


}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCaseMsg(int testrunid,
                                    const QString &message)
{
    //qDebug("Min::RemoteControll::minCaseMsg\n",testrunid);
    Min::Database &db = Min::Database::getInstance();
    db.insertPrintout (db.getTestRunDbId(testrunid),message);

    // Display stuff on status bar
    Min::StatusBar::update("Recieved message from test case: "+message,3000);
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCasePaused(int testrunid)
{
    qDebug("Min::RemoteControll::minCasePaused\n");

    Min::Database &db = Min::Database::getInstance();
    db.updateTestRun(db.getTestRunDbId(testrunid),
		     TP_PAUSED,   // status
		     0,  
		     0,
		     0,     
		     NULL);

    // Display stuff on status bar
    QString tcase = db.getCaseTitleFromTestrunDbId(db.getTestRunDbId(testrunid));
    Min::StatusBar::update("Running test case has been paused: "+tcase,3000);
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCaseResult(int testrunid, int result,
                                        const QString &desc,
                                        int starttime, int endtime)
{
    unsigned int dbid;
    Min::Database &db = Min::Database::getInstance();
    int status = TP_ENDED;
    switch (result) {
    case TP_NC:
	    status = TP_CANCELED;
	    break;
    case TP_CRASHED:
	    status = TP_ABORTED;
	    break;
    default:
	    break;
    }
    dbid = db.getTestRunDbId(testrunid);
    if (dbid == 0) {
	    qDebug  ("minCaseResult: dbid == 0 .. bad");
	    return;
    }

    db.updateTestRun(dbid,
		     status, 
		     starttime,  
		     endtime,
		     result,     
		     desc);

    // Display stuff on status bar
    QString msg = db.getCaseTitleFromTestrunDbId(db.getTestRunDbId(testrunid));
    msg += " result: ";
    msg += Min::DescriptionProvider::getTestCaseResultDescription(result);
    Min::StatusBar::update(msg,3000);
    db.insertLogMessage ("info",msg);
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCaseResumed(int testrunid)
{
    //qDebug("Min::RemoteControll::minCaseResumed\n");

    Min::Database &db = Min::Database::getInstance();
    db.updateTestRun(db.getTestRunDbId(testrunid),
		     TP_RUNNING,   // status
		     0,  
		     0,
		     0,     
		     NULL);

    // Display stuff on status bar
    QString tcase = db.getCaseTitleFromTestrunDbId(db.getTestRunDbId(testrunid));
    Min::StatusBar::update("Paused test case has been resumed: "+tcase,3000);
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCaseStarted(uint moduleid,
					 uint caseid,
					 int testrunid)
{
    //qDebug("Min::RemoteControll::minCaseStarted\n");
    Min::Database &db = Min::Database::getInstance();
    //qDebug ("minCaseStarted: moduleid=%u, caseid = %u, testrunid = %d,"
    //    "requestCount=%d",
    //    moduleid, caseid, testrunid, exeRequest_.count());
    
    for (int i=0;i<exeRequest_.count();i++) {
        Min::RemoteControll::ExeRequestData *tmp;
        tmp = exeRequest_[i];
        if (tmp->moduleid_==moduleid&&tmp->caseid_==caseid) {
            db.insertTestRun(db.getTestCaseDbId(db.getModuleDbId(1,moduleid),
						caseid),
			     testrunid, 
			     tmp->groupid_, // groupid
			     TP_RUNNING,    // status
			     QDateTime::currentDateTime().toTime_t()// start time
			     );
            delete tmp;
	    //qDebug ("minCaseStarted: removing request index %d", i);
            exeRequest_.removeAt(i);
	    break;
        }
    }

    // Display stuff on status bar
    QString msg = "Started test case execution: ";
    msg += db.getModuleNameFromEngineId(moduleid);
    msg += "::";
    msg += db.getCaseTitleFromEngineId(db.getModuleDbId(1,moduleid),caseid);
    Min::StatusBar::update(msg,3000);
    db.insertLogMessage ("info",msg);

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minModuleReady(uint moduleid)
{
    //qDebug("Min::RemoteControll::minModuleReady %d\n",moduleid);

    // Display stuff on status bar
    Min::Database &db = Min::Database::getInstance();
    QString tmp = db.getModuleNameFromEngineId(moduleid);
    Min::StatusBar::update("Module ready: "+tmp,3000);
    db.insertLogMessage ("info","Module ready: "+tmp);

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minNewModule(const QString &modulename, uint moduleid)
{
    qDebug("Min::RemoteControll::minNewModule %d\n",moduleid);
    Min::Database::getInstance().insertModule (1,moduleid,modulename);

    // If module has been added by "Add module functionality" list of test 
    // cases will be not empty
    if (!testCaseFiles_.count()) return;

    // First item is the path to the modulename
    if (testCaseFiles_[0]==modulename) {
        for (int i=1;i<testCaseFiles_.count();i++) {
            minAddTestCaseFile(moduleid,testCaseFiles_[i]);
        }
    }
    minAddTestCaseFile(moduleid,"");
    testCaseFiles_.clear();

    // Display stuff on status bar
    Min::StatusBar::update("Module loaded: "+modulename,3000);
    Min::Database &db = Min::Database::getInstance();
    db.insertLogMessage ("info","Module loaded: "+modulename);
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minNewTestCase(uint moduleid, uint caseid,
                    const QString &casetitle)
{
	qDebug("Min::RemoteControll::minNewTestCase %d %d %s\n",
	       moduleid,caseid,casetitle.toStdString().c_str());
	Min::Database &db = Min::Database::getInstance();
	db.insertTestCase(db.getModuleDbId(1,moduleid),caseid,casetitle,"");
	
	// Display stuff on status bar
	Min::StatusBar::update("New test case: "+casetitle,3000);
	db.insertLogMessage ("info","New test case: "+casetitle);
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minNoModule(const QString &modulename)
{
	qDebug("Min::RemoteControll::minNoModule\n");

	// Display stuff on status bar
	Min::StatusBar::update("Unable to load module: "+modulename,3000);
	Min::Database &db = Min::Database::getInstance();

	db.insertLogMessage ("error","Unable to load module: "+modulename);

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minTestFiles(const QString &files)
{
	qDebug("Min::RemoteControll::minTestFiles %s\n",
	       files.toStdString().c_str());
	
	Min::Database &db = Min::Database::getInstance();
	db.insertTestCaseFile (files);
	
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minTestModules(const QString &modules)
{
	qDebug("Min::RemoteControll::minTestModules %s\n",
	       modules.toStdString().c_str());
	Min::Database &db = Min::Database::getInstance();
	db.insertTestCaseFile (modules);
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minErrorReport(const QString &error)
{

	qDebug ("Min::RemoteControll::minErrorReport: %s", 
		error.toStdString().c_str());
	Min::Database &db = Min::Database::getInstance();
	db.insertLogMessage ("error", error);
	Min::StatusBar::update("Error: "+error,3000);
}

// -----------------------------------------------------------------------------
void Min::RemoteControll::minAbortCase(long testrunid)
{ 
	if (remote_)
		eapiClient_->min_abort_case (testrunid);
	else
		obj_->min_abort_case (testrunid); 
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minAddTestCaseFile(uint moduleid,
                                            const QString &testcasefile)
{ 
	if (remote_)
		eapiClient_->min_add_test_case_file (moduleid, testcasefile);
	else
		obj_->min_add_test_case_file (moduleid, testcasefile); 

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minAddTestModule(const QString &modulepath)
{ 
	if (remote_)
		eapiClient_->min_add_test_module (modulepath);
	else
		obj_->min_add_test_module (modulepath); 
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minFatalError(const QString &what,
                                        const QString &errorstring)
{ 
	if (remote_)
		eapiClient_->min_fatal_error (what, errorstring);
	else
		obj_->min_fatal_error (what,errorstring); 

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minPauseCase(long testrunid)
{ 
	if (remote_)
		eapiClient_->min_pause_case (testrunid);
	else
		obj_->min_pause_case (testrunid); 

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minQueryTestFiles()
{ 
	if (remote_)
		eapiClient_->min_query_test_files ();
	else
		obj_->min_query_test_files (); 
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minQueryTestModules()
{ obj_->min_query_test_modules (); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minResumeCase(long testrunid)
{ 
	if (remote_)
		eapiClient_->min_resume_case (testrunid);
	else
		obj_->min_resume_case (testrunid); 
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minStartCase(uint moduleid, uint caseid, uint groupid)
{
    // Group-id hardcoded to zero in future it should be used (paraeall/sequentiall
    // switch.
    Min::RemoteControll::ExeRequestData *tmp = new Min::RemoteControll::ExeRequestData;
    tmp->moduleid_ = moduleid;
    tmp->caseid_ = caseid;
    tmp->groupid_ = groupid;
    exeRequest_.append(tmp);
    if (remote_)
	    eapiClient_->min_start_case (moduleid,caseid,0);
    else
	    obj_->min_start_case (moduleid,caseid,0);
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minClose()
{ 
    if (remote_)
	    eapiClient_->min_close ();
    else
	    obj_->min_close(); 

    closed_ = true; 
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::setTestCaseFiles(const QStringList &data)
{ testCaseFiles_ = data; }
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
