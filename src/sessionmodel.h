// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "login_session_interface.h"
#include "loginddbustypes.h"

#include <QAbstractTableModel>
#include <QWindow>
#include <qqmlregistration.h>

class OrgFreedesktopLogin1ManagerInterface;

class Login1SessionInterface : public OrgFreedesktopLogin1SessionInterface
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit Login1SessionInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

public Q_SLOTS:
    void slotPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps);

Q_SIGNALS:
    void stateChanged();
};

class SessionModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Columns {
        SessionIdColumn,
        SessionObjectPathColumn,
        StateColumn,
        UserIdColumn,
        UserNameColumn,
        SeatIdColumn,
        ColumnCount,
    };

    explicit SessionModel(QObject *parent = nullptr);
    ~SessionModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void executeAction(int row, const QString &method, QWindow *window);

public Q_SLOTS:
    void slotRefreshSessionList();

Q_SIGNALS:
    void errorOccured(const QString &text);
    void sessionsRefreshed();

private:
    void slotSessionNew(const QString &id, const QDBusObjectPath &path);
    void slotSessionRemoved(const QString &id, const QDBusObjectPath &path);

    struct Session {
        SessionInfo info;
        std::unique_ptr<Login1SessionInterface> sessionIface;
    };

    std::vector<Session> m_sessions;
    OrgFreedesktopLogin1ManagerInterface *const m_loginManagerInterface;
};
