// SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef UNITMODEL_H
#define UNITMODEL_H

#include "systemd_unit_interface.h"
#include "systemdunit.h"

#include <QAbstractTableModel>
#include <QDBusConnection>
#include <QDBusContext>
#include <QTimer>
#include <qqmlintegration.h>

class OrgFreedesktopSystemd1ManagerInterface;

class UnitModel : public QAbstractTableModel, protected QDBusContext
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int activeUnitsCount READ activeUnitsCount NOTIFY unitsRefreshed)
    Q_PROPERTY(int count READ rowCount NOTIFY unitsRefreshed)

public:
    enum Type {
        Unknown,
        SystemUnits,
        UserUnits,
    };
    Q_ENUM(Type);

    enum ExtraRoles {
        ColorRole = Qt::UserRole + 1,
        IconNameRole,
        UnitPathRole,
    };
    Q_ENUM(ExtraRoles);

    enum Columns {
        UnitColumn,
        LoadStateColumn,
        ActiveStateColumn,
        UnitStateColumn,
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

    QDBusConnection connection() const;

    int activeUnitsCount() const;

    /// Caller take ownership of the returned OrgFreedesktopSystemd1ManagerInterface
    Q_INVOKABLE OrgFreedesktopSystemd1UnitInterface *unitObject(int row);

    Q_INVOKABLE QStringList lastJrnlEntries(const QString &unit) const;
    Q_INVOKABLE QString unitFile(int row) const;
    Q_INVOKABLE QString unitFileStatus(int row) const;

public Q_SLOTS:
    void executeUnitAction(int row, const QString &method);
    void slotRefreshUnitsList();

Q_SIGNALS:
    void typeChanged();
    void unitsRefreshed();
    void errorOccurred(const QString &message);

private:
    void setUnits(const QList<SystemdUnit> &units);
    void slotUnitFilesChanged();
    Q_SLOT void slotPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps);
    void slotReloading(bool status);
    void slotUnitNew(const QString &id, const QDBusObjectPath &unit);
    void slotUnitRemoved(const QString &id, const QDBusObjectPath &unit);
    void slotRefreshUnit(const QString &unit);

    QDBusConnection m_connection;
    QTimer *const m_refreshTimer;
    QVector<SystemdUnit> m_units;
    QString m_userBus;
    Type m_type = Unknown;
    OrgFreedesktopSystemd1ManagerInterface *m_managerIface = nullptr;
    QPointer<OrgFreedesktopSystemd1UnitInterface> m_selectedUnit;
    int m_activeUnitsCount;
    bool m_refreshing = false;
};

#endif // UNITMODEL_H
