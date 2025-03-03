// SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "unitmodel.h"

#include <KColorScheme>
#include <KLocalizedString>

#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <QColor>
#include <QIcon>
#include <QTimer>

#include <qtmetamacros.h>
#include <systemd/sd-journal.h>

#include "job/unitsfetchjob.h"
#include "systemd_manager_interface.h"
#include "systemd_unit_interface.h"

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

UnitModel::UnitModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_connection(QDBusConnection::systemBus())
    , m_refreshTimer(new QTimer(this))
{
    m_refreshTimer->setInterval(1s);
    connect(m_refreshTimer, &QTimer::timeout, this, [this]() {
        slotRefreshUnitsList();
    });
}

const QList<SystemdUnit> &UnitModel::unitsConst() const
{
    return m_units;
}

QList<SystemdUnit> UnitModel::units() const
{
    return m_units;
}

void UnitModel::setUnits(const QList<SystemdUnit> &units)
{
    if (m_units.isEmpty()) {
        beginResetModel();
        m_units = units;
        endResetModel();
    } else {
        for (const auto &unit : units) {
            auto it = std::ranges::find_if(m_units, [unit](const auto &existingUnit) {
                return existingUnit.id == unit.id;
            });
            if (it == m_units.cend()) {
                beginInsertRows({}, m_units.count(), m_units.count());
                m_units << unit;
                endInsertRows();
                continue;
            }
            if (it->active_state != unit.active_state || it->load_state != unit.load_state || it->unit_file_status != unit.unit_file_status) {
                it->load_state = unit.load_state;
                it->active_state = unit.active_state;
                it->unit_file_status = unit.unit_file_status;
                const size_t row = std::distance(std::begin(m_units), it);
                Q_EMIT dataChanged(index(row, 0), index(row, columnCount() - 1), {});
            }
        }
    }

    m_activeUnitsCount = 0;
    for (const SystemdUnit &unit : units) {
        if (unit.active_state == "active"_L1) {
            m_activeUnitsCount++;
        }
    }
    Q_EMIT unitsRefreshed();
}

int UnitModel::rowCount(const QModelIndex &) const
{
    return m_units.size();
}

int UnitModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant UnitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == UnitColumn) {
        return i18n("Unit");
    } else if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == LoadStateColumn) {
        return i18n("Load State");
    } else if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == ActiveStateColumn) {
        return i18n("Active State");
    } else if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == UnitStateColumn) {
        return i18n("Unit State");
    }
    return QVariant();
}

QHash<int, QByteArray> UnitModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "displayName"},
        {IconNameRole, "iconName"},
        {ColorRole, "textColor"},
        {UnitPathRole, "unitPath"},
        {Qt::ToolTipRole, "tooltip"},
    };
}

