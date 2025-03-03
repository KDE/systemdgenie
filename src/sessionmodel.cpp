// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "sessionmodel.h"
#include "job/sessionsfetchjob.h"
#include "login_manager_interface.h"

#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KColorScheme>
#include <KIO/JobUiDelegateFactory>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

Login1SessionInterface::Login1SessionInterface(const QString &service, const QString &path, const QDBusConnection &connect, QObject *parent)
    : OrgFreedesktopLogin1SessionInterface(service, path, connect, parent)
{
    const auto connected = connection().connect(service,
                                                path,
                                                u"org.freedesktop.DBus.Properties"_s,
                                                u"PropertiesChanged"_s,
                                                this,
                                                SLOT(slotPropertiesChanged(QString, QVariantMap, QStringList)));
    Q_ASSERT(connected);
}

void Login1SessionInterface::slotPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps)
{
    Q_UNUSED(iface)
    Q_UNUSED(changedProps)
    Q_UNUSED(invalidatedProps)

    for (const auto [property, newValue] : changedProps.asKeyValueRange()) {
        if (property == "Active"_L1) {
            setProperty(property.toLatin1().data(), newValue);
            Q_EMIT stateChanged();
        }
    }
}

SessionModel::SessionModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_loginManagerInterface(
          new OrgFreedesktopLogin1ManagerInterface(u"org.freedesktop.login1"_s, u"/org/freedesktop/login1"_s, QDBusConnection::systemBus(), this))
{
    slotRefreshSessionList();

    connect(m_loginManagerInterface, &OrgFreedesktopLogin1ManagerInterface::SessionNew, this, &SessionModel::slotSessionNew);
    connect(m_loginManagerInterface, &OrgFreedesktopLogin1ManagerInterface::SessionRemoved, this, &SessionModel::slotSessionRemoved);
}

SessionModel::~SessionModel() = default;

int SessionModel::rowCount(const QModelIndex &) const
{
    return m_sessions.size();
}

int SessionModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant SessionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }
    switch (section) {
    case SessionIdColumn:
        return i18n("Session ID");
    case SessionObjectPathColumn:
        return i18n("Session Object Path"); // This column is hidden
    case StateColumn:
        return i18n("State");
    case UserIdColumn:
        return i18n("User ID");
    case UserNameColumn:
        return i18n("User Name");
    case SeatIdColumn:
        return i18n("Seat ID");
    default:
        return {};
    }
}

QHash<int, QByteArray> SessionModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "displayName"},
        {Qt::ToolTipRole, "tooltip"},
    };
}

QVariant SessionModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &session = m_sessions[index.row()];
    if (role == Qt::ForegroundRole) {
        QBrush newcolor;
        const KColorScheme scheme(QPalette::Normal);
        if (session.sessionIface->state() == "active"_L1) {
            return scheme.foreground(KColorScheme::PositiveText);
        } else if (session.sessionIface->state() == "closing"_L1) {
            return scheme.foreground(KColorScheme::InactiveText);
        } else {
            return scheme.foreground(KColorScheme::NormalText);
        }
    }

    if (role == Qt::ToolTipRole) {
        QString toolTipText;
        toolTipText.append(QStringLiteral("<FONT>"));
        toolTipText.append(QStringLiteral("<b>%1</b><hr>").arg(session.sessionIface->id()));

        // Session has a valid session DBus object
        toolTipText.append(i18n("<b>VT:</b> %1", session.sessionIface->vTNr()));

        QString remoteHost = session.sessionIface->remoteHost();
        if (session.sessionIface->remote()) {
            toolTipText.append(i18n("<br><b>Remote host:</b> %1", remoteHost));
            toolTipText.append(i18n("<br><b>Remote user:</b> %1", session.sessionIface->remoteUser()));
        }
        toolTipText.append(i18n("<br><b>Service:</b> %1", session.sessionIface->service()));
        toolTipText.append(i18n("<br><b>Leader (PID):</b> %1", session.sessionIface->leader()));

        QString type = session.sessionIface->type();
        toolTipText.append(i18n("<br><b>Type:</b> %1", type));
        if (type == QLatin1String("x11"))
            toolTipText.append(i18n(" (display %1)", session.sessionIface->display()));
        else if (type == QLatin1String("tty")) {
            QString path, tty = session.sessionIface->tTY();
            if (!tty.isEmpty())
                path = tty;
            else if (!remoteHost.isEmpty())
                path = QStringLiteral("%1@%2").arg(session.sessionIface->name()).arg(remoteHost);
            toolTipText.append(QStringLiteral(" (%1)").arg(path));
        }
        toolTipText.append(i18n("<br><b>Class:</b> %1", session.sessionIface->className()));
        toolTipText.append(i18n("<br><b>State:</b> %1", session.sessionIface->state()));
        toolTipText.append(i18n("<br><b>Scope:</b> %1", session.sessionIface->scope()));

        toolTipText.append(i18n("<br><b>Created: </b>"));
        if (session.sessionIface->timestamp() == 0)
            toolTipText.append(QStringLiteral("n/a"));
        else {
            QDateTime time;
            time.setMSecsSinceEpoch(session.sessionIface->timestamp() / 1000);
            toolTipText.append(time.toString());
        }

        toolTipText.append(QStringLiteral("</FONT"));
        return toolTipText;
    }

    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (index.column()) {
    case SessionIdColumn:
        return session.info.sessionId;
    case SessionObjectPathColumn:
        return session.info.sessionPath.path();
    case StateColumn:
        return session.sessionIface->state();
    case UserIdColumn:
        return session.info.userId;
    case UserNameColumn:
        return session.info.userName;
    case SeatIdColumn:
        return session.info.seatId;
    default:
        return {};
    }
}

