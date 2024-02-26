/*******************************************************************************
 * Copyright (C) 2016 Ragnar Thomsen <rthomsen6@gmail.com>                     *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the Free  *
 * Software Foundation, either version 2 of the License, or (at your option)   *
 * any later version.                                                          *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
 * more details.                                                               *
 *                                                                             *
 * You should have received a copy of the GNU General Public License along     *
 * with this program. If not, see <http://www.gnu.org/licenses/>.              *
 *******************************************************************************/

#ifndef UNITMODEL_H
#define UNITMODEL_H

#include "systemdunit.h"

#include <QAbstractTableModel>

class UnitModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit UnitModel(QObject *parent = 0);
    explicit UnitModel(QObject *parent = 0, const QVector<SystemdUnit> *list = NULL, QString userBusPath = QString());
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

private:
    QStringList getLastJrnlEntries(QString unit) const;
    const QVector<SystemdUnit> *m_unitList;
    QString m_userBus;
};

#endif // UNITMODEL_H

