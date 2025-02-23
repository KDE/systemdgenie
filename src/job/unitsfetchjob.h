// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "systemd_manager_interface.h"
#include "systemdunit.h"
#include <KJob>

class UnitsFetchJob : public KJob
{
    Q_OBJECT

public:
    explicit UnitsFetchJob(OrgFreedesktopSystemd1ManagerInterface *systemdManagerInterface, QObject *parent = nullptr);

    void start() override;

    QVector<SystemdUnit> units() const;

private:
    void slotListUnitsFinished(QDBusPendingCallWatcher *call);
    void slotListUnitFilesFinished(QDBusPendingCallWatcher *call);

    QVector<SystemdUnit> m_units;
    OrgFreedesktopSystemd1ManagerInterface *const m_systemdManagerInterface;
};