void SessionModel::slotRefreshSessionList()
{
    auto job = new SessionsFetchJob(m_loginManagerInterface);
    connect(job, &SessionsFetchJob::finished, this, [this, job](KJob *) {
        if (job->error()) {
            Q_EMIT errorOccured(i18n("Unable to fetch sessions list: %1", job->error()));
            return;
        }

        const auto sessions = job->sessions();

        // Iterate through the new list and compare to model
        for (const SessionInfo &info : sessions) {
            // This is needed to get the "State" property

            const auto it = std::ranges::find_if(m_sessions, [info](const auto &session) {
                return session.info.sessionId == info.sessionId;
            });
            if (it == m_sessions.cend()) {
                beginInsertRows({}, m_sessions.size(), m_sessions.size());
                m_sessions.push_back(
                    {info, std::make_unique<Login1SessionInterface>(u"org.freedesktop.login1"_s, info.sessionPath.path(), QDBusConnection::systemBus())});
                endInsertRows();
            }
        }

        Q_EMIT sessionsRefreshed();
    });

    job->start();
}

void SessionModel::slotSessionNew(const QString &id, const QDBusObjectPath &path)
{
    Q_UNUSED(id);
    Q_UNUSED(path);
    slotRefreshSessionList();
}

void SessionModel::slotSessionRemoved(const QString &id, const QDBusObjectPath &path)
{
    Q_UNUSED(id);
    Q_UNUSED(path);
    slotRefreshSessionList();

    const auto it = std::ranges::find_if(m_sessions, [id](const auto &session) {
        return session.info.sessionId == id;
    });

    qWarning() << "removed" << id << path << m_sessions.cbegin() - it;

    if (it != m_sessions.cend()) {
        beginRemoveRows({}, m_sessions.cbegin() - it, m_sessions.cbegin() - it);
        m_sessions.erase(it);
        endInsertRows();
    }
}

void SessionModel::executeAction(int row, const QString &method, QWindow *window)
{
    const auto idx = index(row, 0);
    if (!idx.isValid()) {
        qWarning() << "invalid index" << row;
        return;
    }
    const auto &session = m_sessions.at(row);

    QVariantMap helperArgs;
    helperArgs[QStringLiteral("service")] = m_loginManagerInterface->service();
    helperArgs[QStringLiteral("path")] = session.sessionIface->path();
    helperArgs[QStringLiteral("interface")] = m_loginManagerInterface->interface();
    helperArgs[QStringLiteral("method")] = method;
    helperArgs[QStringLiteral("argsForCall")] = QVariantList{};

    // Call the helper. This call causes the debug output: "QDBusArgument: read from a write-only object"
    KAuth::Action serviceAction(QStringLiteral("org.kde.kcontrol.systemdgenie.dbusaction"));
    serviceAction.setHelperId(QStringLiteral("org.kde.kcontrol.systemdgenie"));
    serviceAction.setArguments(helperArgs);
    serviceAction.setParentWindow(window);

    KAuth::ExecuteJob *job = serviceAction.execute();
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
    job->exec();

    if (!job->exec()) {
        Q_EMIT errorOccured(i18n("Unable to authenticate/execute the action: %1", job->errorString()));
    }
}

bool SessionModel::canLock(int row)
{
    if (row == -1 || row >= (int)m_sessions.size()) {
        return false;
    }

    return m_sessions.at(row).sessionIface->type() != "tty"_L1;
}
