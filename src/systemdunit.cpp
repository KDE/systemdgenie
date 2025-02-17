/*******************************************************************************
 * Copyright (C) 2016 Ragnar Thomsen <rthomsen6@gmail.com>                     *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the Free  *
 * Software Foundation, either version 2 of the License, or (at your option)   *
 * any later version.                                                          *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
 * more details.                                                               *
 *                                                                             *
 * You should have received a copy of the GNU General Public License along     *
 * with this program. If not, see <http://www.gnu.org/licenses/>.              *
 *******************************************************************************/

#include "systemdunit.h"

QDBusArgument &operator<<(QDBusArgument &argument, const SystemdUnit &unit)
{
    argument.beginStructure();
    argument << unit.id
             << unit.description
             << unit.load_state
             << unit.active_state
             << unit.sub_state
             << unit.following
             << unit.unit_path
             << unit.job_id
             << unit.job_type
             << unit.job_path;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, SystemdUnit &unit)
{
    argument.beginStructure();
    argument >> unit.id
            >> unit.description
            >> unit.load_state
            >> unit.active_state
            >> unit.sub_state
            >> unit.following
            >> unit.unit_path
            >> unit.job_id
            >> unit.job_type
            >> unit.job_path;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const SystemdSession &session)
{
    argument.beginStructure();
    argument << session.session_id
             << session.user_id
             << session.user_name
             << session.seat_id
             << session.session_path;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, SystemdSession &session)
{
    argument.beginStructure();
    argument >> session.session_id
            >> session.user_id
            >> session.user_name
            >> session.seat_id
            >> session.session_path;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const UnitFile &unitFile)
{
    argument.beginStructure();
    argument << unitFile.name
             << unitFile.status;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, UnitFile &unitFile)
{
    argument.beginStructure();
    argument >> unitFile.name
            >> unitFile.status;
    qWarning() << unitFile.name;
    argument.endStructure();
    return argument;
}
