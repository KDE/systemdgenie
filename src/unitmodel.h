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
    explicit UnitModel(QObject *parent = nullptr);
    explicit UnitModel(QObject *parent = nullptr, const QVector<SystemdUnit> *list = NULL, QString userBusPath = QString());
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QStringList getLastJrnlEntries(QString unit) const;
    const QVector<SystemdUnit> *m_unitList;
    QString m_userBus;
};

#endif // UNITMODEL_H
