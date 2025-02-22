/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <KAboutData>
#include <KCrash>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QQmlApplicationEngine>
#include <qdbusmetatype.h>

#include "mainwindow.h"
#include "systemdgenie_version.h"
#include "systemdunit.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    KCrash::initialize();

    KLocalizedString::setApplicationDomain("systemdgenie");

    QGuiApplication::setDesktopFileName(QStringLiteral("org.kde.systemdgenie"));

    KAboutData aboutData(QStringLiteral("systemdgenie"),
                         i18n("SystemdGenie"),
                         QStringLiteral(SYSTEMDGENIE_VERSION_STRING),
                         i18n("Manage systemd units, timers, sessions and config files"),
                         KAboutLicense::GPL,
                         i18n("(c) 2016, Ragnar Thomsen"),
                         QString(),
                         QString());

    aboutData.setOrganizationDomain("kde.org");

    aboutData.addAuthor(i18n("Ragnar Thomsen"), i18n("Maintainer"), QStringLiteral("rthomsen6@gmail.com"));

    KAboutData::setApplicationData(aboutData);

    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("preferences-system-services")));

    QCommandLineParser parser;
    QCommandLineOption quick(QStringList{QStringLiteral("q"), QStringLiteral("quick")}, QStringLiteral("Use qml view"));
    parser.addOption(quick);
    aboutData.setupCommandLine(&parser);

    // Do the command line parsing.
    parser.process(application);

    // Handle standard options.
    aboutData.processCommandLine(&parser);

    qDBusRegisterMetaType<SystemdUnit>();
    qDBusRegisterMetaType<QList<SystemdUnit>>();

    qDBusRegisterMetaType<SystemdSession>();
    qDBusRegisterMetaType<QList<SystemdSession>>();

    qDBusRegisterMetaType<UnitFile>();
    qDBusRegisterMetaType<QList<UnitFile>>();

    if (parser.isSet(quick)) {
        QQmlApplicationEngine engine;
        KLocalization::setupLocalizedContext(&engine);
        engine.loadFromModule("org.kde.systemdgenie", "Main");
        return application.exec();
    } else {
        MainWindow *window = new MainWindow;
        window->show();
        qDebug() << "Entering application loop";
        return application.exec();
    }
}
