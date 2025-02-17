/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "systemdunit.h"
#include "sortfilterunitmodel.h"
#include "unitmodel.h"

#include <KMessageWidget>
#include <KXmlGuiWindow>

#include <QDBusConnection>
#include <QStandardItemModel>

#include "systemd_manager_interface.h"
#include "login_manager_interface.h"

struct conffile
{
    QString filePath;
    QString manPage;
    QString description;

    conffile(QString f, QString m, QString d)
    {
        filePath = f;
        manPage = m;
        description = d;
    }

    conffile(){}

    conffile(QString f)
    {
        filePath = f;
    }

    bool operator==(const QString& right) const
    {
        if (filePath == right)
            return true;
        else
            return false;
    }

    bool operator==(const conffile& right) const
    {
        if (filePath == right.filePath)
            return true;
        else
            return false;
    }
};

enum dbusConn
{
    systemd, logind
};

enum dbusIface
{
    sysdMgr, sysdUnit, sysdTimer, logdMgr, logdSession
};

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow ui;

    void setupSignalSlots();
    void setupUnitslist();
    void setupSessionlist();
    void setupTimerlist();
    void setupConfFilelist();
    void authServiceAction(const QString &, const QString &, const QString &, const QString &, const QList<QVariant> &);
    bool eventFilter(QObject *, QEvent*) override;
    void updateUnitCount();
    void displayMsgWidget(KMessageWidget::MessageType type, QString msg);
    void setupActions();
    void openEditor(const QString &file);
    QVariant getDbusProperty(QString prop, dbusIface ifaceName, QDBusObjectPath path = QDBusObjectPath("/org/freedesktop/systemd1"), dbusBus bus = sys);
    QDBusMessage callDbusMethod(QString method, dbusIface ifaceName, dbusBus bus = sys, const QList<QVariant> &args = QList<QVariant> ());
    QList<QStandardItem *> buildTimerListRow(const SystemdUnit &unit, const QVector<SystemdUnit> &list, dbusBus bus);
    void executeUnitAction(const QString &method);
    void executeSystemDaemonAction(const QString &method);
    void executeUserDaemonAction(const QString &method);
    void executeSessionAction(const QString &method);


    SortFilterUnitModel *m_systemUnitFilterModel;
    SortFilterUnitModel *m_userUnitFilterModel;
    QStandardItemModel *m_sessionModel;
    QStandardItemModel *m_timerModel;
    QStandardItemModel *m_confFileModel;
    UnitModel *m_systemUnitModel;
    UnitModel *m_userUnitModel;
    QVector<SystemdUnit> m_systemUnitsList;
    QVector<SystemdUnit> m_userUnitsList;
    QVector<SystemdSession> m_sessionList;
    QVector<conffile> m_confFileList;
    QString m_userBusPath;
    int systemdVersion;
    int lastSessionRowChecked = -1;
    int m_noActSystemUnits;
    int m_noActUserUnits;
    bool enableUserUnits = true;
    QTimer *timer;
    const QStringList unitTypeSufx = QStringList{QString(),
                                                 QStringLiteral(".service"),
                                                 QStringLiteral(".automount"),
                                                 QStringLiteral(".device"),
                                                 QStringLiteral(".mount"),
                                                 QStringLiteral(".path"),
                                                 QStringLiteral(".scope"),
                                                 QStringLiteral(".slice"),
                                                 QStringLiteral(".socket"),
                                                 QStringLiteral(".swap"),
                                                 QStringLiteral(".target"),
                                                 QStringLiteral(".timer")};
    const QString connSystemd = QStringLiteral("org.freedesktop.systemd1");
    const QString connLogind = QStringLiteral("org.freedesktop.login1");
    const QString pathSysdMgr = QStringLiteral("/org/freedesktop/systemd1");
    const QString pathLogdMgr = QStringLiteral("/org/freedesktop/login1");
    const QString ifaceMgr = QStringLiteral("org.freedesktop.systemd1.Manager");
    const QString ifaceLogdMgr = QStringLiteral("org.freedesktop.login1.Manager");
    const QString ifaceUnit = QStringLiteral("org.freedesktop.systemd1.Unit");
    const QString ifaceTimer = QStringLiteral("org.freedesktop.systemd1.Timer");
    const QString ifaceSession = QStringLiteral("org.freedesktop.login1.Session");
    const QString ifaceDbusProp = QStringLiteral("org.freedesktop.DBus.Properties");
    QDBusConnection m_systemBus = QDBusConnection::systemBus();

    QLabel *m_lblLog;

    QAction *m_refreshAction;
    QAction *m_reloadSystemDaemonAction;
    QAction *m_reexecSystemDaemonAction;
    QAction *m_reloadUserDaemonAction;
    QAction *m_reexecUserDaemonAction;

    QAction *m_startUnitAction;
    QAction *m_stopUnitAction;
    QAction *m_reloadUnitAction;
    QAction *m_restartUnitAction;

    QAction *m_enableUnitAction;
    QAction *m_disableUnitAction;
    QAction *m_maskUnitAction;
    QAction *m_unmaskUnitAction;
    QAction *m_editUnitFileAction;

    QAction *m_editConfFileAction;
    QAction *m_openManPageAction;

    QAction *m_activateSessionAction;
    QAction *m_terminateSessionAction;
    QAction *m_lockSessionAction;

    OrgFreedesktopSystemd1ManagerInterface * const m_systemManagerInterface;
    OrgFreedesktopSystemd1ManagerInterface *m_sessionManagerInterface = nullptr;
    OrgFreedesktopLogin1ManagerInterface * const m_loginManagerInterface;

private Q_SLOTS:
    void quit();

    void updateActions();
    void slotChkShowUnits(int);
    void slotCmbUnitTypes(int);
    void slotUnitContextMenu(const QPoint &pos);
    void slotConfFileContextMenu(const QPoint &pos);
    void slotSessionContextMenu(const QPoint &);
    void slotRefreshUnitsList(bool inital, dbusBus bus);
    void slotRefreshSessionList();
    void slotRefreshTimerList();
    void slotRefreshConfFileList();

    void slotSystemSystemdReloading(bool);
    void slotSystemUnitFilesChanged();
    //void slotSystemUnitNew(const QString &id, const QDBusObjectPath &path);
    //void slotSystemUnitRemoved(const QString &id, const QDBusObjectPath &path);
    void slotSystemJobNew(uint id, const QDBusObjectPath &path, const QString &unit);
    void slotSystemJobRemoved(uint id, const QDBusObjectPath &path, const QString &unit, const QString &result);
    void slotSystemPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps);

    void slotUserSystemdReloading(bool);
    void slotUserUnitFilesChanged();
    //void slotUserUnitNew(const QString &id, const QDBusObjectPath &path);
    //void slotUserUnitRemoved(const QString &id, const QDBusObjectPath &path);
    void slotUserJobNew(uint id, const QDBusObjectPath &path, const QString &unit);
    void slotUserJobRemoved(uint id, const QDBusObjectPath &path, const QString &unit, const QString &result);
    void slotUserPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps);

    void slotLogindPropertiesChanged(const QString &iface, const QVariantMap &changedProps, const QStringList &invalidatedProps);
    void slotLeSearchUnitChanged(QString);
    void slotUpdateTimers();
    void slotRefreshAll();

    void slotEditUnitFile();
    void slotEditConfFile();
    void slotOpenManPage();
};

#endif // MAINWINDOW_H
