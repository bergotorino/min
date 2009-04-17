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
     * @brief
     */
    class RemoteControll: public QObject
    {
    Q_OBJECT
    public:
        /** Default C++ Constructor. */
        RemoteControll();

        /** Destructor. */
        ~RemoteControll();

        /** Gives status of object.
         *  @return true if initialization was success, false otherwise
         */
        bool isValid() const;

    public:

        /**@{ Functionalities of MinObject */
        void minAbortCase(qulonglong testrunid);
        void minAddTestCaseFile(uint moduleid, const QString &testcasefile);
        void minAddTestModule(const QString &modulepath);
        void minFatalError(const QString &what, const QString &errorstring);
        void minPauseCase(qulonglong testrunid);
        void minQueryTestFiles();
        void minQueryTestModules();
        void minResumeCase(qulonglong testrunid);
        void minStartCase(uint moduleid, uint caseid, uint groupid);
        /**@}*/

    private slots:

        /**@{ Signals from MinObject */
        void minCaseMsg(qulonglong testrunid, const QString &message);
        void minCasePaused(qulonglong testrunid);
        void minCaseResult(qulonglong testrunid, int result,
                            const QString &desc,
                            qulonglong starttime, qulonglong endtime);
        void minCaseResumed(qulonglong testrunid);
        void minCaseStarted(uint moduleid, uint caseid, qulonglong testrunid);
        void minModuleReady(uint moduleid);
        void minNewModule(const QString &modulename, uint moduleid);
        void minNewTestCase(uint moduleid, uint caseid,
                            const QString &casetitle);
        void minNoModule(const QString &modulename);
        void minTestFiles(const QString &files);
        void minTestModules(const QString &modules);
        /**@}*/

    private:
        /**@{Declared but nit defined.*/
        /** Copy Constructor. */
        RemoteControll( const Min::RemoteControll& c );
        /** Assignment operator. */
        Min::RemoteControll& operator =( const Min::RemoteControll& c );
        /**@}*/

        /** Connection to the DBus */
        QDBusConnection bus_;

        /** Generated from DBus interface .xml MinObject */
        MinObject obj_;
    };
    // -------------------------------------------------------------------------
}; // namespace Min
// -----------------------------------------------------------------------------
#endif // INCLUDED_MIN_REMOTECONTROLL_HPP (file created by generator.sh v1.08)

