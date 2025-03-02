// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QObject>
#include <qqmlregistration.h>

class OrgFreedesktopSystemd1ManagerInterface;

class Controller : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit Controller(QObject *parent = nullptr);

    Q_INVOKABLE void executeSystemDaemonAction(const QString &method);
    Q_INVOKABLE void executeUserDaemonAction(const QString &method);

private:
    OrgFreedesktopSystemd1ManagerInterface *const m_interface;
    OrgFreedesktopSystemd1ManagerInterface *m_interfaceUser;
};
