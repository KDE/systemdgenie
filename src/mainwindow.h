/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "configfilemodel.h"
#include "controller.h"
#include "sortfilterunitmodel.h"
#include "systemdunit.h"
#include "ui_mainwindow.h"

#include <KMessageWidget>
#include <KXmlGuiWindow>

#include <QDBusConnection>
#include <QStandardItemModel>

enum dbusConn {
    systemd,
    logind
};

enum dbusIface {
    sysdMgr,
    sysdUnit,
    sysdTimer,
    logdMgr,
    logdSession
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
    void setupConfFilelist();
    void setupTimerlist();
    void authServiceAction(const QString &, const QString &, const QString &, const QString &, const QList<QVariant> &);
    void updateUnitCount();
    void displayMsgWidget(KMessageWidget::MessageType type, QString msg);
    void setupActions();
    void openEditor(const QString &file);
    QVariant getDbusProperty(QString prop, dbusIface ifaceName, QDBusObjectPath path = QDBusObjectPath("/org/freedesktop/systemd1"), dbusBus bus = sys);
    QDBusMessage callDbusMethod(QString method, dbusIface ifaceName, dbusBus bus = sys, const QList<QVariant> &args = QList<QVariant>());
    QList<QStandardItem *> buildTimerListRow(const SystemdUnit &unit, const QVector<SystemdUnit> &list, dbusBus bus);
    void executeUnitAction(const QString &method);
    void executeSystemDaemonAction(const QString &method);
    void executeUserDaemonAction(const QString &method);
    void executeSessionAction(const QString &method);

    Controller *m_controller;
    SortFilterUnitModel *m_systemUnitFilterModel;
    SortFilterUnitModel *m_userUnitFilterModel;
    QStandardItemModel *m_timerModel;
    ConfigFileModel *const m_configFileModel;
    QString m_userBusPath;
    int systemdVersion;
    int lastSessionRowChecked = -1;
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
    QAction *m_viewLogsAction;

    QAction *m_activateSessionAction;
    QAction *m_terminateSessionAction;
    QAction *m_lockSessionAction;

private Q_SLOTS:
    void quit();

    void updateActions();
    void slotChkShowUnits(int);
    void slotCmbUnitTypes(int);
    void slotUnitContextMenu(const QPoint &pos);
    void slotConfFileContextMenu(const QPoint &pos);
    void slotSessionContextMenu(const QPoint &);
    void slotRefreshTimerList();

    void slotLeSearchUnitChanged(QString);
    void slotUpdateTimers();
    void slotRefreshAll();

    void slotEditUnitFile();
    void slotEditConfFile();
    void slotOpenManPage();
    void slotViewLogs();
};

#endif // MAINWINDOW_H
