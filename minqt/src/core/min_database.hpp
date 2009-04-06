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

// -----------------------------------------------------------------------------
/**
 * @namespace Min
 * @brief
 */
namespace Min
{
	// ---------------------------------------------------------------------
	class Database
	{
		public:
			Database();
			~Database();
			unsigned int insertDevice(unsigned int device_id);
			unsigned int insertModule(unsigned int device_id,
					unsigned int module_id,
					QString module_name);
			unsigned int insertTestCase(unsigned int module_id,
					unsigned int test_case_id,
					QString test_case_title);
			unsigned int insertTestRun(unsigned int test_case_id,
					unsigned int test_run_pid,
					unsigned int group_id,
					int status,
					unsigned long start_time);
			unsigned int insertPrintout(unsigned int test_run_pid,
					QString content);

			bool updateDevice(unsigned int id,
					unsigned int new_device_id);
			bool updateModule(unsigned int id,
					unsigned int device_id,
					unsigned int module_id,
					QString module_name);
			bool updateTestCase(unsigned int id,
					unsigned int module_id,
					unsigned int test_case_id,
					QString test_case_title);
			bool updateTestRun(unsigned int id,
					unsigned int test_case_id,
					unsigned int test_run_pid,
					unsigned int group_id,
					int status,
					unsigned long start_time,
					unsigned long end_time,
					int result,
					QString result_description);
			bool updatePrintout(unsigned int id,
					unsigned int test_run_id,
					QString content);

			unsigned int getDeviceId(unsigned int device_id);
			unsigned int getModuleId(unsigned int device_id,
					unsigned int module_id);
			unsigned int getModuleId(unsigned int device_id,
					QString module_name);
			unsigned int getTestCaseId(unsigned int module_id,
					unsigned int test_case_id);
			unsigned int getTestCaseId(unsigned int module_id,
					QString test_case_name);
			unsigned int getTestRunId(unsigned int test_case_id,
					test_run_pid);

			QStringList getModules(unsigned int device_id);
			QStringList getTestCases(unsigned int module_id);

		private:
			bool initDatabase();
			QSqlDatabase db;
			
        };
// -------------------------------------------------------------------------
};
// namespace Min
// -----------------------------------------------------------------------------
#endif  // INCLUDED_MIN_DATABASE

