// SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>                     *
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "unitsfetchjob.h"

#include <QDBusMessage>
#include <QDBusPendingCallWatcher>

UnitsFetchJob::UnitsFetchJob(OrgFreedesktopSystemd1ManagerInterface *systemdManagerInterface, QObject *parent)
    : KJob(parent)
    , m_systemdManagerInterface(systemdManagerInterface)
{
    Q_ASSERT(systemdManagerInterface);
}

void UnitsFetchJob::start()
{
    auto reply = m_systemdManagerInterface->ListUnits();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &UnitsFetchJob::slotListUnitsFinished);
}

void UnitsFetchJob::slotListUnitsFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QList<SystemdUnit>> reply = *call;
    if (reply.isError()) {
        setErrorText(reply.error().message());
        setError(KJob::UserDefinedError);
        Q_EMIT emitResult();
        call->deleteLater();
        return;
    } else {
        m_units = reply.value();
    }
    call->deleteLater();

    auto replyFile = m_systemdManagerInterface->ListUnitFiles();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(replyFile, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &UnitsFetchJob::slotListUnitFilesFinished);
}

void UnitsFetchJob::slotListUnitFilesFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QList<UnitFile>> reply = *call;
    if (reply.isError()) {
        qWarning() << "Error fetching units from dbus" << reply.error();
        setErrorText(reply.error().message());
        setError(KJob::UserDefinedError);
        Q_EMIT emitResult();
        call->deleteLater();
        return;
    }

    const QList<UnitFile> unitFiles = reply.value();

    for (const auto &unitFile : unitFiles) {
        int index = m_units.indexOf(SystemdUnit(unitFile.name.section(QLatin1Char('/'), -1)));
        if (index > -1) {
            // The unit was already in the list, add unit file and its status
            m_units[index].unit_file = unitFile.name;
            m_units[index].unit_file_status = unitFile.status;
        } else {
            // Unit not in the list, add it.
            QFile unitfile(unitFile.name);

            if (unitfile.symLinkTarget().isEmpty()) {
                SystemdUnit unit;
                unit.id = unitFile.name.section(QLatin1Char('/'), -1);
                unit.load_state = QStringLiteral("unloaded");
                unit.active_state = QLatin1Char('-');
                unit.sub_state = QLatin1Char('-');
                unit.unit_file = unitFile.name;
                unit.unit_file_status = unitFile.status;
                m_units.append(unit);
            }
        }
    }
    call->deleteLater();

    Q_EMIT emitResult();
}

QList<SystemdUnit> UnitsFetchJob::units() const
{
    return m_units;
}

#include "moc_unitsfetchjob.cpp"
