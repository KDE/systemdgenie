// SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>                     *
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "systemdunit.h"

bool SystemdUnit::update(const SystemdUnit &newOne)
{
    if (this->description == newOne.description && this->load_state == newOne.load_state && this->active_state == newOne.active_state
        && this->sub_state == newOne.sub_state && this->following == newOne.following && this->job_type == newOne.job_type
        && this->unit_file == newOne.unit_file && this->unit_file_status == newOne.unit_file_status && this->unit_path == newOne.unit_path
        && this->job_path == newOne.job_path) {
        return false;
    }
    this->description = newOne.description;
    this->load_state = newOne.load_state;
    this->active_state = newOne.active_state;
    this->sub_state = newOne.sub_state;
    this->following = newOne.following;
    this->job_type = newOne.job_type;
    this->unit_file = newOne.unit_file;
    this->unit_file_status = newOne.unit_file_status;
    this->unit_path = newOne.unit_path;
    this->job_path = newOne.job_path;
    return true;
}

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
