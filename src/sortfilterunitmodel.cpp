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
    setDynamicSortFilter(false);
}

void SortFilterUnitModel::addFilterRegExp(FilterType type, const QString &pattern)
{
    filtersMap[type] = pattern;
}

bool SortFilterUnitModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (filtersMap.isEmpty()) {
        return true;
    }

    bool ret = false;

    for (auto iter = filtersMap.constBegin(); iter != filtersMap.constEnd(); ++iter) {
        QModelIndex indexActiveState = sourceModel()->index(sourceRow, 2, sourceParent);
        QModelIndex indexUnitName = sourceModel()->index(sourceRow, 0, sourceParent);

        if (iter.key() == ActiveState) {
            ret = (indexActiveState.data().toString().contains(QRegularExpression(iter.value())));
        } else if (iter.key() == UnitType) {
            ret = (indexUnitName.data().toString().contains(QRegularExpression(iter.value())));
        } else if (iter.key() == UnitName) {
            ret = (indexUnitName.data().toString().contains(QRegularExpression(iter.value(), QRegularExpression::CaseInsensitiveOption)));
        }

        if (!ret) {
            return ret;
        }
    }

    return ret;
}

#include "moc_sortfilterunitmodel.cpp"
