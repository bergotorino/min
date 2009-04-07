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

#inlcude <QVariant>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>


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
    query.prepare("INSERT INTO device(device_id) VALUES (:devid);");
    query.bindValue(":devid", QVariant(device_id));
    if(query.exec()){
        return query.lastInsertId().toUInt();
    }else{
        return 0;
    }


};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertModule(unsigned int device_id,
                unsigned int module_id,
                QString module_name)
{
    QSqlQuery query;
    query.prepare("INSERT INTO module(module_id, device_id, module_name) VALUES (:modid, :devid, :name);");
    query.bindValue(":devid", QVariant(device_id));
    query.bindValue(":modid", QVariant(module_id));
    query.bindValue(":name", QVariant(module_name));
    if(query.exec()){
        return query.lastInsertId().toUInt();
    }else{
        return 0;
    }

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertTestCase(unsigned int module_id,
                unsigned int test_case_id,
                QString test_case_title)
{
    QSqlQuery query;
    query.prepare("INSERT INTO test_case(test_case_id, module_id, test_case_title) VALUES (:caseid, :modid, :title);");
    query.bindValue(":caseid", QVariant(test_case_id));
    query.bindValue(":modid", QVariant(module_id));
    query.bindValue(":title", QVariant(test_case_title));
    if(query.exec()){
        return query.lastInsertId().toUInt();
    }else{
        return 0;
    }

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertTestRun(unsigned int test_case_id,
                unsigned int test_run_pid,
                unsigned int group_id,
                int status,
                unsigned long start_time)
{
    QSqlQuery query;
    query.prepare("INSERT INTO test_run(test_run_pid, test_case_id, group_id, status, start_time)  VALUES (:runpid, :caseid, :groupid, :status, :starttime);");
    query.bindValue(":caseid", QVariant(test_case_id));
    query.bindValue(":runpid", QVariant(test_run_pid));
    query.bindValue(":groupid", QVariant(group_id));
    query.bindValue(":status", QVariant(status));
    query.bindValue(":starttime", QVariant(start_time));
    if(query.exec()){
        return query.lastInsertId().toUInt();
    }else{
        return 0;
    }
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertPrintout(unsigned int test_run_pid,
                QString content)
{
    QSqlQuery query;
    query.prepare("INSERT INTO printout(test_run_id, content) VALUES (:runid, :content);");
    query.bindValue(":runid", QVariant(test_run_pid));
    query.bindValue(":content", QVariant(content));
    if(query.exec()){
        return query.lastInsertId().toUInt();
    }else{
        return 0;
    }

};
// ----------------------------------------------------------------------------
bool Min::Database::updateDevice(unsigned int id,
                unsigned int new_device_id)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
bool Min::Database::updateModule(unsigned int id,
                unsigned int device_id,
                unsigned int module_id,
                QString module_name)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
bool Min::Database::updateTestCase(unsigned int id,
                unsigned int module_id,
                unsigned int test_case_id,
                QString test_case_title)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
bool Min::Database::updateTestRun(unsigned int id,
                unsigned int test_case_id,
                unsigned int test_run_pid,
                unsigned int group_id,
                int status,
                unsigned long start_time,
                unsigned long end_time,
                int result,
                QString result_description)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
bool Min::Database::updatePrintout(unsigned int id,
                unsigned int test_run_id,
                QString content)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getDeviceId(unsigned int device_id)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getModuleId(unsigned int device_id,
                unsigned int module_id)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getModuleId(unsigned int device_id,
                QString module_name)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestCaseId(unsigned int module_id,
                unsigned int test_case_id)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestCaseId(unsigned int module_id,
                QString test_case_name)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestRunId(unsigned int test_case_id,
                test_run_pid)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
QStringList Min::Database::getModules(unsigned int device_id)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
QStringList Min::Database::getTestCases(unsigned int module_id)
{
    //TODO: fill method

};
// ----------------------------------------------------------------------------
bool initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish MIN database backend.\n"
                     "This plugin needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }else{
        QSqlQuery query;
        query.exec("CREATE TABLE device (id INTEGER PRIMARY KEY, device_id int);");
        query.exec("CREATE TABLE module (id INTEGER PRIMARY KEY, module_id int, device_id int, module_name varchar);");
        query.exec("CREATE TABLE test_case (id INTEGER PRIMARY KEY, test_case_id int, module_id int, test_case_title varchar);");
        query.exec("CREATE TABLE test_run (id INTEGER PRIMARY KEY, test_run_pid int, test_case_id int, group_id int, status int, start_time int, end_time int, result int, result_description varchar);");
        query.exec("CREATE TABLE printout (id INTEGER PRIMARY KEY, test_run_id int, content varchar);");

        /* Demo data */
        /*
        query.exec("INSERT INTO device VALUES (NULL, 10);");
        query.exec("INSERT INTO device VALUES (NULL, 20);");
        query.exec("INSERT INTO module VALUES (NULL, 1, 10, \"minDemoModule\");");
        query.exec("INSERT INTO module VALUES (NULL, 2, 10, \"minDemo2\");");
        query.exec("INSERT INTO module VALUES (NULL, 1, 20, \"minDemoModule\");");
        query.exec("INSERT INTO module VALUES (NULL, 2, 20, \"minDemo2\");");
        */

        return true;
    }


};
// ----------------------------------------------------------------------------
