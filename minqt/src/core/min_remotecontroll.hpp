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
 * @file:     min_remotecontroll.hpp
 * @version:  1.00
 * @date:     27.03.2009
 * @author:   
 */

#ifndef INCLUDED_MIN_REMOTECONTROLL_HPP
#define INCLUDED_MIN_REMOTECONTROLL_HPP

// System includes
#include <QtDBus/QtDBus>
#include <QObject>

// Min includes
#include "min_object.hpp.h"
#include "min_singleton.hpp"

#define MIN_SERVICE_NAME     "org.maemo.MIN"
#define MIN_SERVICE_OBJECT_PATH "/Min"
#define MIN_SERVICE_INTERFACE "org.maemo.MIN"

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // -------------------------------------------------------------------------
    /**
     * @class RemoteControll
     * @brief Interface between MinQt GUI and MIN through DBus
     *
     * RemoteControll owns object MinObject which is generated from Min DBus 
     * interface xml file. It doubles all functionalities (this is needed since
     * MinObject files cannot be edited because their are generated 
     * automatically) provided by MinObject. It is designed as Singleton 
     * (only one instance for whole program) and uses Min::Database class to
     * interact with MinQt data storage place.
     *
     * MinQt uses RemoteControll class to interact with Min through DBus
     */
    class RemoteControll: public QObject, public Min::Singleton<RemoteControll>
    {
    friend class Min::Singleton<RemoteControll>;
    /** Copying is forbidden */
    Q_DISABLE_COPY(RemoteControll);
    Q_OBJECT
    public:

        /** Destructor. */
        ~RemoteControll();

        /** Gives status of object.
         *  @return true if initialization was success, false otherwise
         */
        bool isValid() const;

    public:

        /**@{ Functionalities of MinObject */

        /** Aborts specified ongoing test case.
         *  @param testrunid id of the ongoing test case
         */
        void minAbortCase(qulonglong testrunid);

        /** Adds file with test cases to the module
         *  @param moduleid is the id of motole to which file will be added
         *  @param testcasefile is the path to the test case file
         */
        void minAddTestCaseFile(uint moduleid, const QString &testcasefile);

        /** Adds test module to Min
         *  @param modulepath is the path to the module to be added
         */
        void minAddTestModule(const QString &modulepath);

        /** Indicates there is fatal error and MinQt will not operate
         *  @param what is description of what has failed
         *  @param errorstring is the exact error
         */
        void minFatalError(const QString &what, const QString &errorstring);

        /** Pauses specified case
         *  @param testrunid is the id of ongoing case to be paused
         */
        void minPauseCase(qulonglong testrunid);

        /** Asks Min for list of test case files */
        void minQueryTestFiles();

        /** Asks Min for list of test module files */
        void minQueryTestModules();

        /** Resules previously paused case
         *  @param testrunid is the run id of pasued test case
         */
        void minResumeCase(qulonglong testrunid);

        /** Executed a test case
         *  @param moduleid is the id of module to which test case belongs to
         *  @param caseid is the id of test case within test module
         *  @param groupid is the id of group to which test case belongs to
         *
         *  Note: group id is used when user selects more than one case to be
         *  executed
         */
        void minStartCase(uint moduleid, uint caseid, uint groupid);
        /**@}*/

    private slots:

        /**@{ Signals from MinObject */

        /** Message from ongoing test case
         *  @aram testrunid indicates test case
         *  @param message is the message from test case
         */
        void minCaseMsg(qulonglong testrunid, const QString &message);

        /** Confirmation that test case has been paused
         *  @aram testrunid indicates test case
         */
        void minCasePaused(qulonglong testrunid);

        /** Test case result
         *  @aram testrunid indicates test case
         *  @param result is the test case result
         *  @param desc is the test case result description
         *  @param startime is the unix timestamp of test case start time
         *  @param endtime is the unix timestamp of test case end time
         */
        void minCaseResult(qulonglong testrunid, int result,
                            const QString &desc,
                            qulonglong starttime, qulonglong endtime);

        /** Confirmation that paused test case has been resumed
         *  @aram testrunid indicates test case
         */
        void minCaseResumed(qulonglong testrunid);

        /** Confirmation that test case has been started
         *  @param moduleid is the id of module to which test case belongs to
         *  @param caseid is the id of test case
         *  @param testrunid is the run id that has been assigned to test case
         *
         *  Note: testrunid is used for interaction with test case that is
         *  executing
         */
        void minCaseStarted(uint moduleid, uint caseid, qulonglong testrunid);

        /** Indicates that all test cases has been enumerated from module
         *  @param moduleid is the id of module
         */
        void minModuleReady(uint moduleid);

        /** Indicates that module has been added to Min
         *  @param modulename is the name of module
         *  @param moduleid is the id which has been assigned to the module
         */
        void minNewModule(const QString &modulename, uint moduleid);

        /** Indicates that test case has been found in module
         *  @param moduleid indicates the id of module to whch test case belongs to
         *  @param caseid indicates the test case id within the module
         *  @param casetitle is the title fo the test case
         */
        void minNewTestCase(uint moduleid, uint caseid,
                            const QString &casetitle);

        /** Indicates that specified module has not been added
         *  @param modulename is the name of the module
         */
        void minNoModule(const QString &modulename);

        /** Passes list of test case files available in Min */
        void minTestFiles(const QString &files);

        /** Passes list of test modules available in Min */
        void minTestModules(const QString &modules);
        /**@}*/

    private:

        /** Default C++ Constructor. */
        RemoteControll();

        /** Connection to the DBus */
        QDBusConnection bus_;

        /** Generated from DBus interface .xml MinObject */
        MinObject obj_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_REMOTECONTROLL_HPP (file created by generator.sh v1.08)