QVariant UnitModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &unit = m_units.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0:
            return unit.id;
        case 1:
            return unit.load_state;
        case 2:
            return unit.active_state;
        case 3:
            return unit.sub_state;
        default:
            return {};
        }
    case UnitPathRole: {
        // Check if unit has a writable unit file, if not disable editing.
        QString frpath = unit.unit_file;

        QFileInfo fileInfo(frpath);
        QStorageInfo storageInfo(frpath);
        bool isUnitWritable = fileInfo.permission(QFile::WriteOwner) && !storageInfo.isReadOnly();
        return isUnitWritable ? frpath : QString();
    }
    case Qt::ForegroundRole: {
        const KColorScheme scheme(QPalette::Normal);
        if (unit.active_state == QLatin1String("active"))
            return scheme.foreground(KColorScheme::PositiveText);
        else if (unit.active_state == QLatin1String("failed"))
            return scheme.foreground(KColorScheme::NegativeText);
        else if (unit.active_state == QLatin1String("-"))
            return scheme.foreground(KColorScheme::InactiveText);
        return QVariant();
    }
    case ColorRole: {
        const KColorScheme scheme(QPalette::Normal);
        if (unit.active_state == QLatin1String("active"))
            return scheme.foreground(KColorScheme::PositiveText).color();
        else if (unit.active_state == QLatin1String("failed"))
            return scheme.foreground(KColorScheme::NegativeText).color();
        else if (unit.active_state == QLatin1String("-"))
            return scheme.foreground(KColorScheme::InactiveText).color();
        return scheme.foreground(KColorScheme::NormalText).color();
    }
    case Qt::DecorationRole:
        if (index.column() == 0) {
            if (unit.active_state == QLatin1String("active")) {
                return QIcon::fromTheme(QStringLiteral("emblem-success"));
            } else if (unit.active_state == QLatin1String("inactive")) {
                return QIcon::fromTheme(QStringLiteral("emblem-pause"));
            } else if (unit.active_state == QLatin1String("failed")) {
                return QIcon::fromTheme(QStringLiteral("emblem-error"));
            } else if (unit.active_state == QLatin1String("-")) {
                return QIcon::fromTheme(QStringLiteral("emblem-unavailable"));
            }
            return QVariant();
        }
    case IconNameRole:
        if (index.column() == 0) {
            if (unit.active_state == QLatin1String("active")) {
                return QStringLiteral("emblem-success");
            } else if (unit.active_state == QLatin1String("inactive")) {
                return QStringLiteral("emblem-pause");
            } else if (unit.active_state == QLatin1String("failed")) {
                return QStringLiteral("emblem-error");
            } else if (unit.active_state == QLatin1String("-")) {
                return QStringLiteral("emblem-unavailable");
            }
            return QString{};
        } else {
            return QString{};
        }
    case Qt::ToolTipRole: {
        const QString selUnit = unit.id;
        const QString selUnitPath = unit.unit_path.path();
        const QString selUnitFile = unit.unit_file;

        QString toolTipText;
        toolTipText.append(QStringLiteral("<FONT>"));
        toolTipText.append(QStringLiteral("<b>%1</b><hr>").arg(selUnit));

        // Create a DBus interface
        QDBusConnection bus(QDBusConnection::systemBus());
        if (!m_userBus.isEmpty()) {
            bus = QDBusConnection::connectToBus(m_userBus, QStringLiteral("org.freedesktop.systemd1"));
        }
        QDBusInterface *iface;

        // Use the DBus interface to get unit properties
        if (!selUnitPath.isEmpty()) {
            // Unit has a valid path

            iface = new QDBusInterface(QStringLiteral("org.freedesktop.systemd1"), selUnitPath, QStringLiteral("org.freedesktop.systemd1.Unit"), bus);
            if (iface->isValid()) {
                // Unit has a valid unit DBus object
                toolTipText.append(i18n("<b>Description: </b>"));
                toolTipText.append(iface->property("Description").toString());

                const QString unitFilePath = iface->property("FragmentPath").toString();
                if (!unitFilePath.isEmpty()) {
                    toolTipText.append(i18n("<br><b>Unit file: </b>%1", unitFilePath));
                    toolTipText.append(i18n("<br><b>Unit file state: </b>"));
                    toolTipText.append(iface->property("UnitFileState").toString());
                }

                const QString sourcePath = iface->property("SourcePath").toString();
                if (!sourcePath.isEmpty()) {
                    toolTipText.append(i18n("<br><b>Source path: </b>%1", sourcePath));
                }

                qulonglong ActiveEnterTimestamp = iface->property("ActiveEnterTimestamp").toULongLong();
                toolTipText.append(i18n("<br><b>Activated: </b>"));
                if (ActiveEnterTimestamp == 0)
                    toolTipText.append(QStringLiteral("n/a"));
                else {
                    QDateTime timeActivated;
                    timeActivated.setMSecsSinceEpoch(ActiveEnterTimestamp / 1000);
                    toolTipText.append(timeActivated.toString());
                }

                qulonglong InactiveEnterTimestamp = iface->property("InactiveEnterTimestamp").toULongLong();
                toolTipText.append(i18n("<br><b>Deactivated: </b>"));
                if (InactiveEnterTimestamp == 0)
                    toolTipText.append(QStringLiteral("n/a"));
                else {
                    QDateTime timeDeactivated;
                    timeDeactivated.setMSecsSinceEpoch(InactiveEnterTimestamp / 1000);
                    toolTipText.append(timeDeactivated.toString());
                }
            }
            delete iface;

        } else {
            // Unit does not have a valid unit DBus object (typically unloaded units).
            // Retrieve UnitFileState from Manager object.

            iface = new QDBusInterface(QStringLiteral("org.freedesktop.systemd1"),
                                       QStringLiteral("/org/freedesktop/systemd1"),
                                       QStringLiteral("org.freedesktop.systemd1.Manager"),
                                       bus);
            QList<QVariant> args;
            args << selUnit;

            toolTipText.append(i18n("<b>Unit file: </b>"));
            if (!selUnitFile.isEmpty()) {
                toolTipText.append(selUnitFile);
            }

            toolTipText.append(i18n("<br><b>Unit file state: </b>"));
            if (!selUnitFile.isEmpty()) {
                toolTipText.append(iface->callWithArgumentList(QDBus::AutoDetect, QStringLiteral("GetUnitFileState"), args).arguments().at(0).toString());
            }

            delete iface;
        }

        // Journal entries for units
        toolTipText.append(i18n("<hr><b>Last log entries:</b>"));
        QStringList log = lastJrnlEntries(selUnit);
        if (log.isEmpty()) {
            toolTipText.append(i18n("<br><i>No log entries found for this unit.</i>"));
        } else {
            for (int i = log.count() - 1; i >= 0; --i) {
                if (!log.at(i).isEmpty())
                    toolTipText.append(QStringLiteral("<br>%1").arg(log.at(i)));
            }
        }

        toolTipText.append(QStringLiteral("</FONT"));

        return toolTipText;
    }
    default:
        return QVariant();
    }
}

