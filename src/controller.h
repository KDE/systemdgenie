// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "sessionmodel.h"
#include "systemdunit.h"
#include "unitmodel.h"
#include <QObject>
#include <QStandardItemModel>
#include <qqmlregistration.h>

class OrgFreedesktopSystemd1ManagerInterface;
class OrgFreedesktopLogin1ManagerInterface;

class Controller : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(UnitModel *systemUnitModel READ systemUnitModel CONSTANT)
    Q_PROPERTY(UnitModel *userUnitModel READ systemUnitModel CONSTANT)
    Q_PROPERTY(SessionModel *sessionModel READ sessionModel CONSTANT)

public:
    explicit Controller(QObject *parent = nullptr);

    UnitModel *systemUnitModel() const;
    UnitModel *userUnitModel() const;
    SessionModel *sessionModel() const;

    SystemdUnit systemUnit(int index) const;
    SystemdUnit userUnit(int index) const;

    Q_SLOT void slotRefreshUserUnitsList();
    Q_SLOT void slotRefreshSystemUnitsList();
    Q_SLOT void slotRefreshSessionList();
    void slotRefreshUnitsList(dbusBus bus);

    int nonActiveUserUnits() const;
    int nonActiveSystemUnits() const;

Q_SIGNALS:
    void errorOccured(const QString &error);
    void message(const QString &text);
    void systemUnitsRefreshed();
    void userUnitsRefreshed();
    void sessionsRefreshed();

private:
    // user slots
    void slotUserJobNew(uint id, const QDBusObjectPath &path, const QString &unit);
    void slotUserJobRemoved(uint id, const QDBusObjectPath &path, const QString &unit, const QString &result);
    void slotUserUnitFilesChanged();
    Q_SLOT void slotUserPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps);
    void slotUserSystemdReloading(bool status);

    // system slots
    void slotSystemJobNew(uint id, const QDBusObjectPath &path, const QString &unit);
    void slotSystemJobRemoved(uint id, const QDBusObjectPath &path, const QString &unit, const QString &result);
    void slotSystemUnitFilesChanged();
    Q_SLOT void slotSystemPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps);
    void slotSystemSystemdReloading(bool status);

    UnitModel *m_systemUnitModel;
    UnitModel *m_userUnitModel;
    SessionModel *const m_sessionModel;
    QString m_userBusPath;
    int m_noActUserUnits = 0;
    int m_noActSystemUnits = 0;

    OrgFreedesktopSystemd1ManagerInterface *const m_systemManagerInterface;
    OrgFreedesktopSystemd1ManagerInterface *m_sessionManagerInterface = nullptr;
};
