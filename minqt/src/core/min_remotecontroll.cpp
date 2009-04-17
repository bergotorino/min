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

#include "min_remotecontroll.hpp"

// -----------------------------------------------------------------------------
Min::RemoteControll::RemoteControll()
    : bus_(QDBusConnection::connectToBus(QDBusConnection::SessionBus,
                                        "org.maemo.MIN"))
    , obj_("org.maemo.MIN","/Min",bus_)
{
    // 1. Do error checking
    if (!bus_.isConnected()) {
        qDebug("Error: %s",bus_.lastError().message().toStdString().c_str());
    }

    if (!obj_.isValid()) {
        qDebug("Error: %s",obj_.lastError().message().toStdString().c_str());
    }

    // 2. Connect signals
    connect (&obj_,SIGNAL(min_case_msg(qulonglong, const QString &)),
            this,SLOT(minCaseMsg(qulonglong, const QString &)));

    connect (&obj_,SIGNAL(min_case_paused(qulonglong)),
            this,SLOT(minCasePaused(qulonglong)));

    connect (&obj_,SIGNAL(min_case_result(qulonglong, int, const QString &,
                                            qulonglong, qulonglong)),
            this,SLOT(minCaseResult(qulonglong, int, const QString &,
                                    qulonglong, qulonglong)));

    connect (&obj_,SIGNAL(min_case_resumed(qulonglong)),
            this,SLOT(minCaseResumed(qulonglong)));

    connect (&obj_,SIGNAL(min_case_started(uint, uint, qulonglong)),
            this,SLOT(minCaseStarted(uint, uint, qulonglong)));

    connect (&obj_,SIGNAL(min_module_ready(uint)),
            this,SLOT(minModuleReady(uint)));

    connect (&obj_,SIGNAL(min_new_module(const QString &, uint)),
            this,SLOT(minNewModule(const QString &, uint)));

    connect (&obj_,SIGNAL(min_new_test_case(uint, uint, const QString &)),
            this,SLOT(minNewTestCase(uint, uint, const QString &)));

    connect (&obj_,SIGNAL(min_no_module(const QString &)),
            this,SLOT(minNoModule(const QString &)));

    connect (&obj_,SIGNAL(min_test_files(const QString &)),
            this,SLOT(minTestFiles(const QString &)));
            
    connect (&obj_,SIGNAL(min_test_modules(const QString &)),
            this,SLOT(minTestModules(const QString &)));

    // 3. Open plugin
    obj_.min_open();
}
// -----------------------------------------------------------------------------
bool Min::RemoteControll::isValid() const
{
    if (bus_.isConnected() && obj_.isValid()) return true;
    else return false;
}
// -----------------------------------------------------------------------------
Min::RemoteControll::~RemoteControll()
{ obj_.min_close(); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCaseMsg(qulonglong testrunid,
                                    const QString &message)
{

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCasePaused(qulonglong testrunid)
{

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCaseResult(qulonglong testrunid, int result,
                    const QString &desc,
                    qulonglong starttime, qulonglong endtime)
{

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCaseResumed(qulonglong testrunid)
{

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minCaseStarted(uint moduleid,
                                        uint caseid,
                                        qulonglong testrunid)
{
    qDebug("Min::RemoteControll::minCaseStarted\n");
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minModuleReady(uint moduleid)
{

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minNewModule(const QString &modulename, uint moduleid)
{
    qDebug("Min::RemoteControll::minNewModule\n");
}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minNewTestCase(uint moduleid, uint caseid,
                    const QString &casetitle)
{

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minNoModule(const QString &modulename)
{

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minTestFiles(const QString &files)
{

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minTestModules(const QString &modules)
{

}
// -----------------------------------------------------------------------------
void Min::RemoteControll::minAbortCase(qulonglong testrunid)
{ obj_.min_abort_case (testrunid); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minAddTestCaseFile(uint moduleid,
                                            const QString &testcasefile)
{ obj_.min_add_test_case_file (moduleid, testcasefile); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minAddTestModule(const QString &modulepath)
{ obj_.min_add_test_module (modulepath); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minFatalError(const QString &what,
                                        const QString &errorstring)
{ obj_.min_fatal_error (what,errorstring); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minPauseCase(qulonglong testrunid)
{ obj_.min_pause_case (testrunid); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minQueryTestFiles()
{ obj_.min_query_test_files (); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minQueryTestModules()
{ obj_.min_query_test_modules (); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minResumeCase(qulonglong testrunid)
{ obj_.min_resume_case (testrunid); }
// -----------------------------------------------------------------------------
void Min::RemoteControll::minStartCase(uint moduleid, uint caseid, uint groupid)
{ obj_.min_start_case (moduleid,caseid,groupid); }
// -----------------------------------------------------------------------------
// file created by generator.sh v1.08
