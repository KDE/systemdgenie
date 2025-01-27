/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SYSTEMDUNIT_H
#define SYSTEMDUNIT_H

#include <QString>
#include <QDBusObjectPath>
#include <QDBusArgument>

// struct for storing units retrieved from systemd via DBus
struct SystemdUnit
{
  QString id, description, load_state, active_state, sub_state, following, job_type, unit_file, unit_file_status;
  QDBusObjectPath unit_path, job_path;
  uint job_id;
  
  // The == operator must be provided to use contains() and indexOf()
  // on QLists of this struct
  bool operator==(const SystemdUnit& right) const
  {
    if (id == right.id)
      return true;
    else
      return false;
  }
  SystemdUnit(){}

  SystemdUnit(QString newId)
  {
    id = newId;
  }
};
Q_DECLARE_METATYPE(SystemdUnit)

QDBusArgument &operator<<(QDBusArgument &argument, const SystemdUnit &unit);
const QDBusArgument &operator>>(const QDBusArgument &argument, SystemdUnit &unit);

// struct for storing sessions retrieved from logind via DBus
struct SystemdSession
{
  QString session_id, user_name, seat_id, session_state;
  QDBusObjectPath session_path;
  uint user_id;

  // The == operator must be provided to use contains() and indexOf()
  // on QLists of this struct
  bool operator==(const SystemdSession& right) const
  {
    if (session_id == right.session_id)
      return true;
    else
      return false;
  }
};
Q_DECLARE_METATYPE(SystemdSession)

QDBusArgument &operator<<(QDBusArgument &argument, const SystemdSession &session);
const QDBusArgument &operator>>(const QDBusArgument &argument, SystemdSession &session);

enum dbusBus
{
  sys, session, user
};

struct UnitFile
{
    QString name;
    QString status;

    bool operator==(const UnitFile& right) const
    {
        return name.section(QLatin1Char('/'), -1) == right.name;
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const UnitFile &unitFile);
const QDBusArgument &operator>>(const QDBusArgument &argument, UnitFile &unitFile);

#endif // SYSTEMDUNIT_H
