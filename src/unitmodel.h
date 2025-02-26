/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef UNITMODEL_H
#define UNITMODEL_H

#include "systemdunit.h"

#include <QAbstractTableModel>
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
    void setUnits(const QList<SystemdUnit> &units);

    Type type() const;
    void setType(Type type);

    int nonActiveUnits() const;

public Q_SLOTS:
    void executeUnitAction(int row, const QString &method);
    void slotRefreshUnitsList();

Q_SIGNALS:
    void typeChanged();
    void unitsRefreshed();
    void errorOccured(const QString &message);

private:
    void slotJobNew(uint id, const QDBusObjectPath &path, const QString &unit);
    void slotJobRemoved(uint id, const QDBusObjectPath &path, const QString &unit, const QString &result);
    void slotUnitFilesChanged();
    Q_SLOT void slotPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps);
    void slotReloading(bool status);

    QStringList getLastJrnlEntries(QString unit) const;
    QVector<SystemdUnit> m_units;
    QString m_userBus;
    Type m_type = Unknown;
    OrgFreedesktopSystemd1ManagerInterface *m_managerIface = nullptr;
    int m_nonActiveUnitsCount;
    bool m_refreshing = false;
};

#endif // UNITMODEL_H
