// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractTableModel>
#include <qqmlregistration.h>

#include "systemd_timer_interface.h"
#include "unitmodel.h"

class TimerModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(UnitModel *systemModel READ systemModel WRITE setSystemModel NOTIFY systemModelChanged)
    Q_PROPERTY(UnitModel *userModel READ userModel WRITE setUserModel NOTIFY userModelChanged)

public:
    enum ExtraRoles {
        ColorRole = Qt::UserRole + 1,
        IconNameRole,
        UnitPathRole,
    };
    Q_ENUM(ExtraRoles);

    enum Columns {
        TimerColumn,
        NextColumn,
        LeftColumn,
        LastColumn,
        PassedColumn,
        ActivatesColumn,
        ColumnCount,
    };

    explicit TimerModel(QObject *parent = nullptr);
    ~TimerModel() override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    UnitModel *systemModel() const;
    void setSystemModel(UnitModel *model);

    UnitModel *userModel() const;
    void setUserModel(UnitModel *model);

    struct Timer {
        QString id;
        QString unitToActivate;
        std::unique_ptr<OrgFreedesktopSystemd1TimerInterface> interface;
        bool system;
        QDateTime last;
        QDateTime next;

        Timer() noexcept
            : interface(nullptr)
        {
        }

        Timer(Timer &&other) noexcept
            : id(std::move(other.id))
            , unitToActivate(std::move(other.unitToActivate))
            , interface(std::move(other.interface))
            , system(other.system)
            , last(std::move(other.last))
            , next(std::move(other.next))
        {
        }

        Timer &operator=(Timer &&other) noexcept
        {
            if (this != &other) {
                id = std::move(other.id);
                unitToActivate = std::move(other.unitToActivate);
                interface = std::move(other.interface);
                system = other.system;
                last = std::move(other.last);
                next = std::move(other.next);
            }
            return *this;
        }
    };

Q_SIGNALS:
    void systemModelChanged();
    void userModelChanged();

private:
    void slotRefreshTimerList();

    UnitModel *m_systemModel = nullptr;
    UnitModel *m_userModel = nullptr;
    std::vector<Timer> m_timers;
};
