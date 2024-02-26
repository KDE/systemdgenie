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

#ifndef SORTFILTERUNITMODEL_H
#define SORTFILTERUNITMODEL_H

#include <QSortFilterProxyModel>

enum filterType
{
    activeState, unitType, unitName
};

class SortFilterUnitModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit SortFilterUnitModel(QObject *parent = 0);
    void initFilterMap(const QMap<filterType, QString> &map);
    void addFilterRegExp(filterType type, const QString &pattern);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QMap<filterType, QString> filtersMap;
};

#endif // SORTFILTERUNITMODEL_H
