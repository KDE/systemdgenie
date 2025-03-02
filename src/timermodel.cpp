// SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "timermodel.h"
#include "systemd_timer_interface.h"

#include <KLocalizedString>
#include <QIcon>
#include <qlocale.h>

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

TimerModel::TimerModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        for (auto &timer : m_timers) {
            {
                QDateTime time;
                qlonglong nextElapseMonotonicMsec = timer.interface->nextElapseUSecMonotonic() / 1000;
                qlonglong nextElapseRealtimeMsec = timer.interface->nextElapseUSecRealtime() / 1000;

                if (nextElapseMonotonicMsec == 0) {
                    // Timer is calendar-based
                    time.setMSecsSinceEpoch(nextElapseRealtimeMsec);
                } else {
                    // Timer is monotonic
                    time = QDateTime().currentDateTime();
                    time = time.addMSecs(nextElapseMonotonicMsec);

                    // Get the monotonic system clock
                    struct timespec ts;
                    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
                        qDebug() << "Failed to get the monotonic system clock!";

                    // Convert the monotonic system clock to microseconds
                    qlonglong now_mono_usec = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

                    // And subtract it.
                    time = time.addMSecs(-now_mono_usec / 1000);
                }
                timer.next = time;
            }
            {
                QDateTime time;
                qlonglong nextElapseMonotonicMsec = timer.interface->nextElapseUSecMonotonic() / 1000;
                qlonglong nextElapseRealtimeMsec = timer.interface->nextElapseUSecRealtime() / 1000;

                if (nextElapseMonotonicMsec == 0) {
                    // Timer is calendar-based
                    time.setMSecsSinceEpoch(nextElapseRealtimeMsec);
                } else {
                    // Timer is monotonic
                    time = QDateTime().currentDateTime();
                    time = time.addMSecs(nextElapseMonotonicMsec);

                    // Get the monotonic system clock
                    struct timespec ts;
                    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
                        qDebug() << "Failed to get the monotonic system clock!";

                    // Convert the monotonic system clock to microseconds
                    qlonglong now_mono_usec = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

                    // And subtract it.
                    time = time.addMSecs(-now_mono_usec / 1000);
                }
                timer.next = time;
            }
        }
        Q_EMIT dataChanged(index(0, LeftColumn), index(rowCount() - 1, LastColumn), {});
    });
    timer->setInterval(1min);
    timer->start();
}

TimerModel::~TimerModel() = default;

int TimerModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_timers.size();
}

int TimerModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant TimerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case TimerColumn:
        return i18nc("@title:column", "Timer");
    case NextColumn:
        return i18nc("@title:column", "Next");
    case LeftColumn:
        return i18nc("@title:column", "Left");
    case LastColumn:
        return i18nc("@title:column", "Last");
    case PassedColumn:
        return i18nc("@title:column", "Passed");
    case ActivatesColumn:
        return i18nc("@title:column", "Activates");
    default:
        return {};
    }
}

QVariant TimerModel::data(const QModelIndex &idx, int role) const
{
    Q_ASSERT(checkIndex(idx, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &timer = m_timers.at(idx.row());

    switch (role) {
    case Qt::DisplayRole:
        switch (idx.column()) {
        case TimerColumn:
            return timer.id;
        case NextColumn:
            return QLocale().toString(timer.next, QLocale::ShortFormat);
        case LastColumn:
            if (!timer.last.isValid()) {
                return i18nc("invalid", "n/a");
            }
            return QLocale().toString(timer.last, QLocale::ShortFormat);
        case LeftColumn: {
            const QDateTime current = QDateTime().currentDateTime();
            const qlonglong leftSecs = current.secsTo(timer.next);

            // todo port to kformat
            if (leftSecs >= 31536000)
                return QStringLiteral("%1 years").arg(leftSecs / 31536000);
            else if (leftSecs >= 604800)
                return QStringLiteral("%1 weeks").arg(leftSecs / 604800);
            else if (leftSecs >= 86400)
                return QStringLiteral("%1 days").arg(leftSecs / 86400);
            else if (leftSecs >= 3600)
                return QStringLiteral("%1 hr").arg(leftSecs / 3600);
            else if (leftSecs >= 60)
                return QStringLiteral("%1 min").arg(leftSecs / 60);
            else if (leftSecs < 0)
                return QStringLiteral("0 s");
            else
                return QStringLiteral("%1 s").arg(leftSecs);
        }
        case PassedColumn: {
            if (!timer.last.isValid()) {
                return i18nc("invalid", "n/a");
            }
            const QDateTime current = QDateTime().currentDateTime();
            const qlonglong passedSecs = timer.last.secsTo(current);
            if (passedSecs >= 31536000)
                return QStringLiteral("%1 years").arg(passedSecs / 31536000);
            else if (passedSecs >= 604800)
                return QStringLiteral("%1 weeks").arg(passedSecs / 604800);
            else if (passedSecs >= 86400)
                return QStringLiteral("%1 days").arg(passedSecs / 86400);
            else if (passedSecs >= 3600)
                return QStringLiteral("%1 hr").arg(passedSecs / 3600);
            else if (passedSecs >= 60)
                return QStringLiteral("%1 min").arg(passedSecs / 60);
            else if (passedSecs < 0)
                return QStringLiteral("0 s");
            else
                return QStringLiteral("%1 s").arg(passedSecs);
        }
        case ActivatesColumn:
            return timer.unitToActivate;
        default:
            return {};
        }
    case Qt::DecorationRole:
        if (idx.column() == TimerColumn) {
            return timer.system ? QIcon::fromTheme(u"applications-system-symbolic"_s) : QIcon::fromTheme(u"user-identity-symbolic"_s);
        }
        return {};

    case IconNameRole:
        if (idx.column() == TimerColumn) {
            return timer.system ? u"applications-system-symbolic"_s : u"user-identity-symbolic"_s;
        }
        return {};
    default:
        return {};
    }
}

QHash<int, QByteArray> TimerModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "displayName"},
        {IconNameRole, "iconName"},
    };
}