QStringList UnitModel::lastJrnlEntries(const QString &unit) const
{
    QString match1, match2;
    int r, jflags;
    QStringList reply;
    const void *data;
    size_t length;
    uint64_t time;
    sd_journal *journal;

    if (!m_userBus.isEmpty()) {
        match1 = QStringLiteral("USER_UNIT=%1").arg(unit);
        jflags = (SD_JOURNAL_LOCAL_ONLY | SD_JOURNAL_CURRENT_USER);
    } else {
        match1 = QStringLiteral("_SYSTEMD_UNIT=%1").arg(unit);
        match2 = QStringLiteral("UNIT=%1").arg(unit);
        jflags = (SD_JOURNAL_LOCAL_ONLY | SD_JOURNAL_SYSTEM);
    }

    r = sd_journal_open(&journal, jflags);
    if (r != 0) {
        qDebug() << "Failed to open journal";
        return reply;
    }

    sd_journal_flush_matches(journal);

    r = sd_journal_add_match(journal, match1.toUtf8().constData(), 0);
    if (r != 0)
        return reply;

    if (!match2.isEmpty()) {
        sd_journal_add_disjunction(journal);
        r = sd_journal_add_match(journal, match2.toUtf8().constData(), 0);
        if (r != 0)
            return reply;
    }

    r = sd_journal_seek_tail(journal);
    if (r != 0)
        return reply;

    // Fetch the last 5 entries
    for (int i = 0; i < 5; ++i) {
        r = sd_journal_previous(journal);
        if (r == 1) {
            QString line;

            // Get the date and time
            r = sd_journal_get_realtime_usec(journal, &time);
            if (r == 0) {
                QDateTime date;
                date.setMSecsSinceEpoch(time / 1000);
                line.append(date.toString(QStringLiteral("yyyy.MM.dd hh:mm")));
            }

            // Color messages according to priority
            r = sd_journal_get_data(journal, "PRIORITY", &data, &length);
            if (r == 0) {
                int prio = QString::fromUtf8((const char *)data, length).section(QLatin1Char('='), 1).toInt();
                if (prio <= 3)
                    line.append(QStringLiteral("<span style='color:tomato;'>"));
                else if (prio == 4)
                    line.append(QStringLiteral("<span style='color:khaki;'>"));
                else
                    line.append(QStringLiteral("<span style='color:palegreen;'>"));
            }

            // Get the message itself
            r = sd_journal_get_data(journal, "MESSAGE", &data, &length);
            if (r == 0) {
                line.append(QStringLiteral(": %1</span>").arg(QString::fromUtf8((const char *)data, length).section(QLatin1Char('='), 1)));
                if (line.length() > 195)
                    line = QStringLiteral("%1...</span>").arg(line.left(195));
                reply << line;
            }
        } else // previous failed, no more entries
            return reply;
    }

    sd_journal_close(journal);

    return reply;
}

