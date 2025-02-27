/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef UNITMODEL_H
#define UNITMODEL_H

#include "systemdunit.h"

#include "systemd_unit_interface.h"

#include <QAbstractTableModel>
#include <QDBusConnection>
#include <qqmlintegration.h>

class OrgFreedesktopSystemd1ManagerInterface;

class UnitModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int nonActiveUnits READ nonActiveUnits NOTIFY unitsRefreshed)

public:
    enum Type {
        Unknown,
        SystemUnits,
        UserUnits,
    };
    Q_ENUM(Type);

    enum UnitCapability {
        None = 0,
        CanStart = 1 << 0,
        CanStop = 1 << 1,
        CanReload = 1 << 2,
        LoadState = 1 << 3,
        ActiveState = 1 << 4,
        HasUnitObject = 1 << 5,
    };
    Q_ENUM(UnitCapability);
    Q_DECLARE_FLAGS(UnitCapabilities, UnitCapability)

    enum ExtraRoles {
        ColorRole = Qt::UserRole + 1,
        IconNameRole,
    };

    explicit UnitModel(QObject *parent = nullptr);
    explicit UnitModel(QString userBusPath, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    const QList<SystemdUnit> &unitsConst() const;
    QList<SystemdUnit> units() const;

    Type type() const;
    void setType(Type type);

    int nonActiveUnits() const;

    /// Caller take ownership of the returned OrgFreedesktopSystemd1ManagerInterface
    Q_INVOKABLE OrgFreedesktopSystemd1UnitInterface *unitObject(int row) const;

public Q_SLOTS:
    void executeUnitAction(int row, const QString &method);
    void slotRefreshUnitsList();

Q_SIGNALS:
    void typeChanged();
    void unitsRefreshed();
    void errorOccured(const QString &message);

private:
    void setUnits(const QList<SystemdUnit> &units);
    void slotJobNew(uint id, const QDBusObjectPath &path, const QString &unit);
    void slotJobRemoved(uint id, const QDBusObjectPath &path, const QString &unit, const QString &result);
    void slotUnitFilesChanged();
    Q_SLOT void slotPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps);
    void slotReloading(bool status);

    QStringList getLastJrnlEntries(QString unit) const;
    QDBusConnection m_connection;
    QVector<SystemdUnit> m_units;
    QString m_userBus;
    Type m_type = Unknown;
    OrgFreedesktopSystemd1ManagerInterface *m_managerIface = nullptr;
    int m_nonActiveUnitsCount;
    bool m_refreshing = false;
};

#endif // UNITMODEL_H
