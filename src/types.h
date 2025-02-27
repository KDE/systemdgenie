// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>
#include <qqmlregistration.h>

#include "systemd_unit_interface.h"

class UnitInterface : public QObject
{
    Q_OBJECT
    QML_FOREIGN(OrgFreedesktopSystemd1UnitInterface)
    QML_ELEMENT
    QML_UNCREATABLE("")
};
