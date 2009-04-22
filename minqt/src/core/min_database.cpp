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

// ----------------------------------------------------------------------------
Min::Database::Database()
{
    initDatabase();
};

// ----------------------------------------------------------------------------
Min::Database::~Database()
{
    /* EMPTY BODY */
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertDevice(unsigned int device_id)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM test_case WHERE device_id=:devid;");
    query.bindValue(":devid", QVariant(device_id));
    query.exec();
    if(query.size()>0){
        return 0;
    }
    query.finish();
    query.prepare("INSERT INTO device(device_id) VALUES (:devid);");
    query.bindValue(":devid", QVariant(device_id));
    if(query.exec()){
        return query.lastInsertId().toUInt();
    }else{
        return 0;
    }


};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertModule(unsigned int device_dbid,
                unsigned int module_id,
                const QString &module_name)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM module WHERE device_id=:devid AND module_id=:modid AND module_name=:name;");
    query.bindValue(QString(":devid"), QVariant(device_dbid));
    query.bindValue(QString(":modid"), QVariant(module_id));
    query.bindValue(QString(":name"), QVariant(module_name));
    query.exec();
    if(query.size()>0){
        return 0;
    }
    query.finish();
    query.prepare("INSERT INTO module(module_id, device_id, module_name) VALUES (:modid, :devid, :name);");
    query.bindValue(QString(":devid"), QVariant(device_dbid));
    query.bindValue(QString(":modid"), QVariant(module_id));
    query.bindValue(QString(":name"), QVariant(module_name));
    if(query.exec()){
        return query.lastInsertId().toUInt();
    }else{
        return 0;
    }

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertTestCase(unsigned int module_dbid,
                unsigned int test_case_id,
                const QString &test_case_title,
		const QString &test_case_description)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM test_case WHERE module_id=:modid AND module_name=:title AND test_case_id=:caseid AND test_case_description=:descr;");
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    query.bindValue(QString(":modid"), QVariant(module_dbid));
    query.bindValue(QString(":title"), QVariant(test_case_title));
    query.bindValue(QString(":descr"), QVariant(test_case_description));
    query.exec();
    if(query.size()>0){
        return 0;
    }
    query.finish();
    query.prepare("INSERT INTO test_case(test_case_id, module_id, test_case_title, test_case_description) VALUES (:caseid, :modid, :title, :descr);");
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    query.bindValue(QString(":modid"), QVariant(module_dbid));
    query.bindValue(QString(":title"), QVariant(test_case_title));
    query.bindValue(QString(":descr"), QVariant(test_case_description));
    if(query.exec()){
        return query.lastInsertId().toUInt();
    }else{
        return 0;
    }

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertTestRun(unsigned int test_case_dbid,
                                        unsigned int test_run_pid,
                                        unsigned int group_id,
                                        int status,
                                        unsigned long start_time)
{
    QSqlQuery query;
    query.prepare("INSERT INTO test_run(test_run_pid, test_case_id, group_id, status, start_time)  VALUES (:runpid, :caseid, :groupid, :status, :starttime);");
    query.bindValue(QString(":caseid"), QVariant(test_case_dbid));
    query.bindValue(QString(":runpid"), QVariant(test_run_pid));
    query.bindValue(QString(":groupid"), QVariant(group_id));
    query.bindValue(QString(":status"), QVariant(status));
    query.bindValue(QString(":starttime"), QVariant( (qlonglong)start_time));
    if(query.exec()){
        return query.lastInsertId().toUInt();
    }else{
        return 0;
    }
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertPrintout(unsigned int test_run_dbid,
                                        const QString &content)
{
    QSqlQuery query;
    query.prepare("INSERT INTO printout(test_run_id, content) VALUES (:runid, :content);");
    query.bindValue(QString(":runid"), QVariant(test_run_dbid));
    query.bindValue(QString(":content"), QVariant(content));
    if (query.exec()) return query.lastInsertId().toUInt();
    else return 0;
};

// ----------------------------------------------------------------------------
bool Min::Database::updateTestRun(unsigned int dbid,
                                int status,
                                unsigned long start_time,
                                unsigned long end_time,
                                int result,
                                const QString &result_description)
{
    QSqlQuery query;
    QString raw_query("");
    raw_query.append("UPDATE test_run SET status=");
    raw_query.append(QString::number(status));
    if(0!=start_time){
        raw_query.append(" , start_time=");
        raw_query.append(QString::number(start_time));
    }
    if(0!=end_time){
        raw_query.append(" , end_time=");
        raw_query.append(QString::number(end_time));
        raw_query.append(" , result=");
        raw_query.append(QString::number(result));
        raw_query.append(" , result_description=");
        raw_query.append(result_description);
    }
    raw_query.append(" WHERE id=");
    raw_query.append(QString::number(dbid));
    raw_query.append(" ;");
    return query.exec(raw_query);

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
unsigned int Min::Database::getTestCaseDbId(unsigned int module_id,
                                        unsigned int test_case_id)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM test_case WHERE module_id=:modid AND test_case_id=:caseid;");
    query.bindValue(QString(":modid"), QVariant(module_id));
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
    query.prepare("SELECT id FROM test_case WHERE module_id=:modid AND test_case_name=:casename;");
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
QVector<QStringList> Min::Database::getUIView(unsigned int device_dbid)
{
    QSqlQuery query;
    QVector<QStringList> retval;
    QStringList row;
    query.prepare("SELECT * FROM uiview;");
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
bool Min::Database::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
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
        query.exec("CREATE TABLE test_run (id INTEGER PRIMARY KEY, test_run_pid int, test_case_id int, group_id int, status int, start_time int, end_time int, result int, result_description varchar);");
        query.exec("CREATE TABLE printout (id INTEGER PRIMARY KEY, test_run_id int, content varchar);");

	
	
	query.exec("CREATE VIEW uiview AS SELECT module.module_name AS module_name, test_case.test_case_name AS test_case_name, test_case.test_case_description AS test_case_description, test_case.id AS test_case_dbid WHERE module.id=test_case.module_id;");

        /* Demo data */
        
        query.exec("INSERT INTO device VALUES (NULL, 10);");
        query.exec("INSERT INTO module VALUES (NULL, 1, 10, \"minDemoModule\");");
        query.exec("INSERT INTO module VALUES (NULL, 2, 10, \"scripter\");");
	query.exec("INSERT INTO test_case VALUES(NULL, 1, 1, \"Demo_1\", \"\");");
	query.exec("INSERT INTO test_case VALUES(NULL, 2, 1, \"Demo_2\"), \"\";");
	query.exec("INSERT INTO test_case VALUES(NULL, 1, 2, \"Scripted test case 1\", \"\");");
	query.exec("INSERT INTO test_case VALUES(NULL, 2, 2, \"Second scripter case\", \"\");");


        

        return true;
    }


};
// ----------------------------------------------------------------------------
