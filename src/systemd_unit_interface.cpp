// This file was generated by a modified version of qdbusxml2cpp version 0.8
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "systemd_unit_interface.h"
#include <QDBusMetaType>

/*
 * Implementation of interface class OrgFreedesktopSystemd1UnitInterface
 */

using namespace Qt::StringLiterals;

OrgFreedesktopSystemd1UnitInterface::OrgFreedesktopSystemd1UnitInterface(const QString &service,
                                                                         const QString &path,
                                                                         const QDBusConnection &_connection,
                                                                         const char *interface,
                                                                         QObject *parent)
    : QDBusAbstractInterface(service, path, interface, _connection, parent)
{
}

OrgFreedesktopSystemd1UnitInterface::OrgFreedesktopSystemd1UnitInterface(const QString &service,
                                                                         const QString &path,
                                                                         const QDBusConnection &_connection,
                                                                         QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), _connection, parent)
{
}

OrgFreedesktopSystemd1UnitInterface::~OrgFreedesktopSystemd1UnitInterface()
{
}

void OrgFreedesktopSystemd1UnitInterface::handlePropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps)
{
    Q_UNUSED(invalidatedProps);
    const QMetaObject *mo = metaObject();
    for (const auto [property, newValue] : changedProps.asKeyValueRange()) {
        int ind = mo->indexOfProperty(property.toLatin1().constData());
        if (ind < 0)
            continue;
        QMetaProperty metaProperty = mo->property(ind);
        QMetaMethod signal = metaProperty.notifySignal();
        if (!signal.isValid())
            continue;
        QVariant returnValue;
        const auto type = metaProperty.metaType();
        if (newValue.metaType() == type || type.id() == QMetaType::QVariant) {
            returnValue = newValue;
        } else if (newValue.metaType() == QMetaType::fromType<QDBusArgument>()) {
            QDBusArgument arg = qvariant_cast<QDBusArgument>(newValue);
            QDBusMetaType::demarshall(arg, QMetaType(metaProperty.metaType()), &returnValue);
        }
        metaProperty.write(this, returnValue);
        signal.invoke(this, Qt::DirectConnection, QGenericReturnArgument(nullptr));
    }
}

#include "moc_systemd_unit_interface.cpp"
