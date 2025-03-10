// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "controller.h"
#include "systemd_manager_interface.h"
#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KIO/ApplicationLauncherJob>
#include <KIO/JobUiDelegateFactory>
#include <KService>
#include <QDBusPendingCall>

using namespace Qt::StringLiterals;

Controller::Controller(QObject *parent)
    : QObject(parent)
    , m_interface(new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd1"_s, u"/org/freedesktop/systemd1"_s, QDBusConnection::systemBus(), this))
{
    QString userBus;
    if (QFileInfo::exists(QStringLiteral("/run/user/%1/bus").arg(QString::number(getuid())))) {
        userBus = QStringLiteral("unix:path=/run/user/%1/bus").arg(QString::number(getuid()));
    } else if (QFileInfo::exists(QStringLiteral("/run/user/%1/dbus/user_bus_socket").arg(QString::number(getuid())))) {
        userBus = QStringLiteral("unix:path=/run/user/%1/dbus/user_bus_socket").arg(QString::number(getuid()));
    }

    if (!userBus.isEmpty()) {
        auto connection = QDBusConnection::connectToBus(userBus, u"org.freedesktop.systemd1"_s);
        m_interfaceUser = new OrgFreedesktopSystemd1ManagerInterface(u"org.freedesktop.systemd1"_s, u"/org/freedesktop/systemd1"_s, connection, this);
    }
}

void Controller::executeSystemDaemonAction(const QString &method)
{
    QVariantMap helperArgs;
    helperArgs[QStringLiteral("service")] = u"org.freedesktop.systemd1"_s;
    helperArgs[QStringLiteral("path")] = m_interface->path();
    helperArgs[QStringLiteral("interface")] = m_interface->interface();
    helperArgs[QStringLiteral("method")] = method;
    helperArgs[QStringLiteral("argsForCall")] = QVariantList{};

    // Call the helper. This call causes the debug output: "QDBusArgument: read from a write-only object"
    KAuth::Action serviceAction(QStringLiteral("org.kde.kcontrol.systemdgenie.dbusaction"));
    serviceAction.setHelperId(QStringLiteral("org.kde.kcontrol.systemdgenie"));
    serviceAction.setArguments(helperArgs);

    KAuth::ExecuteJob *job = serviceAction.execute();
    job->exec();
}

void Controller::executeUserDaemonAction(const QString &method)
{
    auto call = m_interfaceUser->asyncCallWithArgumentList(method, {});
    auto watcher = new QDBusPendingCallWatcher(call, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, call, method](QDBusPendingCallWatcher *) {
        if (call.isError()) {
            return;
        }
        if (QRegularExpression(QStringLiteral("EnableUnitFiles|DisableUnitFiles|MaskUnitFiles|UnmaskUnitFiles")).match(method).hasMatch()) {
            m_interfaceUser->Reload();
        }
    });
}

bool Controller::canViewLogs() const
{
    const bool hasLogViewer = KService::serviceByDesktopName(u"org.kde.kjournaldbrowser"_s) || KService::serviceByDesktopName(u"org.kde.ksystemlog.desktop"_s);
    return hasLogViewer;
}

void Controller::viewLogs()
{
    const auto kJournaldBrowser = KService::serviceByDesktopName(u"org.kde.kjournaldbrowser"_s);
    const auto kSystemLog = KService::serviceByDesktopName(u"org.kde.ksystemlog.desktop"_s);
    if (!kJournaldBrowser && !kSystemLog) {
        return;
    }

    auto availableLogViewer = kJournaldBrowser ? kJournaldBrowser : kSystemLog;

    auto job = new KIO::ApplicationLauncherJob(availableLogViewer);
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
    job->start();
}

#include "moc_controller.cpp"
