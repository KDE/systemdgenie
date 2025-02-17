// SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>                     *
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "sessionsfetchjob.h"

#include <QDBusMessage>
#include <QDBusPendingCallWatcher>

SessionsFetchJob::SessionsFetchJob(OrgFreedesktopLogin1ManagerInterface *loginManagerInterface, QObject *parent)
    : KJob(parent)
    , m_loginManagerInterface(loginManagerInterface)
{
    Q_ASSERT(loginManagerInterface);
}

void SessionsFetchJob::start()
{
    auto reply = m_loginManagerInterface->ListSessions();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
                     &SessionsFetchJob::slotListSessionsFinished);
}

void SessionsFetchJob::slotListSessionsFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QList<SystemdSession>> reply = *call;
    if (reply.isError()) {
        setErrorText(reply.error().message());
        setError(KJob::UserDefinedError);
        Q_EMIT emitResult();
        call->deleteLater();
        return;
    } else {
        m_sessions = reply.value();
    }
    Q_EMIT emitResult();
    call->deleteLater();
}


QList<SystemdSession> SessionsFetchJob::sessions() const
{
    return m_sessions;
}
