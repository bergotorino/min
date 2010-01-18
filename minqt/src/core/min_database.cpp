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

#include <QDate>
#include <QFile>
#include <QDir>

// ----------------------------------------------------------------------------
Min::Database::Database()
    : db()
{
  
    QDir minhome = QDir (QDir::homePath()+"/.min/");
    if (!minhome.exists()) {
      if (!minhome.mkdir(QDir::homePath()+"/.min/")) {
	  qDebug ("Failed to create ~/.min/");
	  return;
	}
    }
    QFile file(QDir::homePath()+"/.min/.qtinstance");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
      qDebug ("Failed to open qtinstance file");
      return;
    }

    QByteArray line = file.readLine();
    bool ok;
    unsigned qtinstance = line.toInt(&ok, 10);
    qtinstance++;

    instNum = qtinstance;
    line.setNum (qtinstance);
    file.seek(0);
    file.write (line.constData());
    file.close();
    initDatabase();

    this->groupCounter = 0;
};
// ----------------------------------------------------------------------------
Min::Database::~Database()
{
	db.close();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getGroup()
{
	return ++(this->groupCounter);
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertDevice(unsigned int device_id)
{
    // Check if device with this id exists
    QSqlQuery query;
    query.prepare("SELECT id FROM test_case WHERE device_id=:devid;");
    query.bindValue(":devid", QVariant(device_id));
    if (query.exec() && query.size()>0) return 0;
    query.finish();

    // Insert device id
    query.prepare("INSERT INTO device(device_id) VALUES (:devid);");
    query.bindValue(":devid", QVariant(device_id));
    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();
    query.finish();

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
    query.prepare("SELECT id FROM module WHERE device_id=:devid "
		  "AND module_id=:modid AND module_name=:name "
		  "AND instance_id=:iid;");
    query.bindValue(QString(":devid"), QVariant(device_dbid));
    query.bindValue(QString(":modid"), QVariant(module_id));
    query.bindValue(QString(":name"), QVariant(module_name));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if (query.exec() && query.size()>0) return 0;
    query.finish();

    if (module_name.compare("scripter_cli") == 0)
	    return 0;
    // Insert module
    query.prepare("INSERT INTO module"
		  "(module_id, device_id, module_name, instance_id) "
		  "VALUES (:modid, :devid, :name, :iid);");
    query.bindValue(QString(":devid"), QVariant(device_dbid));
    query.bindValue(QString(":modid"), QVariant(module_id));
    query.bindValue(QString(":name"), QVariant(module_name));
    query.bindValue(QString(":iid"), QVariant(instNum));

    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();

    // Notify
    if (retval) emit updated();
    query.finish();

    // Return id of last insert
    return retval;
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertTestCaseFile(const QString &case_file_name)
{
    QSqlQuery query;

    // Insert test case file
    query.prepare("INSERT INTO test_case_file(test_case_file_name) VALUES (:casefile);");
    query.bindValue(QString(":casefile"), QVariant(case_file_name));
    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();

    query.finish();

    // Return id of last insert
    return retval;
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertTestModuleFile(const QString &module_file_name)
{
    QSqlQuery query;

    // Insert test case file
    query.prepare("INSERT INTO test_module_file(test_module_file_name) "
		  "VALUES (:modfile);");
    query.bindValue(QString(":modfile"), QVariant(module_file_name));
    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();

    query.finish();

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
    // untaint
    QString tctitle(test_case_title);
    if (tctitle.contains ('\"'))
	    tctitle.remove ('\"');
    
    query.prepare("SELECT id FROM test_case "
		  "WHERE module_id=:modid AND module_name=:title "
		  "AND test_case_id=:caseid AND test_case_description=:descr "
		  "AND instance_id=:iid;");
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    query.bindValue(QString(":modid"), QVariant(module_dbid));
    query.bindValue(QString(":title"), QVariant(tctitle));
    query.bindValue(QString(":descr"), QVariant(test_case_description));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if (query.exec() && query.size()>0) return 0;
    query.finish();

    // Insert test case
    query.prepare("INSERT INTO test_case"
		  "(test_case_id, module_id, test_case_title, "
		  "test_case_description, instance_id) "
		  "VALUES (:caseid, :modid, :title, :descr, :iid);");
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    query.bindValue(QString(":modid"), QVariant(module_dbid));
    query.bindValue(QString(":title"), QVariant(tctitle));
    query.bindValue(QString(":descr"), QVariant(test_case_description));
    query.bindValue(QString(":iid"), QVariant(instNum));

    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();
    query.finish();

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
    //    qDebug ("insertTestRun: dbid = %u, test_run_id = %u, group_id = %u, "
    //    "status = %d, start_time = %lu",
    //    test_case_dbid, test_run_pid, group_id, status, start_time);
    query.prepare("INSERT INTO test_run"
		  "(test_run_pid, test_case_id, group_id, "
		  "status, start_time, instance_id) "
		  "VALUES (:runpid, :caseid, :groupid, "
		  ":status, :starttime, :iid);");
    query.bindValue(QString(":caseid"), QVariant(test_case_dbid));
    query.bindValue(QString(":runpid"), QVariant(test_run_pid));
    query.bindValue(QString(":groupid"), QVariant(group_id));
    query.bindValue(QString(":status"), QVariant(status));
    query.bindValue(QString(":starttime"), QVariant( (qlonglong)start_time));
    query.bindValue(QString(":iid"), QVariant(instNum));

    unsigned int retval = 0;
    if (query.exec()) retval = query.lastInsertId().toUInt();
    query.finish();

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
    // untaint
    QString cont(content);
    if (cont.contains ('\"'))
	    cont.remove ('\"');
    
    query.prepare("INSERT INTO printout(test_run_id, content, instance_id) "
		  "VALUES (:runid, :content, :iid);");
    query.bindValue(QString(":runid"), QVariant(test_run_dbid));
    query.bindValue(QString(":content"), QVariant(cont));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if (query.exec()) {
        // Notify
        query.finish();
        emit updated();
        return query.lastInsertId().toUInt();
    }
    else return 0;
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::insertLogMessage(const QString &logtype,
					     const QString &content)
{
    QSqlQuery query;
    // untaint
    QString cont(content);
    if (cont.contains ('\"'))
	    cont.remove ('\"');
    
    query.prepare("INSERT INTO log(logtype,content,logtime) "
		  "VALUES (:logtype, :content, :logtime);");
    query.bindValue(QString(":logtype"), QVariant(logtype));
    query.bindValue(QString(":content"), QVariant(cont));
    query.bindValue(QString(":logtime"), 
		    QDateTime::currentDateTime().toTime_t());

    if (query.exec()) {
        // Notify
        query.finish();
        emit updated();
	if (logtype.compare("error") == 0)
		emit errors_updated();

        return query.lastInsertId().toUInt();
    }
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

    // Update existing test run
    QSqlQuery query;
    QString raw_query("");
    QString result_d (result_description);

    //qDebug ("updateTestRun: %u %d %ld %ld %d %s",
    //    dbid, status, start_time, end_time, result, 
    //    result_description.toStdString().c_str());
    if (result_d.contains ('\"'))
	    result_d.remove ('\"');
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
        raw_query.append(result_d);
        raw_query.append("\"");
    }
    raw_query.append(" WHERE id=");
    raw_query.append(QString::number(dbid));
    raw_query.append(" AND instance_id=");
    raw_query.append(QString::number(instNum));
    raw_query.append(" ;");
    bool retval = query.exec(raw_query);
    if (!retval)
      qDebug ("query failed! %s", query.lastQuery().toStdString().c_str());
    // Notify
    if (retval) emit updated();
    query.finish();
    // Return status of update
    return retval;
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::updateCaseDesc (unsigned int module_dbid,
					    unsigned int test_case_id,
					    const QString &test_case_description)
{
    // Update existing test run
    QSqlQuery query;
    QString raw_query("");
    QString desc (test_case_description);

    if (desc.contains ('\"'))
	    desc.remove ('\"');
    raw_query.append("UPDATE test_case SET test_case_description=\"");
    
    raw_query.append(desc);
    raw_query.append("\"");

    raw_query.append(" WHERE id=");
    raw_query.append(QString::number(getTestCaseDbId(module_dbid, 
						     test_case_id)));
    raw_query.append(" ;");
    unsigned int retval = query.exec(raw_query);
    if (!retval)
      qDebug ("query failed! %s", query.lastQuery().toStdString().c_str());
    // Notify
    if (retval) emit updated();
    query.finish();
    // Return status of update
    return retval;

}
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
    query.finish();

    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getModuleDbId(unsigned int device_id,
                                        unsigned int module_id)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM module "
		  "WHERE device_id=:devid AND module_id=:modid "
		  "AND instance_id=:iid;");
    query.bindValue(QString(":devid"), QVariant(device_id));
    query.bindValue(QString(":modid"), QVariant(module_id));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    query.finish();

    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getModuleDbId(unsigned int device_id,
                                        const QString &module_name)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM module "
		  "WHERE device_id=:devid AND module_name=:modname "
		  "AND instance_id=:iid;");
    query.bindValue(QString(":devid"), QVariant(device_id));
    query.bindValue(QString(":modname"), QVariant(module_name));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    query.finish();

    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestRunEngineId(unsigned int testRunDbId)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT test_run_pid FROM test_run WHERE id=:trdbid;");
    query.bindValue(QString(":trdbid"), QVariant(testRunDbId));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    query.finish();

    return id.toUInt();
}
// ----------------------------------------------------------------------------
unsigned int Min::Database::getModuleEngineId(unsigned int deviceId,
                                            unsigned int moduleDbId)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT module_id FROM module "
		  "WHERE device_id=:devid AND id=:moddbid "
		  "AND instance_id=:iid;");
    query.bindValue(QString(":devid"), QVariant(deviceId));
    query.bindValue(QString(":moddbid"), QVariant(moduleDbId));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    query.finish();

    return id.toUInt();
}
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestCaseDbId(unsigned int module_dbid,
					    unsigned int test_case_id)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM test_case "
		  "WHERE module_id=:modid AND test_case_id=:caseid "
		  "AND instance_id=:iid;");
    query.bindValue(QString(":modid"), QVariant(module_dbid));
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    query.finish();

    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestCaseDbId(unsigned int module_id,
					    const QString &test_case_name)
{

    QSqlQuery query;
    QVariant id(0);
    query.prepare("SELECT id FROM test_case "
		  "WHERE module_id=:modid AND test_case_title=:casename "
		  "AND instance_id=:iid;");
    query.bindValue(QString(":modid"), QVariant(module_id));
    query.bindValue(QString(":casename"), QVariant(test_case_name));
    query.bindValue(QString(":iid"), QVariant(instNum));

//     QMapIterator<QString, QVariant> i(query.boundValues());
//     while (i.hasNext()) {
//        i.next();
//        qDebug ("getTestCaseDbId: %s:%s", i.key().toAscii().data(),
// 	       i.value().toString().toAscii().data());
//     }

    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        } 
    }
    query.finish();
    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestCaseEngineId(unsigned int moduleDbId,
                                                unsigned int testCaseDbId)
{
    QSqlQuery query;
    QVariant id(0);
    query.prepare("SELECT test_case_id FROM test_case "
		  "WHERE module_id=:moddbid AND id=:casedbid "
		  "AND instance_id=:iid;");
    query.bindValue(QString(":moddbid"), QVariant(moduleDbId));
    query.bindValue(QString(":casedbid"), QVariant(testCaseDbId));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
	    //	    qDebug ("query ok");
	    if(query.next()) {
		    //    qDebug ("query.next ok");
		    id=query.value(0);
	    }
    }
    query.finish();

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
    query.finish();
    return id.toUInt();
};
// ----------------------------------------------------------------------------
unsigned int Min::Database::getTestRunDbId(unsigned int test_case_id,
					   unsigned int test_run_pid)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT id FROM test_run "
		  "WHERE test_case_id=:caseid AND test_run_pid=:runpid;");
    query.bindValue(QString(":runpid"), QVariant(test_run_pid));
    query.bindValue(QString(":caseid"), QVariant(test_case_id));
    if(query.exec()){
        if(query.next()) {
            id=query.value(0);
        }
    }
    query.finish();
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
    query.finish();
    return retval;
};
// ----------------------------------------------------------------------------
QStringList Min::Database::getTestCaseFiles()
{
    QSqlQuery query;
    QStringList retval;
    query.prepare("SELECT test_case_file_name FROM test_case_file");
    if(query.exec()){
        while(query.next()) {
            retval.append(query.value(0).toString());
        }
    }
    query.finish();
    return retval;
};
// ----------------------------------------------------------------------------
QStringList Min::Database::getTestModuleFiles()
{
    QSqlQuery query;
    QStringList retval;
    query.prepare("SELECT test_module_file_name FROM test_module_file");
    if(query.exec()){
        while(query.next()) {
            retval.append(query.value(0).toString());
        }
    }
    query.finish();
    return retval;
};
// ----------------------------------------------------------------------------
QStringList Min::Database::getTestCases(unsigned int module_dbid)
{
    QSqlQuery query;
    QStringList retval;
    query.prepare("SELECT test_case_title FROM test_case "
		  "WHERE module_id=:modid;");
    query.bindValue(QString(":modid"), QVariant(module_dbid) );
    if(query.exec()){
        while(query.next()) {
            retval.append(query.value(0).toString());
        }
    }
    query.finish();
    return retval;

};
// ----------------------------------------------------------------------------
QList<unsigned int> Min::Database::getTestRunGroups()
{
    QSqlQuery query;
    QList<unsigned int> retval;
    query.prepare("SELECT DISTINCT group_id FROM test_run;");
    if(query.exec()){
        while(query.next()) {
            retval.append(query.value(0).toUInt());
        }
    }
    query.finish();

    return retval;

};
// ----------------------------------------------------------------------------
QVector<QStringList> Min::Database::getAvailableView(unsigned int devid) const
{
    QSqlQuery query;
    QVector<QStringList> retval;
    QStringList row;
    query.prepare("SELECT module_name, test_case_title, "
		  "test_case_description, test_case_dbid, module_dbid "
		  "FROM availableview "
		  "WHERE device_dbid=:devdbid AND instance_id=:iid;");
    query.bindValue(QString(":devdbid"), QVariant(devid));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
        while(query.next()) {
	    row.clear();
            row.append(query.value(0).toString());
            row.append(query.value(1).toString());
            row.append(query.value(2).toString());
            row.append(query.value(3).toString());
            row.append(query.value(4).toString());
	    retval.append(row);
        }
    }
    query.finish();

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
    query.finish();

    return retval;
};
// ----------------------------------------------------------------------------
QVector<QStringList> Min::Database::getLogMessages(void) const
{
    QSqlQuery query;
    QVector<QStringList> retval;
    QStringList row;

    query.prepare("SELECT * FROM log");
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
    query.finish();

    return retval;
};
// ----------------------------------------------------------------------------
QVector<QStringList> Min::Database::getTestRunsInGroup(unsigned int device_dbid, unsigned int group_id) const
{
    QSqlQuery query;
    QVector<QStringList> retval;
    QStringList row;
    query.prepare("SELECT * FROM executedview "
		  "WHERE device_dbid=:devdbid AND group_id=:grpid;");
    query.bindValue(QString(":grpid"), QVariant(group_id));
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
    query.finish();

    return retval;
};
/// ----------------------------------------------------------------------------
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
    query.finish();

    return retval;
};
// ----------------------------------------------------------------------------
QString Min::Database::getModuleNameFromEngineId(unsigned int moduleEngineId)
{
    QSqlQuery query;
    QVariant id="";
    query.prepare("SELECT module_name FROM module WHERE module_id=:moduleid AND instance_id=:iid;");
    query.bindValue(QString(":moduleid"), QVariant(moduleEngineId));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
        if(query.next()) { id=query.value(0); }
    }
    query.finish();

    return id.toString();
}
// ----------------------------------------------------------------------------
QString Min::Database::getModuleNameFromDbId(unsigned int moduleDbId)
{
    QSqlQuery query;
    QVariant id="";
    query.prepare("SELECT module_name "
		  "FROM module WHERE id=:moduleid AND instance_id=:iid;");
    query.bindValue(QString(":moduleid"), QVariant(moduleDbId));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
        if(query.next()) { id=query.value(0); }
    }
    query.finish();

    return id.toString();
}
// ----------------------------------------------------------------------------
QString Min::Database::getCaseTitleFromEngineId(unsigned int moduleDbId,
                                                unsigned int caseEngineId)
{
    QSqlQuery query;
    QVariant id="";
    query.prepare("SELECT test_case_title FROM test_case WHERE "
                  "test_case_id=:caseid AND module_id=:moduleid "
		  "AND instance_id=:iid;");
    query.bindValue(QString(":caseid"), QVariant(caseEngineId));
    query.bindValue(QString(":moduleid"), QVariant(moduleDbId));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
        if(query.next()) { id=query.value(0); }
    }
    query.finish();

    return id.toString();
}
// ----------------------------------------------------------------------------
QString Min::Database::getCaseTitleFromDbId(unsigned int moduleDbId,
                                            unsigned int caseDbId)
{
    QSqlQuery query;
    QVariant id="";
    query.prepare("SELECT test_case_title FROM test_case WHERE "
                  "id=:caseid AND module_id=:moduleid AND instance_id=:iid; ");
    query.bindValue(QString(":caseid"), QVariant(caseDbId));
    query.bindValue(QString(":moduleid"), QVariant(moduleDbId));
    query.bindValue(QString(":iid"), QVariant(instNum));

    if(query.exec()){
        if(query.next()) { id=query.value(0); }
    }
    query.finish();

    return id.toString();
}
// ----------------------------------------------------------------------------
unsigned int Min::Database::getModuleIdFromCaseDbId(unsigned int caseDbId)
{
    QSqlQuery query;
    QVariant id=0;
    query.prepare("SELECT module_id FROM test_case "
		  "WHERE id=:caseid AND instance_id=:iid;");
    query.bindValue(QString(":caseid"), QVariant(caseDbId));
    query.bindValue(QString(":iid"), QVariant(instNum));
    if(query.exec()){
        if(query.next()) { id=query.value(0); }
    }
    query.finish();

    return id.toUInt();
}
// ----------------------------------------------------------------------------
QString Min::Database::getCaseTitleFromTestrunDbId(unsigned int testrunDbId)
{
    QSqlQuery query;
    QVariant id="";
    query.prepare("SELECT test_case_title FROM executed_view "
		  "WHERE test_run_dbid=:testrunid;");
    query.bindValue(QString(":testrunid"), QVariant(testrunDbId));
    if(query.exec()){
        if(query.next()) { id=query.value(0); }
    }
    query.finish();

    return id.toString();
}
// ----------------------------------------------------------------------------
void Min::Database::clearDevice()
{
	QSqlQuery query;
        query.exec("DELETE FROM test_case;");
        query.exec("DELETE FROM module;");
	query.exec("DELETE FROM printout;");
	query.exec("DELETE FROM device;");
}

