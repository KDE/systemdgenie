/*
    SPDX-FileCopyrightText: 2016 Ragnar Thomsen <rthomsen6@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "systemdgenie_version.h"
#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    KCrash::initialize();

    KLocalizedString::setApplicationDomain("systemdgenie");

    QGuiApplication::setDesktopFileName(QStringLiteral("org.kde.systemdgenie"));

    KAboutData aboutData(QStringLiteral("systemdgenie"),
                         i18n("Services Manager"),
                         QStringLiteral(SYSTEMDGENIE_VERSION_STRING),
                         i18n("Manage systemd units, timers, sessions and config files"),
                         KAboutLicense::GPL,
                         i18n("(c) 2016, Ragnar Thomsen"),
                         QString(),
                         QString()
                         );

    aboutData.setOrganizationDomain("kde.org");

    aboutData.addAuthor(i18n("Ragnar Thomsen"),
                        i18n("Maintainer"),
                        QStringLiteral("rthomsen6@gmail.com"));

    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("preferences-system-services")));

    QCommandLineParser parser;
    parser.setApplicationDescription(aboutData.shortDescription());
    parser.addHelpOption();
    parser.addVersionOption();

    aboutData.setupCommandLine(&parser);

    KAboutData::setApplicationData(aboutData);

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

    MainWindow *window = new MainWindow;
    window->show();

    qDebug() << "Entering application loop";
    return application.exec();
}
