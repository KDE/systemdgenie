/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SORTFILTERUNITMODEL_H
#define SORTFILTERUNITMODEL_H

#include <QSortFilterProxyModel>
#include <qqmlregistration.h>

class SortFilterUnitModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum FilterType {
        ActiveState,
        UnitType,
        UnitName
    };
    Q_ENUM(FilterType)

    explicit SortFilterUnitModel(QObject *parent = nullptr);
    Q_SLOT void addFilterRegExp(FilterType type, const QString &pattern);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QHash<FilterType, QString> filtersMap;
};

#endif // SORTFILTERUNITMODEL_H
