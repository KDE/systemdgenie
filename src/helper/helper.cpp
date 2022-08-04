/*******************************************************************************
 * Copyright (C) 2016 Ragnar Thomsen <rthomsen6@gmail.com>                     *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the Free  *
 * Software Foundation, either version 2 of the License, or (at your option)   *
 * any later version.                                                          *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
 * more details.                                                               *
 *                                                                             *
 * You should have received a copy of the GNU General Public License along     *
 * with this program. If not, see <http://www.gnu.org/licenses/>.              *
 *******************************************************************************/

#include "helper.h"

#include <QtDBus>
#include <QFile>

#include <KAuth/HelperSupport>

ActionReply Helper::saveunitfile(const QVariantMap& args)
{
    ActionReply reply;

    QFile file(args[QStringLiteral("file")].toString());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        reply = ActionReply::HelperErrorReply();
        reply.addData(QStringLiteral("errorDescription"), file.errorString());
        //reply.setErrorCode(file.error());
        //reply.addData("filename", iter.key());
        return reply;
    }
    QTextStream stream(&file);
    stream << args[QStringLiteral("contents")].toString();
    file.close();

    return reply;
}

ActionReply Helper::dbusaction(const QVariantMap& args)
{
    ActionReply reply;
    QDBusMessage dbusreply;

    // Get arguments to method call
    QString service = args[QStringLiteral("service")].toString();
    QString path = args[QStringLiteral("path")].toString();
    QString interface = args[QStringLiteral("interface")].toString();
    QString method = args[QStringLiteral("method")].toString();
    QList<QVariant> argsForCall = args[QStringLiteral("argsForCall")].toList();

    QDBusInterface *iface = new QDBusInterface(service,
                                               path,
                                               interface,
                                               QDBusConnection::systemBus(),
                                               this);
    if (iface->isValid())
        dbusreply = iface->callWithArgumentList(QDBus::AutoDetect, method, argsForCall);
    delete iface;

    // Error handling
    if (method != QLatin1String("Reexecute"))
    {
        if (dbusreply.type() == QDBusMessage::ErrorMessage)
        {
            reply.setErrorCode(ActionReply::DBusError);
            reply.setErrorDescription(dbusreply.errorMessage());
        }
    }

    // Reload systemd daemon to update the enabled/disabled status
    QRegularExpression rxMethods(QStringLiteral("EnableUnitFiles|DisableUnitFiles|MaskUnitFiles|UnmaskUnitFiles"));
    if (rxMethods.match(method).hasMatch())
    {
        // systemd does not update properties when these methods are called so we
        // need to reload the systemd daemon.
        iface = new QDBusInterface (QStringLiteral("org.freedesktop.systemd1"),
                                    QStringLiteral("/org/freedesktop/systemd1"),
                                    QStringLiteral("org.freedesktop.systemd1.Manager"),
                                    QDBusConnection::systemBus(),
                                    this);
        dbusreply = iface->call(QDBus::AutoDetect, QStringLiteral("Reload"));
        delete iface;
    }
    return reply;
}

KAUTH_HELPER_MAIN("org.kde.kcontrol.systemdgenie", Helper)