void UnitModel::executeUnitAction(int row, const QString &method)
{
    const auto idx = index(row, 0);
    if (!idx.isValid()) {
        qWarning() << "invalid index" << row;
        return;
    }
    const QString unit = idx.data().toString();

    QVariantList args;
    if (method == QLatin1String("EnableUnitFiles") || method == QLatin1String("MaskUnitFiles")) {
        args << QVariant(QStringList{unit}) << false << true;
    } else if (method == QLatin1String("DisableUnitFiles") || method == QLatin1String("UnmaskUnitFiles")) {
        args << QVariant(QStringList{unit}) << false;
    } else {
        args = QVariantList{unit, QStringLiteral("replace")};
    }

    if (m_type == UserUnits) {
        auto pendingReply = m_managerIface->asyncCallWithArgumentList(method, args);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingReply, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher, pendingReply](QDBusPendingCallWatcher *) {
            if (pendingReply.isError()) {
                qWarning() << pendingReply.error().message();
                Q_EMIT errorOccured(pendingReply.error().message());
            }
            qWarning() << pendingReply.reply();
            watcher->deleteLater();
        });
        return;
    }

    QVariantMap helperArgs;
    helperArgs[QStringLiteral("service")] = m_managerIface->service();
    helperArgs[QStringLiteral("path")] = m_managerIface->path();
    helperArgs[QStringLiteral("interface")] = m_managerIface->interface();
    helperArgs[QStringLiteral("method")] = method;
    helperArgs[QStringLiteral("argsForCall")] = args;

    // Call the helper. This call causes the debug output: "QDBusArgument: read from a write-only object"
    KAuth::Action serviceAction(QStringLiteral("org.kde.kcontrol.systemdgenie.dbusaction"));
    serviceAction.setHelperId(QStringLiteral("org.kde.kcontrol.systemdgenie"));
    serviceAction.setArguments(helperArgs);

    KAuth::ExecuteJob *job = serviceAction.execute();
    job->exec();

    if (!job->exec())
        Q_EMIT errorOccured(i18n("Unable to authenticate/execute the action: %1", job->errorString()));
    else {
        qDebug() << "DBus action successful.";
    }
}

UnitModel::Type UnitModel::type() const
{
    return m_type;
}

void UnitModel::setType(Type type)
{
    if (m_type == type) {
        return;
    }
    m_type = type;
    Q_EMIT typeChanged();

    if (m_type == UnitModel::SystemUnits) {
        m_connection = QDBusConnection::systemBus();
        m_managerIface = new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd1"_s, u"/org/freedesktop/systemd1"_s, m_connection, this);
        m_userBus.clear();
    } else {
        if (QFileInfo::exists(QStringLiteral("/run/user/%1/bus").arg(QString::number(getuid())))) {
            m_userBus = QStringLiteral("unix:path=/run/user/%1/bus").arg(QString::number(getuid()));
        } else if (QFileInfo::exists(QStringLiteral("/run/user/%1/dbus/user_bus_socket").arg(QString::number(getuid())))) {
            m_userBus = QStringLiteral("unix:path=/run/user/%1/dbus/user_bus_socket").arg(QString::number(getuid()));
        }

        if (!m_userBus.isEmpty()) {
            m_connection = QDBusConnection::connectToBus(m_userBus, u"org.freedesktop.systemd1"_s);
            m_managerIface = new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd1"_s, u"/org/freedesktop/systemd1"_s, m_connection, this);
        }
    }

    m_managerIface->Subscribe();
    connect(m_managerIface, &OrgFreedesktopSystemd1ManagerInterface::Reloading, this, &UnitModel::slotReloading);
    connect(m_managerIface, &OrgFreedesktopSystemd1ManagerInterface::UnitFilesChanged, this, &UnitModel::slotUnitFilesChanged);
    connect(m_managerIface, &OrgFreedesktopSystemd1ManagerInterface::UnitNew, this, &UnitModel::slotUnitNew);
    connect(m_managerIface, &OrgFreedesktopSystemd1ManagerInterface::UnitRemoved, this, &UnitModel::slotUnitRemoved);
    auto connected = m_managerIface->connection().connect(u"org.freedesktop.systemd1"_s,
                                                          {},
                                                          u"org.freedesktop.DBus.Properties"_s,
                                                          u"PropertiesChanged"_s,
                                                          this,
                                                          SLOT(slotPropertiesChanged(QString, QVariantMap, QStringList)));
    Q_ASSERT(connected);

    slotRefreshUnitsList();
}

void UnitModel::slotRefreshUnitsList()
{
    if (!m_managerIface || m_refreshing) {
        return;
    }

    m_refreshing = true;
    auto job = new UnitsFetchJob(m_managerIface);
    connect(job, &UnitsFetchJob::finished, this, [this, job](KJob *) {
        const auto units = job->units();
        setUnits(units);
        Q_EMIT unitsRefreshed();
        m_refreshing = false;
    });
    job->start();
}

void UnitModel::slotReloading(bool status)
{
    if (!status && !m_refreshTimer->isActive()) {
        m_refreshTimer->start();
    }
}

