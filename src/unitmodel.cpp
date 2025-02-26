/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "unitmodel.h"

#include <KColorScheme>
#include <KLocalizedString>

#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <QColor>
#include <QIcon>
#include <QtDBus/QtDBus>

#include <systemd/sd-journal.h>

#include "job/unitsfetchjob.h"
#include "systemd_manager_interface.h"

using namespace Qt::StringLiterals;

UnitModel::UnitModel(QObject *parent)
    : QAbstractTableModel(parent)
{
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
    beginResetModel();
    m_units = units;
    for (const SystemdUnit &unit : units) {
        if (unit.active_state == "active"_L1) {
            m_nonActiveUnitsCount++;
        }
    }
    endResetModel();
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
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0) {
        return i18n("Unit");
    } else if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 1) {
        return i18n("Load State");
    } else if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 2) {
        return i18n("Active State");
    } else if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 3) {
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
        QStringList log = getLastJrnlEntries(selUnit);
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

QStringList UnitModel::getLastJrnlEntries(QString unit) const
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
    // qDebug() << "unit:" << unit;

    QVariantList args;
    if (method == QLatin1String("EnableUnitFiles") || method == QLatin1String("MaskUnitFiles")) {
        args << QVariant(QStringList{unit}) << false << true;
    } else if (method == QLatin1String("DisableUnitFiles") || method == QLatin1String("UnmaskUnitFiles")) {
        args << QVariant(QStringList{unit}) << false;
    } else {
        args = QVariantList{unit, QStringLiteral("replace")};
    }

    if (m_type == UserUnits) {
        m_managerIface->asyncCallWithArgumentList(method, args);
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
        m_managerIface =
            new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd1"_s, u"/org/freedesktop/systemd1"_s, QDBusConnection::systemBus(), this);
        m_userBus.clear();
    } else {
        if (QFileInfo::exists(QStringLiteral("/run/user/%1/bus").arg(QString::number(getuid())))) {
            m_userBus = QStringLiteral("unix:path=/run/user/%1/bus").arg(QString::number(getuid()));
        } else if (QFileInfo::exists(QStringLiteral("/run/user/%1/dbus/user_bus_socket").arg(QString::number(getuid())))) {
            m_userBus = QStringLiteral("unix:path=/run/user/%1/dbus/user_bus_socket").arg(QString::number(getuid()));
        }

        if (!m_userBus.isEmpty()) {
            auto connection = QDBusConnection::connectToBus(m_userBus, u"org.freedesktop.systemd1"_s);
            m_managerIface = new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd1"_s, u"/org/freedesktop/systemd1"_s, connection, this);
        }
    }

    m_managerIface->Subscribe();
    connect(m_managerIface, &OrgFreedesktopSystemd1ManagerInterface::Reloading, this, &UnitModel::slotReloading);
    connect(m_managerIface, &OrgFreedesktopSystemd1ManagerInterface::UnitFilesChanged, this, &UnitModel::slotUnitFilesChanged);
    connect(m_managerIface, &OrgFreedesktopSystemd1ManagerInterface::JobNew, this, &UnitModel::slotJobNew);
    connect(m_managerIface, &OrgFreedesktopSystemd1ManagerInterface::JobRemoved, this, &UnitModel::slotJobRemoved);
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
        qWarning() << "refreshed" << m_type;
        setUnits(units);
        Q_EMIT unitsRefreshed();
        m_refreshing = false;
    });
    job->start();
}

void UnitModel::slotJobNew(uint id, const QDBusObjectPath &path, const QString &unit)
{
    qDebug() << "UserJobNew: " << id << path.path() << unit;
    slotRefreshUnitsList();
}

void UnitModel::slotJobRemoved(uint id, const QDBusObjectPath &path, const QString &unit, const QString &result)
{
    qDebug() << "UserJobRemoved: " << id << path.path() << unit << result;
    slotRefreshUnitsList();
}

void UnitModel::slotReloading(bool status)
{
    if (!status) {
        Q_EMIT errorOccured(
            i18nc("%1 is a time", "%1: User daemon reloaded...", QLocale().toString(QDateTime::currentDateTime().time(), QLocale::ShortFormat)));
        slotRefreshUnitsList();
    }
}

void UnitModel::slotPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps)
{
    Q_UNUSED(changedProps)
    Q_UNUSED(invalidatedProps)

    qDebug() << "userPropertiesChanged:" << iface; // << changedProps << invalidatedProps;
    slotRefreshUnitsList();
}

void UnitModel::slotUnitFilesChanged()
{
    // qDebug() << "User unitFilesChanged";
    slotRefreshUnitsList();
}

int UnitModel::nonActiveUnits() const
{
    return m_nonActiveUnitsCount;
}
