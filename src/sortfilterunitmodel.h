/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
    explicit SortFilterUnitModel(QObject *parent = nullptr);
    void initFilterMap(const QMap<filterType, QString> &map);
    void addFilterRegExp(filterType type, const QString &pattern);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QMap<filterType, QString> filtersMap;
};

#endif // SORTFILTERUNITMODEL_H
