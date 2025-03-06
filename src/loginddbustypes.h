// SPDX-FileCopyrightText: 2019 David Edmundson <kde@davidedmundson.co.uk>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDBusArgument>
#include <QDBusObjectPath>

struct SessionInfo {
    QString sessionId;
    uint userId;
    QString userName;
    QString seatId;
    QDBusObjectPath sessionPath;
};

typedef QList<SessionInfo> SessionInfoList;

inline QDBusArgument &operator<<(QDBusArgument &argument, const SessionInfo &sessionInfo)
{
    argument.beginStructure();
    argument << sessionInfo.sessionId;
    argument << sessionInfo.userId;
    argument << sessionInfo.userName;
    argument << sessionInfo.seatId;
    argument << sessionInfo.sessionPath;
    argument.endStructure();

    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, SessionInfo &sessionInfo)
{
    argument.beginStructure();
    argument >> sessionInfo.sessionId;
    argument >> sessionInfo.userId;
    argument >> sessionInfo.userName;
    argument >> sessionInfo.seatId;
    argument >> sessionInfo.sessionPath;
    argument.endStructure();

    return argument;
}

struct UserInfo {
    uint userId;
    QString name;
    QDBusObjectPath path;
};

typedef QList<UserInfo> UserInfoList;

inline QDBusArgument &operator<<(QDBusArgument &argument, const UserInfo &userInfo)
{
    argument.beginStructure();
    argument << userInfo.userId;
    argument << userInfo.name;
    argument << userInfo.path;
    argument.endStructure();

    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, UserInfo &userInfo)
{
    argument.beginStructure();
    argument >> userInfo.userId;
    argument >> userInfo.name;
    argument >> userInfo.path;
    argument.endStructure();

    return argument;
}

struct NamedSeatPath {
    QString name;
    QDBusObjectPath path;
};

inline QDBusArgument &operator<<(QDBusArgument &argument, const NamedSeatPath &namedSeat)
{
    argument.beginStructure();
    argument << namedSeat.name;
    argument << namedSeat.path;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, NamedSeatPath &namedSeat)
{
    argument.beginStructure();
    argument >> namedSeat.name;
    argument >> namedSeat.path;
    argument.endStructure();
    return argument;
}

typedef QList<NamedSeatPath> NamedSeatPathList;

typedef NamedSeatPath NamedSessionPath;
typedef NamedSeatPathList NamedSessionPathList;

struct NamedUserPath {
    uint userId;
    QDBusObjectPath path;
};

inline QDBusArgument &operator<<(QDBusArgument &argument, const NamedUserPath &namedUser)
{
    argument.beginStructure();
    argument << namedUser.userId;
    argument << namedUser.path;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, NamedUserPath &namedUser)
{
    argument.beginStructure();
    argument >> namedUser.userId;
    argument >> namedUser.path;
    argument.endStructure();
    return argument;
}

struct Inhibitor {
    QString what;
    QString who;
    QString why;
    QString mode;
    uint userId;
    uint processId;
};

typedef QList<Inhibitor> InhibitorList;

inline QDBusArgument &operator<<(QDBusArgument &argument, const Inhibitor &inhibitor)
{
    argument.beginStructure();
    argument << inhibitor.what;
    argument << inhibitor.who;
    argument << inhibitor.why;
    argument << inhibitor.mode;
    argument << inhibitor.userId;
    argument << inhibitor.processId;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, Inhibitor &inhibitor)
{
    argument.beginStructure();
    argument >> inhibitor.what;
    argument >> inhibitor.who;
    argument >> inhibitor.why;
    argument >> inhibitor.mode;
    argument >> inhibitor.userId;
    argument >> inhibitor.processId;
    argument.endStructure();
    return argument;
}
