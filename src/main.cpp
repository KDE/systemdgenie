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

    KAboutData aboutData(QStringLiteral("systemdgenie"),
                         i18n("SystemdGenie"),
                         QStringLiteral(SYSTEMDGENIE_VERSION_STRING),
                         i18n("Systemd management utility"),
                         KAboutLicense::GPL,
                         i18n("(c) 2016, Ragnar Thomsen"),
                         QString(),
                         QString()
                         );

    aboutData.setOrganizationDomain("kde.org");

    aboutData.addAuthor(i18n("Ragnar Thomsen"),
                        i18n("Maintainer"),
                        QStringLiteral("rthomsen6@gmail.com"));

    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop")));

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

    MainWindow *window = new MainWindow;
    window->show();

    qDebug() << "Entering application loop";
    return application.exec();
}
