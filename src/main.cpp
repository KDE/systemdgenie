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
#include <QIcon>
#include <QQmlApplicationEngine>
#include <qdbusmetatype.h>

#include "loginddbustypes.h"
#include "systemdgenie_version.h"
#include "systemdunit.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    KCrash::initialize();

    KLocalizedString::setApplicationDomain("systemdgenie");

    QGuiApplication::setDesktopFileName(QStringLiteral("org.kde.systemdgenie"));

    KAboutData aboutData(QStringLiteral("systemdgenie"),
                         i18n("systemdGenie"),
                         QStringLiteral(SYSTEMDGENIE_VERSION_STRING),
                         i18n("Manage systemd units, timers, sessions and config files"),
                         KAboutLicense::GPL,
                         i18nc("@about:credit", "(c) 2016 Ragnar Thomsen, %1 KDE Community", QString::number(QDate::currentDate().year())),
                         QString(),
                         QString());

    aboutData.setOrganizationDomain("kde.org");

    aboutData.addAuthor(i18n("Ragnar Thomsen"), i18n("Former Maintainer"), QStringLiteral("rthomsen6@gmail.com"));
    aboutData.addAuthor(i18n("Carl Schwan"), i18n("Maintainer"), QStringLiteral("carl@carlschwan.eu"));

    KAboutData::setApplicationData(aboutData);

    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("preferences-system-services")));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    // Do the command line parsing.
    parser.process(application);

    // Handle standard options.
    aboutData.processCommandLine(&parser);

    qDBusRegisterMetaType<SystemdUnit>();
    qDBusRegisterMetaType<QList<SystemdUnit>>();

    qDBusRegisterMetaType<SessionInfo>();
    qDBusRegisterMetaType<SessionInfoList>();
    qDBusRegisterMetaType<UserInfo>();
    qDBusRegisterMetaType<UserInfoList>();
    qDBusRegisterMetaType<NamedSeatPath>();
    qDBusRegisterMetaType<NamedSeatPathList>();
    qDBusRegisterMetaType<NamedUserPath>();
    qDBusRegisterMetaType<Inhibitor>();
    qDBusRegisterMetaType<InhibitorList>();
    qDBusRegisterMetaType<std::pair<QString, QString>>();
    qDBusRegisterMetaType<QList<std::pair<QString, QString>>>();

    qDBusRegisterMetaType<UnitFile>();
    qDBusRegisterMetaType<QList<UnitFile>>();

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);
    engine.loadFromModule("org.kde.systemdgenie", "Main");
    return application.exec();
}
