// SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>                     *
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "systemdunit.h"

QDBusArgument &operator<<(QDBusArgument &argument, const SystemdUnit &unit)
{
    argument.beginStructure();
    argument << unit.id << unit.description << unit.load_state << unit.active_state << unit.sub_state << unit.following << unit.unit_path << unit.job_id
             << unit.job_type << unit.job_path;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, SystemdUnit &unit)
{
    argument.beginStructure();
    argument >> unit.id >> unit.description >> unit.load_state >> unit.active_state >> unit.sub_state >> unit.following >> unit.unit_path >> unit.job_id
        >> unit.job_type >> unit.job_path;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const SystemdSession &session)
{
    argument.beginStructure();
    argument << session.session_id << session.user_id << session.user_name << session.seat_id << session.session_path;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, SystemdSession &session)
{
    argument.beginStructure();
    argument >> session.session_id >> session.user_id >> session.user_name >> session.seat_id >> session.session_path;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const UnitFile &unitFile)
{
    argument.beginStructure();
    argument << unitFile.name << unitFile.status;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, UnitFile &unitFile)
{
    argument.beginStructure();
    argument >> unitFile.name >> unitFile.status;
    argument.endStructure();
    return argument;
}
