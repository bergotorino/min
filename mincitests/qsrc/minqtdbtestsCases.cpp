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
 *  @file       minqtdbtestsCases.c
 *  @version    0.1
 *  @brief      This file contains implementation of minqtdbtests
 *              test module test functions.
 */
/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include "minqtdbtests.h"
#include "minqtdbtestsCases.hpp"
#include "min_singleton.hpp"
#include "min_database.hpp"

#include <QString>
#include <QDate>

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
const char *module_date = __DATE__;
const char *module_time = __TIME__;
/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL int insertDev (MinItemParser * item);
/* ------------------------------------------------------------------------- */
LOCAL int insertMod (MinItemParser * item);
/* ------------------------------------------------------------------------- */
LOCAL int insertCase (MinItemParser * item);
/* ------------------------------------------------------------------------- */
LOCAL int updateDesc (MinItemParser * item);
/* ------------------------------------------------------------------------- */
LOCAL int checkCase (MinItemParser *item);
/* ------------------------------------------------------------------------- */
LOCAL int checkModuleCount (MinItemParser * item);
/* ------------------------------------------------------------------------- */
LOCAL int initDB (MinItemParser * item);
/* ------------------------------------------------------------------------- */
LOCAL int checkTCFileInsert (MinItemParser * item);
/* ------------------------------------------------------------------------- */
LOCAL int checkTMFileInsert (MinItemParser * item);
/* ------------------------------------------------------------------------- */
LOCAL int insertRun (MinItemParser *item);
/* ------------------------------------------------------------------------- */
LOCAL int updateRun (MinItemParser *item);
/* ------------------------------------------------------------------------- */
LOCAL int insertPrintout (MinItemParser *item);
/* ------------------------------------------------------------------------- */
LOCAL int checkPrintout (MinItemParser *item);
/* ------------------------------------------------------------------------- */
LOCAL int insertLog (MinItemParser *item);
/* ------------------------------------------------------------------------- */
LOCAL int checkLog (MinItemParser *item);
/* ------------------------------------------------------------------------- */
LOCAL int checkIDs (MinItemParser *item);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* ------------------------------------------------------------------------- */
/* None */
/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
LOCAL int insertDev (MinItemParser * item)
{
        int dev_id, ret;
	Min::Database &db = Min::Database::getInstance();
	mip_get_next_int (item,  &dev_id);
	ret = db.insertDevice(dev_id);
	dev_id = db.getDeviceDbId (dev_id);
	
	tm_printf (0,(char *)"", (char *)"inserted device id=%d ret=%d", 
		   dev_id, ret); 
	return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int insertMod (MinItemParser * item)
{
        int dev_id, mod_id, ret;
	char *module_name = NULL;
	Min::Database &db = Min::Database::getInstance();
	mip_get_next_int (item, &dev_id);
	mip_get_next_int (item, &mod_id);
	mip_get_next_string (item, &module_name);
	
	dev_id = db.getDeviceDbId (dev_id);

	ret = db.insertModule(dev_id, mod_id, module_name);
	tm_printf (0, (char *)"", 
		   (char *)"inserted module id=%d dev_id=%d name=%s ret=%d", 
		   mod_id, dev_id, module_name, ret); 

	if (!db.getModuleNameFromEngineId(mod_id).contains(module_name))
		return -1;

	mod_id = db.getModuleDbId (dev_id, mod_id);

	if (!db.getModuleNameFromDbId(mod_id).contains(module_name))
		return -1;
	
	return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int insertCase (MinItemParser * item)
{
        int dev_id, mod_id, case_id, ret;
	char *title = NULL, *desc = NULL, *mod_name = NULL;
	Min::Database &db = Min::Database::getInstance();
	mip_get_next_int (item, &dev_id);
	mip_get_next_string (item, &mod_name);
	mip_get_next_int (item, &case_id);
	mip_get_next_string (item, &title);
	mip_get_next_string (item, &desc);
	
	dev_id = db.getDeviceDbId (dev_id);
	mod_id = db.getModuleDbId (dev_id, mod_name);

	ret = db.insertTestCase(mod_id, case_id, title, desc);
	tm_printf (0, (char *)"", (char *)"inserted case mod_id=%d case_id=%d"
		   " title=%s desc=%s ret=%d", mod_id, case_id, title, desc, 
		   ret); 

	if (!db.getCaseTitleFromEngineId(mod_id,case_id).contains(title))
		return -1;

	case_id = db.getTestCaseDbId (mod_id, case_id);

	if (!db.getCaseTitleFromDbId(mod_id, case_id).contains(title))
		return -1;

	return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int updateDesc (MinItemParser * item)
{
        int dev_id, mod_id, case_id, ret;
	char *title = NULL, *desc = NULL;
	Min::Database &db = Min::Database::getInstance();
	mip_get_next_int (item, &dev_id);
	mip_get_next_int (item, &mod_id);
	mip_get_next_int (item, &case_id);
	mip_get_next_string (item, &title);
	mip_get_next_string (item, &desc);
	
	dev_id = db.getDeviceDbId (dev_id);
	mod_id = db.getModuleDbId (dev_id, mod_id);
	case_id = db.getTestCaseDbId  (mod_id, case_id);

	ret = db.updateCaseDesc(mod_id, case_id, desc);

	return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int checkCase (MinItemParser *item)
{
	int dev_id, mod_id;
	char *casename = NULL;

	Min::Database &db = Min::Database::getInstance();

	mip_get_next_int (item, &dev_id);
	mip_get_next_int (item, &mod_id);
	mip_get_next_string (item, &casename);

	dev_id = db.getDeviceDbId (dev_id);
	mod_id = db.getModuleDbId (dev_id, mod_id);
	tm_printf (0, (char *)"", (char *)"Checking if Case %s exists in DB",
		  casename);
	QStringList list = db.getTestCases(mod_id); 
	if (list.contains(casename))
		return ENOERR;
	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int checkModuleCount (MinItemParser * item)
{
        int dev_id, count;
	char *title = NULL, *desc = NULL;
	Min::Database &db = Min::Database::getInstance();
	mip_get_next_int (item, &dev_id);
	mip_get_next_int (item, &count);
	
	dev_id = db.getDeviceDbId (dev_id);
	
	int modcount = db.getModules(dev_id).size();
	tm_printf (0, (char *)"", (char *)"# mods in db %d, expected %d", 
		   modcount, count); 
	if (count == modcount)
	        return ENOERR;

	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int initDB (MinItemParser * item)
{
	Min::Database &db = Min::Database::getInstance();
	int dev_id;

	mip_get_next_int (item, &dev_id);
	
	dev_id = db.getDeviceDbId (dev_id);

	db.clearDevice ();

	return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int checkTCFileInsert (MinItemParser * item)
{
	char *fname = NULL;
	Min::Database &db = Min::Database::getInstance();
	mip_get_next_string (item, &fname);

	db.insertTestCaseFile(fname);
	QStringList flist = db.getTestCaseFiles();
	if (flist.contains(fname))
		return ENOERR;
	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int checkTMFileInsert (MinItemParser * item)
{
	char *fname = NULL;

	Min::Database &db = Min::Database::getInstance();
	mip_get_next_string (item, &fname);

	db.insertTestModuleFile(fname);
	QStringList flist = db.getTestModuleFiles();
	if (flist.contains(fname))
		return ENOERR;
	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int insertRun (MinItemParser *item)
{
	int ret, dev_id, mod_id, case_id, run_pid, group_id;
	Min::Database &db = Min::Database::getInstance();

	mip_get_next_int (item, &dev_id);
	mip_get_next_int (item, &mod_id);
	mip_get_next_int (item, &case_id);
	mip_get_next_int (item, &run_pid);

	group_id = db.getGroup();
	dev_id = db.getDeviceDbId (dev_id);
	mod_id = db.getModuleDbId (dev_id, mod_id);
	case_id = db.getTestCaseDbId  (mod_id, case_id);

	ret = db.insertTestRun(case_id,
			       run_pid,
			       group_id,
			       TP_RUNNING,
			       QDateTime::currentDateTime().toTime_t());

	QVector<QStringList> vec = db.getTestRunsInGroup(dev_id, group_id);
	for (int i = 0; i < vec.size(); i++) {
		QStringList row = vec [i];
		if (row.contains (db.getCaseTitleFromEngineId (mod_id, 
							       case_id)))
			return ENOERR;
	}


	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int updateRun (MinItemParser *item)
{
	int ret, dev_id, mod_id, case_id, run_pid, group_id;
	char *casename = NULL, *desc = NULL;
	Min::Database &db = Min::Database::getInstance();

	mip_get_next_int (item, &dev_id);
	mip_get_next_int (item, &mod_id);
	mip_get_next_string (item, &casename);
	mip_get_next_int (item, &run_pid);
	mip_get_next_string (item, &desc);

	group_id = db.getGroup();
	dev_id = db.getDeviceDbId (dev_id);
	mod_id = db.getModuleDbId (dev_id, mod_id);
	case_id = db.getTestCaseDbId  (mod_id, casename);
	run_pid = db.getTestRunDbId(case_id, run_pid);

	if(db.updateTestRun(run_pid,
			    TP_ENDED,
			    0,
			    QDateTime::currentDateTime().toTime_t(),
			    TP_PASSED,
			    desc))
		return ENOERR;
	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int checkGroupCount (MinItemParser *item)
{
	int ret, count;
	char *casename = NULL, *desc = NULL;
	Min::Database &db = Min::Database::getInstance();

	mip_get_next_int (item, &count);
	
	QList<unsigned int> list = db.getTestRunGroups();
	
	if (list.size() == count)
		return ENOERR;

	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int checkExecuted (MinItemParser *item)
{
	int ret, dev_id;
	char *casename = NULL;
	Min::Database &db = Min::Database::getInstance();

	mip_get_next_int (item, &dev_id);
	mip_get_next_string (item, &casename);
	dev_id = db.getDeviceDbId (dev_id);

	QVector<QStringList> vec = db.getExecutedView (dev_id);

	for (int i = 0; i < vec.size(); i++) {
		QStringList row = vec [i];
		if (row.contains (casename))
			return ENOERR;
	}

	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int insertPrintout (MinItemParser *item)
{
	int run_pid, ret;
	char *printout = NULL;
	Min::Database &db = Min::Database::getInstance();

	mip_get_next_int (item, &run_pid);
	mip_get_next_string (item, &printout);
	run_pid = db.getTestRunDbId(run_pid); 

	ret = db.insertPrintout(run_pid, printout);
	return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int checkPrintout (MinItemParser *item)
{
	int run_pid;
	char *printout = NULL;
	Min::Database &db = Min::Database::getInstance();

	mip_get_next_int (item, &run_pid);
	mip_get_next_string (item, &printout);
	run_pid = db.getTestRunDbId(run_pid); 

	QStringList list = db.getPrintoutView(run_pid); 
	if (list.contains(printout))
		return ENOERR;

	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int insertLog (MinItemParser *item)
{
	int run_pid, ret;
	char *log = NULL;
	char *logtype = NULL;

	Min::Database &db = Min::Database::getInstance();

	mip_get_next_string (item, &logtype);
	mip_get_next_string (item, &log);

	ret = db.insertLogMessage(logtype, log);

	return ENOERR;
}
/* ------------------------------------------------------------------------- */
LOCAL int checkLog (MinItemParser *item)
{
	int run_pid;
	char *log = NULL, *logtype = NULL;
	Min::Database &db = Min::Database::getInstance();

	mip_get_next_string (item, &logtype);
	mip_get_next_string (item, &log);

	QVector<QStringList> logs = db.getLogMessages();
	for (int i = 0; i < logs.size(); i++) {
		QStringList logrow = logs[i];
		if (logrow.contains(logtype) && logrow.contains(log))
			return ENOERR;
	}

	return -1;
}
/* ------------------------------------------------------------------------- */
LOCAL int checkIDs (MinItemParser *item)
{
	int dev_id, mod_id, case_id, run_id;
	char *log = NULL, *logtype = NULL;
	Min::Database &db = Min::Database::getInstance();
	mip_get_next_int (item, &dev_id);
	mip_get_next_int (item, &mod_id);
	mip_get_next_int (item, &case_id);
	mip_get_next_int (item, &run_id);

	dev_id = db.getDeviceDbId (dev_id);
	if (mod_id != db.getModuleEngineId (db.getModuleDbId (dev_id,
								mod_id), 
					      dev_id))
		return -1;
	mod_id = db.getModuleDbId (dev_id, mod_id);
	if (case_id != db.getTestCaseEngineId (db.getTestCaseDbId(mod_id,
								  case_id),
					      dev_id))
		return -1;

	case_id = db.getTestCaseDbId (mod_id, case_id);
	if (mod_id != db.getModuleIdFromCaseDbId (case_id))
		return -1;


	if (run_id != db.getTestRunEngineId (db.getTestRunDbId (run_id)))
		return -1;

	return ENOERR;
}
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
int ts_get_test_cases (DLList ** list)
{
        ENTRYTC (*list, "Init", initDB);
        ENTRYTC (*list, "InsertDev", insertDev);
	ENTRYTC (*list, "InsertMod", insertMod);
	ENTRYTC (*list, "InsertCase", insertCase);
	ENTRYTC (*list, "UpdateDesc", updateDesc);
	ENTRYTC (*list, "CheckCase", checkCase);
	ENTRYTC (*list, "CheckModCount", checkModuleCount);
	ENTRYTC (*list, "CheckTCFileInsert", checkTCFileInsert);
	ENTRYTC (*list, "CheckTMFileInsert", checkTMFileInsert);
	ENTRYTC (*list, "InsertRun", insertRun);
	ENTRYTC (*list, "UpdateRun", updateRun);
	ENTRYTC (*list, "CheckGroupCount", checkGroupCount);
	ENTRYTC (*list, "CheckExecuted", checkExecuted);
	ENTRYTC (*list, "InsertPrintout", insertPrintout);
	ENTRYTC (*list, "CheckPrintout", checkPrintout);
	ENTRYTC (*list, "InsertLog", insertLog);
	ENTRYTC (*list, "CheckLog", checkLog);
	ENTRYTC (*list, "IdCheck", checkIDs);
}
/* ------------------------------------------------------------------------- */
/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */

