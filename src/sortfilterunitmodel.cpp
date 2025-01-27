/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sortfilterunitmodel.h"

#include <QDebug>
#include <QRegularExpression>

SortFilterUnitModel::SortFilterUnitModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void SortFilterUnitModel::initFilterMap(const QMap<filterType, QString> &map)
{
    filtersMap.clear();

    for(QMap<filterType, QString>::const_iterator iter = map.constBegin(); iter != map.constEnd(); ++iter)
    {
        filtersMap[iter.key()] = iter.value();
    }

}

void SortFilterUnitModel::addFilterRegExp(filterType type, const QString &pattern)
{
    if (!filtersMap.contains(type)) {
        return;
    }

    filtersMap[type] = pattern;
}

bool SortFilterUnitModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(filtersMap.isEmpty()) {
        return true;
    }

    bool ret = false;

    for(QMap<filterType, QString>::const_iterator iter = filtersMap.constBegin(); iter != filtersMap.constEnd(); ++iter)
    {
        QModelIndex indexActiveState = sourceModel()->index(sourceRow, 2, sourceParent);
        QModelIndex indexUnitName = sourceModel()->index(sourceRow, 0, sourceParent);

        if (iter.key() == activeState) {
            ret = (indexActiveState.data().toString().contains(QRegularExpression(iter.value())));
        } else if (iter.key() == unitType) {
            ret = (indexUnitName.data().toString().contains(QRegularExpression(iter.value())));
        } else if (iter.key() == unitName) {
            ret = (indexUnitName.data().toString().contains(QRegularExpression(iter.value(), QRegularExpression::CaseInsensitiveOption)));
        }

        if(!ret) {
            return ret;
        }
    }

    return ret;
}

