/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef UNITMODEL_H
#define UNITMODEL_H

#include "systemdunit.h"

#include <QAbstractTableModel>

class UnitModel : public QAbstractTableModel
{
    Q_OBJECT

public:
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

private:
    QStringList getLastJrnlEntries(QString unit) const;
    QVector<SystemdUnit> m_units;
    QString m_userBus;
};

#endif // UNITMODEL_H
