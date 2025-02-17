/*******************************************************************************
 * Copyright (C) 2016 Ragnar Thomsen <rthomsen6@gmail.com>                     *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the Free  *
 * Software Foundation, either version 2 of the License, or (at your option)   *
 * any later version.                                                          *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
 * more details.                                                               *
 *                                                                             *
 * You should have received a copy of the GNU General Public License along     *
 * with this program. If not, see <http://www.gnu.org/licenses/>.              *
 *******************************************************************************/

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

    KAboutData::setApplicationData(aboutData);

    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop")));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    // Do the command line parsing.
    parser.process(application);

    // Handle standard options.
    aboutData.processCommandLine(&parser);

    MainWindow *window = new MainWindow;
    window->show();

    qDebug() << "Entering application loop";
    return application.exec();
}