UnitModel *TimerModel::systemModel() const
{
    return m_systemModel;
}

void TimerModel::setSystemModel(UnitModel *model)
{
    if (m_systemModel == model) {
        return;
    }
    m_systemModel = model;
    Q_EMIT systemModelChanged();

    connect(m_systemModel, &UnitModel::unitsRefreshed, this, &TimerModel::slotRefreshTimerList);
}

UnitModel *TimerModel::userModel() const
{
    return m_userModel;
}

void TimerModel::setUserModel(UnitModel *model)
{
    if (m_userModel == model) {
        return;
    }
    m_userModel = model;
    Q_EMIT userModelChanged();

    connect(m_userModel, &UnitModel::unitsRefreshed, this, &TimerModel::slotRefreshTimerList);
}

static TimerModel::Timer buildTimerListRow(const SystemdUnit &unit, const QVector<SystemdUnit> &list, bool system, const QDBusConnection &connection)
{
    TimerModel::Timer timer;
    timer.id = unit.id;
    timer.interface = std::make_unique<OrgFreedesktopSystemd1TimerInterface>(u"org.freedesktop.systemd1"_s, unit.unit_path.path(), connection, nullptr);
    timer.system = system;
    timer.unitToActivate = timer.interface->unit();

    // use unit object to get last time for activated service
    int index = list.indexOf(SystemdUnit(timer.id));
    if (index != -1) {
        const qlonglong lastTriggerMSec = timer.interface->lastTriggerUSec() / 1000;
        const qlonglong inactivateExitTimestampMsec = timer.interface->inactiveExitTimestamp() / 1000;

        if (inactivateExitTimestampMsec == 0) {
            // The unit has not run in this boot
            // Use LastTrigger to see if the timer is persistent
            if (lastTriggerMSec == 0)
                timer.last = {};
            else {
                timer.last.setMSecsSinceEpoch(lastTriggerMSec);
            }
        } else {
            timer.last.setMSecsSinceEpoch(inactivateExitTimestampMsec);
        }
    }

    return timer;
}

void TimerModel::slotRefreshTimerList()
{
    beginResetModel();
    m_timers.clear();
    // Iterate through system unitlist and add timers to the model
    for (const SystemdUnit &unit : m_systemModel->unitsConst()) {
        if (unit.id.endsWith(QLatin1String(".timer")) && unit.load_state != QLatin1String("unloaded")) {
            auto timer = buildTimerListRow(unit, m_systemModel->unitsConst(), true, m_systemModel->connection());
            m_timers.push_back(std::move(timer));
        }
    }

    // Iterate through user unitlist and add timers to the model
    for (const SystemdUnit &unit : m_userModel->unitsConst()) {
        if (unit.id.endsWith(QLatin1String(".timer")) && unit.load_state != QLatin1String("unloaded")) {
            m_timers.push_back(buildTimerListRow(unit, m_userModel->unitsConst(), false, m_userModel->connection()));
        }
    }
    endResetModel();
}