// ----------------------------------------------------------------------------
bool Min::Database::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(".min.db");
    //    db.setDatabaseName(":memory:");
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
	query.exec("DELETE FROM log;");
	query.exec("DELETE FROM test_case_file;");
	query.exec("DELETE FROM test_module_file;");

	/* clean-up some old-data */
	if (instNum > 2) {
	        query.prepare("DELETE from module WHERE instance_id<:iid");
		query.bindValue(QString(":iid"), QVariant(instNum-1));
		query.exec();
	        query.prepare("DELETE from test_case WHERE instance_id<:iid");
		query.bindValue(QString(":iid"), QVariant(instNum-1));
		query.exec();
	        query.prepare("DELETE from test_run WHERE instance_id<:iid");
		query.bindValue(QString(":iid"), QVariant(instNum-1));
		query.exec();
	        query.prepare("DELETE from printout WHERE instance_id<:iid");
		query.bindValue(QString(":iid"), QVariant(instNum-1));
		query.exec();
	}
	if (instNum == 0) {
	        /* Amazing: instance number wrap around */
	        query.exec("DELETE * FROM module");
		query.exec("DELETE * FROM test_case");
		query.exec("DELETE * FROM test_run");
		query.exec("DELETE * FROM printout");
	}
        query.exec("CREATE TABLE device "
		   "(id INTEGER PRIMARY KEY, "
		   "device_id int, "
		   "instance_id int);");
        query.exec("CREATE TABLE module "
		   "(id INTEGER PRIMARY KEY, "
		   "module_id int, "
		   "device_id int, "
		   "module_name varchar, "
		   "instance_id int);");
        query.exec("CREATE TABLE test_case "
		   "(id INTEGER PRIMARY KEY, "
		   "test_case_id int, "
		   "module_id int, "
		   "test_case_title varchar, "
		   "test_case_description varchar, "
		   "instance_id int);");
        query.exec("CREATE TABLE test_run "
		   "(id INTEGER PRIMARY KEY, "
		   "test_run_pid int, "
		   "test_case_id int, "
		   "group_id int, "
		   "status int, "
		   "start_time int, "
		   "end_time int, "
		   "result int default 2, "
		   "result_description varchar, "
		   "instance_id int);");
        query.exec("CREATE TABLE printout "
		   "(id INTEGER PRIMARY KEY, "
		   "test_run_id int, "
		   "content varchar, "
		   "instance_id int);");
        query.exec("CREATE TABLE log "
		   "(id INTEGER PRIMARY KEY, "
		   "content varchar, "
		   "logtype varchar, "
		   "logtime int);");

	query.exec("CREATE TABLE test_case_file "
		   "(test_case_file_name varchar);");
	query.exec("CREATE TABLE test_module_file "
		   "(test_module_file_name varchar);");

	query.exec("CREATE VIEW availableview AS "
		   "SELECT module.device_id AS device_dbid,"
		   "module.module_name AS module_name,"
		   "module.instance_id AS instance_id,"
		   "test_case.test_case_title AS test_case_title,"
		   "test_case.test_case_description "
		   " AS test_case_description,"
		   "test_case.id AS test_case_dbid,"
		   "test_case.module_id AS module_dbid "
		   "FROM test_case, module "
		   "WHERE module.id=test_case.module_id;");

	query.exec("CREATE VIEW executedview AS "
		   "SELECT test_case.test_case_title AS test_case_title, "
		   "test_case.test_case_description AS test_case_description, "
		   "test_run.group_id AS group_id, "
		   "test_run.status AS status, "
		   "test_run.start_time AS start_time, "
		   "test_run.end_time AS end_time, "
		   "test_run.result AS result, "
		   "test_run.result_description AS result_description, "
		   "test_run.id AS test_run_dbid, "
		   "module.device_id AS device_dbid "
		   "FROM module, test_case, test_run "
		   "WHERE test_run.test_case_id=test_case.id "
		   "AND module.id=test_case.module_id;");
	query.finish();

        return true;
    }


};
// ----------------------------------------------------------------------------
