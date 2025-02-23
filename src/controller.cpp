// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "controller.h"

#include <QDBusConnection>

#include <KColorScheme>
#include <KLocalizedString>

#include "job/unitsfetchjob.h"
#include "systemd_manager_interface.h"

using namespace Qt::StringLiterals;

constexpr static const QLatin1StringView connSystemd = "org.freedesktop.systemd1"_L1;

Controller::Controller(QObject *parent)
    : QObject(parent)
    , m_sessionModel(new SessionModel(this))
    , m_systemManagerInterface(
          new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd1"_s, u"/org/freedesktop/systemd1"_s, QDBusConnection::systemBus(), this))
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
        m_sessionManagerInterface = new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd1"_s, u"/org/freedesktop/systemd1"_s, connection, this);

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

    m_systemUnitModel = new UnitModel(this);
    m_userUnitModel = new UnitModel(m_userBusPath, this);
}

UnitModel *Controller::systemUnitModel() const
{
    return m_systemUnitModel;
}

UnitModel *Controller::userUnitModel() const
{
    return m_userUnitModel;
}

SessionModel *Controller::sessionModel() const
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

void Controller::slotRefreshSessionList()
{
    m_sessionModel->slotRefreshSessionList();
}

void Controller::slotRefreshSystemUnitsList()
{
    slotRefreshUnitsList(sys);
}

void Controller::slotRefreshUserUnitsList()
{
    slotRefreshUnitsList(user);
}

void Controller::slotRefreshUnitsList(dbusBus bus)
{
    if (bus == user && m_userBusPath.isEmpty()) {
        return;
    }

    OrgFreedesktopSystemd1ManagerInterface *interface = bus == sys ? m_systemManagerInterface : m_sessionManagerInterface;

    auto job = new UnitsFetchJob(interface);
    connect(job, &UnitsFetchJob::finished, this, [this, bus, job](KJob *) {
        const auto units = job->units();
        if (bus == user) {
            m_userUnitModel->setUnits(units);
            m_noActUserUnits = 0;
            for (const SystemdUnit &unit : units) {
                if (unit.active_state == "active"_L1)
                    m_noActUserUnits++;
            }
            m_userUnitModel->dataChanged(m_userUnitModel->index(0, 0), m_userUnitModel->index(m_userUnitModel->rowCount(), 0));
            Q_EMIT userUnitsRefreshed();
        } else {
            m_systemUnitModel->setUnits(units);

            for (const SystemdUnit &unit : units) {
                if (unit.active_state == QLatin1String("active"))
                    m_noActSystemUnits++;
            }

            m_systemUnitModel->dataChanged(m_systemUnitModel->index(0, 0), m_systemUnitModel->index(m_systemUnitModel->rowCount(), 0));
            Q_EMIT systemUnitsRefreshed();
        }
    });
    job->start();
}

SystemdUnit Controller::systemUnit(int index) const
{
    return m_systemUnitModel->unitsConst().at(index);
}

SystemdUnit Controller::userUnit(int index) const
{
    return m_userUnitModel->unitsConst().at(index);
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
