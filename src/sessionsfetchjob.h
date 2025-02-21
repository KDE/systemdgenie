// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "login_manager_interface.h"
#include "systemdunit.h"
#include <KJob>

class SessionsFetchJob : public KJob
{
    Q_OBJECT

public:
    explicit SessionsFetchJob(OrgFreedesktopLogin1ManagerInterface *loginManagerInterface, QObject *parent = nullptr);

    void start() override;

    QVector<SystemdSession> sessions() const;

private:
    void slotListSessionsFinished(QDBusPendingCallWatcher *call);

    QVector<SystemdSession> m_sessions;
    OrgFreedesktopLogin1ManagerInterface *const m_loginManagerInterface;
};
