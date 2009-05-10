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





#include "min_database.hpp"
#include "tmc_common.h"

// ----------------------------------------------------------------------------
Min::Database::Database()
{
    initDatabase();
};

// ----------------------------------------------------------------------------
Min::Database::~Database()
{
	db.close();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertDevice(unsigned int device_id)
{
    // Check if device with this id exists
    QSqlQuery query;
    query.prepare("SELECT id FROM test_case WHERE device_id=:devid;");
    query.bindValue(":devid", QVariant(device_id));
    query.exec();
    if (query.size()>0) return 0;
    query.finish();

    // Insert device id
    query.prepare("INSERT INTO device(device_id) VALUES (:devid);");
    query.bindValue(":devid", QVariant(device_id));
    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();

    // Notify
    if (retval) emit updated();

    // Return id of last insert
    return retval;
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertModule(unsigned int device_dbid,
                unsigned int module_id,
                const QString &module_name)
{
    // Check if module with this id exists
    QSqlQuery query;
    query.prepare("SELECT id FROM module WHERE device_id=:devid AND module_id=:modid AND module_name=:name;");
    query.bindValue(QString(":devid"), QVariant(device_dbid));
    query.bindValue(QString(":modid"), QVariant(module_id));
    query.bindValue(QString(":name"), QVariant(module_name));
    query.exec();
    if (query.size()>0) return 0;
    query.finish();

    // Insert module
    query.prepare("INSERT INTO module(module_id, device_id, module_name) VALUES (:modid, :devid, :name);");
    query.bindValue(QString(":devid"), QVariant(device_dbid));
    query.bindValue(QString(":modid"), QVariant(module_id));
    query.bindValue(QString(":name"), QVariant(module_name));
    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();

    // Notify
    if (retval) emit updated();

    // Return id of last insert
    return retval;
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertTestCase(unsigned int module_dbid,
                unsigned int test_case_id,
                const QString &test_case_title,
		const QString &test_case_description)
{
    // Check if test case with this id exists
    QSqlQuery query;
    query.prepare("SELECT id FROM test_case WHERE module_id=:modid AND module_name=:title AND test_case_id=:caseid AND test_case_description=:descr;");
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    query.bindValue(QString(":modid"), QVariant(module_dbid));
    query.bindValue(QString(":title"), QVariant(test_case_title));
    query.bindValue(QString(":descr"), QVariant(test_case_description));
    query.exec();
    if (query.size()>0) return 0;
    query.finish();

    // Insert test case
    query.prepare("INSERT INTO test_case(test_case_id, module_id, test_case_title, test_case_description) VALUES (:caseid, :modid, :title, :descr);");
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    query.bindValue(QString(":modid"), QVariant(module_dbid));
    query.bindValue(QString(":title"), QVariant(test_case_title));
    query.bindValue(QString(":descr"), QVariant(test_case_description));
    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();

    // Notify
    if (retval) emit updated();

    // Return id of last insert
    return retval;
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertTestRun(unsigned int test_case_dbid,
                                        unsigned int test_run_pid,
                                        unsigned int group_id,
                                        int status,
                                        unsigned long start_time)
{
    // Insert test run
    QSqlQuery query;
    query.prepare("INSERT INTO test_run(test_run_pid, test_case_id, group_id, status, start_time)  VALUES (:runpid, :caseid, :groupid, :status, :starttime);");
    query.bindValue(QString(":caseid"), QVariant(test_case_dbid));
    query.bindValue(QString(":runpid"), QVariant(test_run_pid));
    query.bindValue(QString(":groupid"), QVariant(group_id));
    query.bindValue(QString(":status"), QVariant(status));
    query.bindValue(QString(":starttime"), QVariant( (qlonglong)start_time));
    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();

    // Notify
    if (retval) emit updated();

    // Return id of last insert
    return retval;
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertPrintout(unsigned int test_run_dbid,
                                        const QString &content)
{
    QSqlQuery query;
    query.prepare("INSERT INTO printout(test_run_id, content) VALUES (:runid, :content);");
    query.bindValue(QString(":runid"), QVariant(test_run_dbid));
    query.bindValue(QString(":content"), QVariant(content));
    if (query.exec()) {
        // Notify
        emit updated();
        return query.lastInsertId().toUInt();
    }
    else return 0;

    // Notify
    emit updated();
};

// ----------------------------------------------------------------------------
bool Min::Database::updateTestRun(unsigned int dbid,
                                int status,
                                unsigned long start_time,
                                unsigned long end_time,
                                int result,
                                const QString &result_description)
{

    // Update existing test run
    QSqlQuery query;
    QString raw_query("");
    raw_query.append("UPDATE test_run SET status=");
    raw_query.append(QString::number(status));
    if (0!=start_time) {
        raw_query.append(" , start_time=");
        raw_query.append(QString::number(start_time));
    }
    if (0!=end_time) {
        raw_query.append(" , end_time=");
        raw_query.append(QString::number(end_time));
        raw_query.append(" , result=");
        raw_query.append(QString::number(result));
        raw_query.append(" , result_description=\"");
        raw_query.append(result_description);
        raw_query.append("\"");
    }
    raw_query.append(" WHERE id=");
    raw_query.append(QString::number(dbid));
    raw_query.append(" ;");
    bool retval = query.exec(raw_query);

    // Notify
    if (retval) emit updated();

    // Return status of update
    return retval;
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getDeviceDbId(unsigned int device_id)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM device WHERE device_id=:devid;");
    query.bindValue(QString(":devid"), QVariant(device_id));
    if(query.exec()){
        if(query.next()) id=query.value(0);
    }
    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getModuleDbId(unsigned int device_id,
                                        unsigned int module_id)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM module WHERE device_id=:devid AND module_id=:modid;");
    query.bindValue(QString(":devid"), QVariant(device_id));
    query.bindValue(QString(":modid"), QVariant(module_id));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }

    }
    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getModuleDbId(unsigned int device_id,
                                        const QString &module_name)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM module WHERE device_id=:devid AND module_name=:modname;");
    query.bindValue(QString(":devid"), QVariant(device_id));
    query.bindValue(QString(":modname"), QVariant(module_name));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getModuleEngineId(unsigned int deviceId,
                                            unsigned int moduleDbId)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT module_id FROM module WHERE device_id=:devid AND id=:moddbid;");
    query.bindValue(QString(":devid"), QVariant(deviceId));
    query.bindValue(QString(":moddbid"), QVariant(moduleDbId));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    return id.toUInt();
}
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestCaseDbId(unsigned int module_dbid,
                                        unsigned int test_case_id)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM test_case WHERE module_id=:modid AND test_case_id=:caseid;");
    query.bindValue(QString(":modid"), QVariant(module_dbid));
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestCaseDbId(unsigned int module_id,
                                        const QString &test_case_name)
{
    QSqlQuery query;
    QVariant id(0);
    query.prepare("SELECT id FROM test_case WHERE module_id=:modid AND test_case_title=:casename;");
    query.bindValue(QString(":modid"), QVariant(module_id));
    query.bindValue(QString(":casename"), QVariant(test_case_name));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestCaseEngineId(unsigned int moduleDbId,
                                                unsigned int testCaseDbId)
{
    QSqlQuery query;
    QVariant id(0);
    query.prepare("SELECT test_case_id FROM test_case WHERE module_id=:moddbid AND id=:casedbid;");
    query.bindValue(QString(":moddbid"), QVariant(moduleDbId));
    query.bindValue(QString(":casedbid"), QVariant(testCaseDbId));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    return id.toUInt();
}
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestRunDbId(unsigned int test_run_pid)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM test_run WHERE test_run_pid=:runpid;");
    query.bindValue(QString(":runpid"), QVariant(test_run_pid));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestRunDbId(unsigned int test_case_id,
                                        unsigned int test_run_pid)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM test_run WHERE test_case_id=:caseid AND test_run_pid=:runpid;");
    query.bindValue(QString(":runpid"), QVariant(test_run_pid));
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    return id.toUInt();
};
// ----------------------------------------------------------------------------
QStringList Min::Database::getModules(unsigned int device_dbid)
{
    QSqlQuery query;
    QStringList retval;
    query.prepare("SELECT module_name FROM module WHERE device_id=:devid;");
    query.bindValue(QString(":devid"), QVariant(device_dbid));
    if(query.exec()){
        while(query.next()) {
            retval.append(query.value(0).toString());
        }
    }
    return retval;
};
// ----------------------------------------------------------------------------
QStringList Min::Database::getTestCases(unsigned int module_dbid)
{
    QSqlQuery query;
    QStringList retval;
    query.prepare("SELECT test_case_name FROM module WHERE module_id=:modid;");
    query.bindValue(QString(":modid"), QVariant(module_dbid) );
    if(query.exec()){
        while(query.next()) {
            retval.append(query.value(0).toString());
        }
    }
    return retval;

};
// ----------------------------------------------------------------------------
QVector<QStringList> Min::Database::getAvailableView(unsigned int devid) const
{
    QSqlQuery query;
    QVector<QStringList> retval;
    QStringList row;
    query.prepare("SELECT module_name, test_case_title, test_case_description, test_case_dbid FROM availableview WHERE device_dbid=:devdbid;");
    query.bindValue(QString(":devdbid"), QVariant(devid));
    if(query.exec()){
        while(query.next()) {
	    row.clear();
            row.append(query.value(0).toString());
            row.append(query.value(1).toString());
            row.append(query.value(2).toString());
            row.append(query.value(3).toString());
	    retval.append(row);
        }
    }
    return retval;
};
// ----------------------------------------------------------------------------
QStringList Min::Database::getPrintoutView(unsigned int test_run_dbid) const
{
    QSqlQuery query;
    QStringList retval;
    query.prepare("SELECT content FROM printout WHERE test_run_id=:trid;");
    query.bindValue(QString(":trid"), QVariant(test_run_dbid));
    if(query.exec()){
        while(query.next()) {
            retval.append(query.value(0).toString());
        }
    }
    return retval;
};
// ----------------------------------------------------------------------------
QVector<QStringList> Min::Database::getExecutedView(unsigned int device_dbid) const
{
    QSqlQuery query;
    QVector<QStringList> retval;
    QStringList row;
    query.prepare("SELECT * FROM executedview WHERE device_dbid=:devdbid;");
    query.bindValue(QString(":devdbid"), QVariant(device_dbid));
    if(query.exec()){
        while(query.next()) {
	    row.clear();
            row.append(query.value(0).toString());
            row.append(query.value(1).toString());
            row.append(query.value(2).toString());
            row.append(query.value(3).toString());
            row.append(query.value(4).toString());
            row.append(query.value(5).toString());
            row.append(query.value(6).toString());
            row.append(query.value(7).toString());
            row.append(query.value(8).toString());
            row.append(query.value(9).toString());
	    retval.append(row);
        }
    }
    return retval;
};
// ----------------------------------------------------------------------------
bool Min::Database::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("/home/user/min.db");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(0, QString("Cannot open database"),
            QString("Unable to establish MIN database backend.\n"
                     "This plugin needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }else{
        QSqlQuery query;
        query.exec("CREATE TABLE device (id INTEGER PRIMARY KEY, device_id int);");
        query.exec("CREATE TABLE module (id INTEGER PRIMARY KEY, module_id int, device_id int, module_name varchar);");
        query.exec("CREATE TABLE test_case (id INTEGER PRIMARY KEY, test_case_id int, module_id int, test_case_title varchar, test_case_description varchar);");
        query.exec("CREATE TABLE test_run (id INTEGER PRIMARY KEY, test_run_pid int, test_case_id int, group_id int, status int, start_time int, end_time int, result int default 2, result_description varchar);");
        query.exec("CREATE TABLE printout (id INTEGER PRIMARY KEY, test_run_id int, content varchar);");

        /* Demo data */
/*        
    query.exec("INSERT INTO device VALUES (NULL, 10);");
    query.exec("INSERT INTO module VALUES (NULL, 1, 1, \"minDemo0\");");
	query.exec("INSERT INTO module VALUES (NULL, 2, 1, \"minDemo1\");");

	query.exec("INSERT INTO test_case VALUES(NULL, 1, 1, \"D1\", \"demo 1 test case\");");
	query.exec("INSERT INTO test_case VALUES(NULL, 2, 1, \"D2\", \"demo 2 test case\");");
	query.exec("INSERT INTO test_case VALUES(NULL, 3, 1, \"D3\", \"demo 3 test case\");");
	query.exec("INSERT INTO test_case VALUES(NULL, 4, 1, \"D4\", \"demo 4 test case\");");
	query.exec("INSERT INTO test_case VALUES(NULL, 1, 2, \"D5\", \"demo 5 test case\");");
	query.exec("INSERT INTO test_case VALUES(NULL, 2, 2, \"D6\", \"demo 6 test case\");");

	query.exec("INSERT INTO test_run VALUES(NULL, 12345, 3, 1, 1, 10, 100, 1, \"result of run\" );");
	query.exec("INSERT INTO test_run VALUES(NULL, 12346, 3, 1, 1, 10, 100, 1, \"result of run\" );");
	query.exec("INSERT INTO test_run VALUES(NULL, 12360, 3, 1, 1, 10, 100, 1, \"result of run\" );");
	query.exec("INSERT INTO test_run VALUES(NULL, 12399, 3, 2, 1, 10, 100, 1, \"result of run\" );");
	query.exec("INSERT INTO test_run VALUES(NULL, 12400, 3, 2, 1, 10, 100, 1, \"result of run\" );");
	query.exec("INSERT INTO test_run VALUES(NULL, 12460, 3, 2, 1, 10, 100, 1, \"result of run\" );");
	query.exec("INSERT INTO test_run VALUES(NULL, 12500, 3, 2, 1, 10, 100, 1, \"result of run\" );");
*/
	query.exec("INSERT INTO printout VALUES (NULL, 1, \"printout 1\");");
	query.exec("INSERT INTO printout VALUES (NULL, 1, \"printout 2\");");
	query.exec("INSERT INTO printout VALUES (NULL, 1, \"printout 3\");");
	query.exec("INSERT INTO printout VALUES (NULL, 1, \"printout 4\");");
	query.exec("INSERT INTO printout VALUES (NULL, 1, \"printout 5\");");

	query.exec("CREATE VIEW availableview AS SELECT module.device_id AS device_dbid, module.module_name AS module_name, test_case.test_case_title AS test_case_title, test_case.test_case_description AS test_case_description, test_case.id AS test_case_dbid FROM test_case, module WHERE module.id=test_case.module_id;");
	query.exec("CREATE VIEW executedview AS SELECT test_case.test_case_title AS test_case_title, test_case.test_case_description AS test_case_description, test_run.group_id AS group_id, test_run.status AS status, test_run.start_time AS start_time, test_run.end_time AS end_time, test_run.result AS result, test_run.result_description AS result_description, test_run.id as test_run_dbid, module.device_id AS device_dbid FROM module, test_case, test_run WHERE test_run.test_case_id=test_case.id AND module.id=test_case.module_id;");
//		qDebug("view creation failed");
	

//	query.exec("CREATE VIEW availableview AS SELECT module.module_name AS module_name, test_case.test_case_title AS test_case_title, \"descr\" AS test_case_description, test_case.id AS test_case_dbid FROM test_case, module WHERE module.id=test_case.module_id;");

        return true;
    }


};
// ----------------------------------------------------------------------------
