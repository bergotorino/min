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

/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -c MinObject -p min_object.hpp:min_object.cpp /home/user/work/gmo/min/trunk/mindbusplugin/src/min_dbus_interface.xml
 *
 * dbusxml2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef MIN_OBJECT_HPP_H_1237812676
#define MIN_OBJECT_HPP_H_1237812676

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.maemo.MIN
 */
class MinObject: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.maemo.MIN"; }

public:
    MinObject(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~MinObject();

public Q_SLOTS: // METHODS
    inline QDBusReply<void> min_abort_case(qulonglong testrunid)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(testrunid);
        return callWithArgumentList(QDBus::Block, QLatin1String("min_abort_case"), argumentList);
    }

    inline QDBusReply<void> min_add_test_case_file(uint moduleid, const QString &testcasefile)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(moduleid) << qVariantFromValue(testcasefile);
        return callWithArgumentList(QDBus::Block, QLatin1String("min_add_test_case_file"), argumentList);
    }

    inline QDBusReply<void> min_add_test_module(const QString &modulepath)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(modulepath);
        return callWithArgumentList(QDBus::Block, QLatin1String("min_add_test_module"), argumentList);
    }

    inline QDBusReply<void> min_close()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("min_close"), argumentList);
    }

    inline QDBusReply<void> min_fatal_error(const QString &what, const QString &errorstring)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(what) << qVariantFromValue(errorstring);
        return callWithArgumentList(QDBus::Block, QLatin1String("min_fatal_error"), argumentList);
    }

    inline QDBusReply<void> min_open()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("min_open"), argumentList);
    }

    inline QDBusReply<void> min_pause_case(qulonglong testrunid)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(testrunid);
        return callWithArgumentList(QDBus::Block, QLatin1String("min_pause_case"), argumentList);
    }

    inline QDBusReply<void> min_query_test_files()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("min_query_test_files"), argumentList);
    }

    inline QDBusReply<void> min_query_test_modules()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("min_query_test_modules"), argumentList);
    }

    inline QDBusReply<void> min_resume_case(qulonglong testrunid)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(testrunid);
        return callWithArgumentList(QDBus::Block, QLatin1String("min_resume_case"), argumentList);
    }

    inline QDBusReply<void> min_start_case(uint moduleid, uint caseid, uint groupid)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(moduleid) << qVariantFromValue(caseid) << qVariantFromValue(groupid);
        return callWithArgumentList(QDBus::Block, QLatin1String("min_start_case"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void min_case_msg(qulonglong testrunid, const QString &message);
    void min_case_paused(qulonglong testrunid);
    void min_case_result(qulonglong testrunid, int result, const QString &desc, qulonglong starttime, qulonglong endttime);
    void min_case_resumed(qulonglong testrunid);
    void min_case_started(uint moduleid, uint caseid, qulonglong testrunid);
    void min_module_ready(uint moduleid);
    void min_new_module(const QString &modulename, uint moduleid);
    void min_new_test_case(uint moduleid, uint caseid, const QString &casetitle);
    void min_no_module(const QString &modulename);
    void min_test_files(const QString &files);
    void min_test_modules(const QString &modules);
};

namespace org {
  namespace maemo {
    typedef ::MinObject MIN;
  }
}
#endif
