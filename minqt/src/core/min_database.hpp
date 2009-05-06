/*
 * This file is part of MIN Test Framework. Copyright © 2008 Nokia Corporation
 * and/or its subsidiary(-ies).
 * Contact: Maciej Jablonski
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


#ifndef INCLUDED_MIN_DATABASE_HPP
#define INCLUDED_MIN_DATABASE_HPP

// System includes
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

// Min includes
#include <min_singleton.hpp>

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
    // ---------------------------------------------------------------------
    class Database: public QObject,
                    public Min::Singleton<Database>
    {
    Q_OBJECT
    friend class Singleton<Database>;
    public:
	
	/**
	 * default destructor
	 */
        ~Database();

	/**
	 * device insertion to database
	 * @param device_id - device ID in MIN
	 * @ret device ID in database
	 */
        unsigned int insertDevice(unsigned int device_id);

	/**
	 * module insertion do database
	 * @param device_dbid - device ID in database
	 * @param module_id - module ID in MIN
	 * @param mosule_name - module name in MIN
	 * @ret module ID in database
	 */
        unsigned int insertModule(unsigned int device_dbid,
                                  unsigned int module_id,
                                  const QString &module_name);

	/**
	 * test case insertion to database
	 * @param module_dbid - module ID in database
	 * @param test_case_id - test case ID in MIN
	 * @param test_case_title - test case title in MIN
	 * @parame test_case_description - description of test case
	 * @ret test case ID in database
	 */
        unsigned int insertTestCase(unsigned int module_dbid,
                                    unsigned int test_case_id,
                                    const QString &test_case_title,
				    const QString &test_case_description);

	/**
	 * test run  (an "instance" of test case) insertion to database
	 * @param test_case_dbid - test case ID in database
	 * @param test_run_pid - test run ID in min (process ID)
	 * @param group_id - group ID of test run
	 * @param status - status of test run (paused, ongoing, finished etc.)
	 * @param start_time - start time of test run (in seconds since 1970/01/01 00:00:00 UTC)
	 * @param end_time - end time of test run (same as above)
	 * @ret test run ID in database
	 */
        unsigned int insertTestRun(unsigned int test_case_dbid,
                                   unsigned int test_run_pid,
                                   unsigned int group_id,
                                   int status,
                                   unsigned long start_time);

	/**
	 * printout from test case run insertion
	 * @param test_run_dbid - test run ID in database
	 * @param content - text of printout
	 * @ret - ID of printout in database
	 */
        unsigned int insertPrintout(unsigned int test_run_dbid,
                                    const QString &content);

	/**
	 * updates test run information ( of pause, resume etc.)
	 * @param dbid - database ID of test run (mandatory)
	 * @param status - new status of test run (mandatory)
	 * @param start_time (optional) - start time of test run (if 0 not updated)
	 * @param end_time (optional) - end time of test run (if 0 not updated)
	 * @param result (optional) - result of test run
	 * @param result_description (optional) - result description of test run)
	 * @ret true on success, false on fail
	 */
        bool updateTestRun(unsigned int dbid,
                           int status,
                           unsigned long start_time=0,
                           unsigned long end_time=0,
                           int result=0,
                           const QString &result_description="");

	/**
	 * gets devise ID from database
	 * @param device_id - device id from MIN
	 *
	 */
        unsigned int getDeviceDbId(unsigned int device_id);
        unsigned int getModuleDbId(unsigned int device_id,
                                 unsigned int module_id);
        unsigned int getModuleDbId(unsigned int device_id,
                                 const QString &module_name);
        unsigned int getTestCaseDbId(unsigned int module_id,
                                   unsigned int test_case_id);
        unsigned int getTestCaseDbId(unsigned int module_id,
                                   const QString &test_case_name);
        unsigned int getTestRunDbId(unsigned int test_case_id,
                                  unsigned int test_run_pid);

	/**
	 * gets module names from database for UI
	 *
	 */
        QStringList getModules(unsigned int device_dbid);

	/**
	 * gets test case names from database for UI
	 */
        QStringList getTestCases(unsigned int module_dbid = 1);

	QVector<QStringList> getAvailableView(unsigned int device_dbid = 1) const;

	QVector<QStringList> getExecutedView(unsigned int device_dbid = 1);

	QVector<QStringList> getOngoingView(unsigned int device_dbid = 1);

	QVector<QStringList> getPassedView(unsigned int device_dbid = 1);

	QVector<QStringList> getFailedView(unsigned int device_dbid = 1);

	QVector<QStringList> getAbortedView(unsigned int device_dbid = 1);

    signals:

        /** Emitted when database contents are updated (inserted,removed, changed) */
        void updated();

    private:
	/**
	 * default constructor
	 */
        Database();
        /**@{ Declared but not defined by design. */
        /** Copy Constructor. */
        Database(const Min::Database &c);
        /** Assignment operator. */
        Database& operator =(const Min::Database &c);
        /**@}*/

        bool initDatabase();
        QSqlDatabase db;
    };
    // -------------------------------------------------------------------------
};
// namespace Min
// -----------------------------------------------------------------------------
#endif  // INCLUDED_MIN_DATABASE