void UnitModel::slotPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps)
{
    Q_UNUSED(changedProps)
    Q_UNUSED(invalidatedProps)
    Q_UNUSED(iface)

    const QString pathFull = message().path();

    if (m_selectedUnit && m_selectedUnit->path() == pathFull) {
        m_selectedUnit->handlePropertiesChanged(iface, changedProps, invalidatedProps);
    }

    auto it = std::ranges::find_if(m_units, [pathFull](const auto &unitObject) {
        return unitObject.unit_path.path() == pathFull;
    });

    if (it == m_units.cend()) {
        return;
    }

    bool changed = false;
    if (changedProps.contains("ActiveState"_L1)) {
        it->active_state = changedProps["ActiveState"_L1].toString();
        changed = true;
    }

    if (changedProps.contains("LoadState"_L1)) {
        it->load_state = changedProps["LoadState"_L1].toString();
        changed = true;
    }

    if (changed) {
        const size_t row = std::distance(std::begin(m_units), it);
        Q_EMIT dataChanged(index(row, 0), index(row, columnCount() - 1), {});
    }
}

void UnitModel::slotRefreshUnit(const QString &unit)
{
    const auto getUnit = [this](const QString &unit) {
        return std::ranges::find_if(m_units, [unit](const auto &unitObject) {
            return unitObject.id == unit;
        });
    };

    const auto unitIt = getUnit(unit);
    if (unitIt == std::cend(m_units)) {
        qWarning() << "unit refreshed not found";
        return;
    }

    auto reply = m_managerIface->GetUnit(unit);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [reply, watcher, unit, getUnit, this](QDBusPendingCallWatcher *) {
        watcher->deleteLater();
        if (reply.isError()) {
            qWarning() << reply.error().message();
            return;
        }

        const auto unitIt = getUnit(unit);
        if (unitIt == std::cend(m_units)) {
            qWarning() << "unit refreshed not found";
            return;
        }

        OrgFreedesktopSystemd1UnitInterface interface(u"org.freedesktop.systemd1"_s, reply.value().path(), m_connection, nullptr);

        unitIt->load_state = interface.loadState();
        unitIt->active_state = interface.activeState();
        unitIt->unit_file_status = interface.unitFileState();
        const size_t row = std::distance(std::begin(m_units), unitIt);
        Q_EMIT dataChanged(index(row, 0), index(row, columnCount() - 1), {});
    });
}

void UnitModel::slotUnitFilesChanged()
{
    if (!m_refreshTimer->isActive()) {
        m_refreshTimer->start();
    }
}

int UnitModel::activeUnitsCount() const
{
    return m_activeUnitsCount;
}

OrgFreedesktopSystemd1UnitInterface *UnitModel::unitObject(int row)
{
    auto &unit = m_units[row];
    const auto &path = unit.unit_path;

    if (path.path().isEmpty()) {
        return nullptr;
    }

    auto interface = new OrgFreedesktopSystemd1UnitInterface(u"org.freedesktop.systemd1"_s, path.path(), m_connection, nullptr);

    unit.load_state = interface->loadState();
    unit.active_state = interface->activeState();
    unit.unit_file_status = interface->unitFileState();
    Q_EMIT dataChanged(index(row, 0), index(row, columnCount() - 1), {});

    m_selectedUnit = interface;

    return interface;
}

QString UnitModel::unitFile(int row) const
{
    if (row < 0) {
        return {};
    }
    auto &unit = m_units[row];
    return unit.unit_file;
}

QString UnitModel::unitFileStatus(int row) const
{
    if (row < 0) {
        return {};
    }
    auto &unit = m_units[row];
    return unit.unit_file_status;
}

void UnitModel::slotUnitNew(const QString &id, const QDBusObjectPath &unit)
{
    Q_UNUSED(id);
    Q_UNUSED(unit);
    slotRefreshUnitsList();
}

void UnitModel::slotUnitRemoved(const QString &id, const QDBusObjectPath &unit)
{
    Q_UNUSED(unit);

    const auto unitIt = std::ranges::find_if(m_units, [id](const auto &unitObject) {
        return unitObject.id == id;
    });

    if (unitIt == std::cend(m_units)) {
        qWarning() << "unit to delete not found";
        return;
    }

    const size_t row = std::distance(std::begin(m_units), unitIt);
    beginRemoveRows({}, row, row);
    m_units.remove(row);
    endRemoveRows();
}

QDBusConnection UnitModel::connection() const
{
    return m_connection;
}
