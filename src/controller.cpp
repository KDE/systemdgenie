// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "controller.h"

#include <QDBusConnection>

#include <KColorScheme>
#include <KLocalizedString>

#include "login_manager_interface.h"
#include "login_session_interface.h"
#include "sessionsfetchjob.h"
#include "systemd_manager_interface.h"
#include "unitsfetchjob.h"

using namespace Qt::StringLiterals;

constexpr static const QLatin1StringView connSystemd = "org.freedesktop.systemd1"_L1;

Controller::Controller(QObject *parent)
    : QObject(parent)
    , m_sessionModel(new QStandardItemModel(this))
    , m_systemManagerInterface(
          new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd1"_s, u"/org/freedesktop/systemd1"_s, QDBusConnection::systemBus(), this))
    , m_loginManagerInterface(
          new OrgFreedesktopLogin1ManagerInterface(u"org.freedesktop.login1"_s, u"/org/freedesktop/login1"_s, QDBusConnection::systemBus(), this))
{
    // Subscribe to dbus signals from systemd system daemon and connect them to slots.
    m_systemManagerInterface->Subscribe();
    connect(m_systemManagerInterface, &OrgFreedesktopSystemd1ManagerInterface::Reloading, this, &Controller::slotSystemSystemdReloading);
    connect(m_systemManagerInterface, &OrgFreedesktopSystemd1ManagerInterface::UnitFilesChanged, this, &Controller::slotSystemUnitFilesChanged);
    connect(m_systemManagerInterface, &OrgFreedesktopSystemd1ManagerInterface::JobNew, this, &Controller::slotSystemJobNew);
    connect(m_systemManagerInterface, &OrgFreedesktopSystemd1ManagerInterface::JobRemoved, this, &Controller::slotSystemJobRemoved);
    bool connected = m_systemManagerInterface->connection().connect(u"org.freedesktop.systemd1"_s,
                                                                    {},
                                                                    u"org.freedesktop.DBus.Properties"_s,
                                                                    u"PropertiesChanged"_s,
                                                                    this,
                                                                    SLOT(slotSystemPropertiesChanged(QString, QVariantMap, QStringList)));

    Q_ASSERT(connected);

    if (QFileInfo::exists(QStringLiteral("/run/user/%1/bus").arg(QString::number(getuid())))) {
        m_userBusPath = QStringLiteral("unix:path=/run/user/%1/bus").arg(QString::number(getuid()));
    } else if (QFileInfo::exists(QStringLiteral("/run/user/%1/dbus/user_bus_socket").arg(QString::number(getuid())))) {
        m_userBusPath = QStringLiteral("unix:path=/run/user/%1/dbus/user_bus_socket").arg(QString::number(getuid()));
    }
    if (!m_userBusPath.isEmpty()) {
        auto connection = QDBusConnection::connectToBus(m_userBusPath, connSystemd);
        m_sessionManagerInterface =
            new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd.Manager"_s, u"/org/freedesktop/systemd1"_s, connection, this);

        // Subscribe to dbus signals from systemd user daemon and connect them to slots
        m_sessionManagerInterface->Subscribe();
        connect(m_sessionManagerInterface, &OrgFreedesktopSystemd1ManagerInterface::Reloading, this, &Controller::slotUserSystemdReloading);
        connect(m_sessionManagerInterface, &OrgFreedesktopSystemd1ManagerInterface::UnitFilesChanged, this, &Controller::slotUserUnitFilesChanged);
        connect(m_sessionManagerInterface, &OrgFreedesktopSystemd1ManagerInterface::JobNew, this, &Controller::slotUserJobNew);
        connect(m_sessionManagerInterface, &OrgFreedesktopSystemd1ManagerInterface::JobRemoved, this, &Controller::slotUserJobRemoved);
        connected = m_sessionManagerInterface->connection().connect(u"org.freedesktop.systemd1"_s,
                                                                    {},
                                                                    u"org.freedesktop.DBus.Properties"_s,
                                                                    u"PropertiesChanged"_s,
                                                                    this,
                                                                    SLOT(slotUserPropertiesChanged(QString, QVariantMap, QStringList)));
        Q_ASSERT(connected);
    }

    m_systemUnitModel = new UnitModel(this, &m_systemUnitsList);
    m_userUnitModel = new UnitModel(this, &m_userUnitsList, m_userBusPath);

    // Set header row
    m_sessionModel->setHorizontalHeaderItem(0, new QStandardItem(i18n("Session ID")));
    m_sessionModel->setHorizontalHeaderItem(1, new QStandardItem(i18n("Session Object Path"))); // This column is hidden
    m_sessionModel->setHorizontalHeaderItem(2, new QStandardItem(i18n("State")));
    m_sessionModel->setHorizontalHeaderItem(3, new QStandardItem(i18n("User ID")));
    m_sessionModel->setHorizontalHeaderItem(4, new QStandardItem(i18n("User Name")));
    m_sessionModel->setHorizontalHeaderItem(5, new QStandardItem(i18n("Seat ID")));

    connected = m_loginManagerInterface->connection().connect(u"org.freedesktop.login1"_s,
                                                              {},
                                                              u"org.freedesktop.DBus.Properties"_s,
                                                              u"PropertiesChanged"_s,
                                                              this,
                                                              SLOT(slotLoginPropertiesChanged(QString, QVariantMap, QStringList)));
    Q_ASSERT(connected);
}

UnitModel *Controller::systemUnitModel() const
{
    return m_systemUnitModel;
}

UnitModel *Controller::userUnitModel() const
{
    return m_userUnitModel;
}

QStandardItemModel *Controller::sessionModel() const
{
    return m_sessionModel;
}

int Controller::nonActiveUserUnits() const
{
    return m_noActUserUnits;
}

int Controller::nonActiveSystemUnits() const
{
    return m_noActSystemUnits;
}

void Controller::slotRefreshUnitsList(dbusBus bus)
{
    if (bus == user && m_userBusPath.isEmpty()) {
        return;
    }

    OrgFreedesktopSystemd1ManagerInterface *interface = bus == sys ? m_systemManagerInterface : m_sessionManagerInterface;

    auto job = new UnitsFetchJob(m_systemManagerInterface);
    connect(job, &UnitsFetchJob::finished, this, [this, bus, job](KJob *) {
        if (bus == user) {
            m_userUnitsList.clear();
            m_userUnitsList = job->units();
            m_noActUserUnits = 0;
            for (const SystemdUnit &unit : m_userUnitsList) {
                if (unit.active_state == QLatin1StringView("active"))
                    m_noActUserUnits++;
            }
            m_userUnitModel->dataChanged(m_userUnitModel->index(0, 0), m_userUnitModel->index(m_userUnitModel->rowCount(), 0));
            Q_EMIT userUnitsRefreshed();
        } else {
            m_systemUnitsList.clear();
            m_systemUnitsList = job->units();

            for (const SystemdUnit &unit : m_systemUnitsList) {
                if (unit.active_state == QLatin1String("active"))
                    m_noActSystemUnits++;
            }

            m_systemUnitModel->dataChanged(m_systemUnitModel->index(0, 0), m_systemUnitModel->index(m_systemUnitModel->rowCount(), 0));
            Q_EMIT systemUnitsRefreshed();
        }
    });
    job->start();
}

void Controller::slotRefreshSessionList()
{
    // Updates the session list
    qDebug() << "Refreshing session list...";

    // clear list
    m_sessionList.clear();

    auto job = new SessionsFetchJob(m_loginManagerInterface);
    connect(job, &SessionsFetchJob::finished, this, [this, job](KJob *) {
        if (job->error()) {
            Q_EMIT errorOccured(i18n("Unable to fetch sessions list: %1", job->error()));
            return;
        }

        m_sessionList = job->sessions();

        // Iterate through the new list and compare to model
        for (const SystemdSession &s : m_sessionList) {
            // This is needed to get the "State" property

            QList<QStandardItem *> items = m_sessionModel->findItems(s.session_id, Qt::MatchExactly, 0);

            auto session =
                std::make_unique<OrgFreedesktopLogin1SessionInterface>(u"org.freedesktop.login1"_s, s.session_path.path(), QDBusConnection::systemBus());

            if (items.isEmpty()) {
                // New session discovered so add it to the model
                QList<QStandardItem *> row;
                row << new QStandardItem(s.session_id) << new QStandardItem(s.session_path.path()) << new QStandardItem(session->state())
                    << new QStandardItem(QString::number(s.user_id)) << new QStandardItem(s.user_name) << new QStandardItem(s.seat_id);
                m_sessionModel->appendRow(row);
            } else {
                m_sessionModel->item(items.at(0)->row(), 2)->setData(session->state(), Qt::DisplayRole);
            }
        }

        // Check to see if any sessions were removed
        if (m_sessionModel->rowCount() != m_sessionList.size()) {
            QList<QPersistentModelIndex> indexes;
            // Loop through model and compare to retrieved m_sessionList
            for (int row = 0; row < m_sessionModel->rowCount(); ++row) {
                SystemdSession session;
                session.session_id = m_sessionModel->index(row, 0).data().toString();
                if (!m_sessionList.contains(session)) {
                    // Add removed units to list for deletion
                    // qDebug() << "Unit removed: " << systemUnitModel->index(row,0).data().toString();
                    indexes << m_sessionModel->index(row, 0);
                }
            }
            // Delete the identified units from model
            for (const QPersistentModelIndex &i : indexes)
                m_sessionModel->removeRow(i.row());
        }

        // Update the text color in model
        for (int row = 0; row < m_sessionModel->rowCount(); ++row) {
            QBrush newcolor;
            const KColorScheme scheme(QPalette::Normal);
            if (m_sessionModel->data(m_sessionModel->index(row, 2), Qt::DisplayRole) == QLatin1String("active"))
                newcolor = scheme.foreground(KColorScheme::PositiveText);
            else if (m_sessionModel->data(m_sessionModel->index(row, 2), Qt::DisplayRole) == QLatin1String("closing"))
                newcolor = scheme.foreground(KColorScheme::InactiveText);
            else
                newcolor = scheme.foreground(KColorScheme::NormalText);

            for (int col = 0; col < m_sessionModel->columnCount(); ++col)
                m_sessionModel->setData(m_sessionModel->index(row, col), QVariant(newcolor), Qt::ForegroundRole);
        }
        Q_EMIT sessionsRefreshed();
    });

    job->start();
}

SystemdUnit Controller::systemUnit(int index) const
{
    return m_systemUnitsList.at(index);
}

SystemdUnit Controller::userUnit(int index) const
{
    return m_userUnitsList.at(index);
}

void Controller::slotUserSystemdReloading(bool status)
{
    if (!status) {
        Q_EMIT message(i18nc("%1 is a time", "%1: User daemon reloaded...", QLocale().toString(QDateTime::currentDateTime().time(), QLocale::ShortFormat)));
        slotRefreshUnitsList(user);
    }
}

void Controller::slotSystemSystemdReloading(bool status)
{
    if (!status) {
        Q_EMIT message(i18nc("%1 is a time", "%1: System daemon reloaded...", QLocale().toString(QDateTime::currentDateTime().time(), QLocale::ShortFormat)));
        slotRefreshUnitsList(sys);
    }
}

void Controller::slotUserJobNew(uint id, const QDBusObjectPath &path, const QString &unit)
{
    qDebug() << "UserJobNew: " << id << path.path() << unit;
    slotRefreshUnitsList(user);
}

void Controller::slotUserJobRemoved(uint id, const QDBusObjectPath &path, const QString &unit, const QString &result)
{
    qDebug() << "UserJobRemoved: " << id << path.path() << unit << result;
    slotRefreshUnitsList(user);
}

void Controller::slotSystemJobNew(uint id, const QDBusObjectPath &path, const QString &unit)
{
    qDebug() << "SystemJobNew: " << id << path.path() << unit;
    slotRefreshUnitsList(sys);
}

void Controller::slotSystemJobRemoved(uint id, const QDBusObjectPath &path, const QString &unit, const QString &result)
{
    qDebug() << "SystemJobRemoved: " << id << path.path() << unit << result;
    slotRefreshUnitsList(sys);
}

void Controller::slotSystemPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps)
{
    Q_UNUSED(changedProps)
    Q_UNUSED(invalidatedProps)

    qDebug() << "systemPropertiesChanged:" << iface; // << changedProps << invalidatedProps;
    slotRefreshUnitsList(sys);
}

void Controller::slotUserPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps)
{
    Q_UNUSED(changedProps)
    Q_UNUSED(invalidatedProps)

    qDebug() << "userPropertiesChanged:" << iface; // << changedProps << invalidatedProps;
    slotRefreshUnitsList(user);
}

void Controller::slotLoginPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps)
{
    Q_UNUSED(iface)
    Q_UNUSED(changedProps)
    Q_UNUSED(invalidatedProps)

    // qDebug() << "Logind properties changed on iface " << iface_name;
    slotRefreshSessionList();
}

void Controller::slotSystemUnitFilesChanged()
{
    // qDebug() << "System unitFilesChanged";
    slotRefreshUnitsList(sys);
    Q_EMIT message(i18nc("%1 is a time", "%1: System units changed...", QLocale().toString(QDateTime::currentDateTime().time(), QLocale::ShortFormat)));
}

void Controller::slotUserUnitFilesChanged()
{
    // qDebug() << "User unitFilesChanged";
    slotRefreshUnitsList(user);
}
