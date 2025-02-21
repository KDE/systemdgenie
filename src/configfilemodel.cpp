// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "configfilemodel.h"

#include <QIcon>
#include <QFileInfo>
#include <QLocale>
#include <QFileSystemWatcher>

#include <KLocalizedString>
#include <KService>
#include <KIO/ApplicationLauncherJob>
#include <KIO/JobUiDelegateFactory>

using namespace Qt::StringLiterals;

ConfigFileModel::ConfigFileModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_configFiles.push_back(ConfigFile{QStringLiteral("/etc/systemd/coredump.conf"),
                                   QStringLiteral("coredump.conf"),
                                   i18n("Coredump generation and storage")});
    m_configFiles.push_back(ConfigFile{QStringLiteral("/etc/systemd/journal-upload.conf"),
                                   QStringLiteral("journal-upload.conf"),
                                   i18n("Send journal messages over network")});
    m_configFiles.push_back(ConfigFile{QStringLiteral("/etc/systemd/journald.conf"),
                                   QStringLiteral("journald.conf"),
                                   i18n("Journal manager settings")});
    m_configFiles.push_back(ConfigFile{QStringLiteral("/etc/systemd/logind.conf"),
                                   QStringLiteral("logind.conf"),
                                   i18n("Login manager configuration")});
    m_configFiles.push_back(ConfigFile{QStringLiteral("/etc/systemd/resolved.conf"),
                                   QStringLiteral("resolved.conf"),
                                   i18n("Network name resolution configuration")});
    m_configFiles.push_back(ConfigFile{QStringLiteral("/etc/systemd/system.conf"),
                                   QStringLiteral("systemd-system.conf"),
                                   i18n("Systemd daemon configuration")});
    m_configFiles.push_back(ConfigFile{QStringLiteral("/etc/systemd/timesyncd.conf"),
                                   QStringLiteral("timesyncd.conf"),
                                   i18n("Time synchronization settings")});
    m_configFiles.push_back(ConfigFile{QStringLiteral("/etc/systemd/user.conf"),
                                   QStringLiteral("systemd-system.conf"),
                                   i18n("Systemd user daemon configuration")});

    QFileSystemWatcher *m_fileWatcher = new QFileSystemWatcher;
    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &ConfigFileModel::slotFileChanged);

    for (const ConfigFile &f : std::as_const(m_configFiles)) {
        m_fileWatcher->addPath(f.filePath);
    }

    std::erase_if(m_configFiles, [](const ConfigFile &configFile) {
        return !QFileInfo::exists(configFile.filePath);
    });
}

int ConfigFileModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_configFiles.size();
}

int ConfigFileModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 3;
}

QVariant ConfigFileModel::data(const QModelIndex &index, int role) const
{
    const auto &configFile = m_configFiles[index.row()];

    switch (index.column()) {
    case FileColumn:
        switch (role) {
        case Qt::DisplayRole:
            return configFile.filePath;
        case Qt::DecorationRole:
            return QIcon::fromTheme(QStringLiteral("text-plain"));
        case ModifiedRole:
            return QLocale().toString(QFileInfo(configFile.filePath).lastModified(), QLocale::ShortFormat);
        case DescriptionRole:
            return configFile.description;
        default:
            return {};
        }
    case ModifiedColumn:
        switch (role) {
        case Qt::DisplayRole:
            return QLocale().toString(QFileInfo(configFile.filePath).lastModified(), QLocale::ShortFormat);
        default:
            return {};
        }
    case DescriptionColumn:
        switch (role) {
        case Qt::DisplayRole:
            return configFile.description;
        default:
            return {};
        }
    default:
        return {};
    }
}

QHash<int, QByteArray> ConfigFileModel::roleNames() const
{
    return {
        { FileRole, "file" },
        { ModifiedRole, "modified" },
        { DescriptionRole, "description" },
    };
}

QVariant ConfigFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (orientation) {
    case Qt::Horizontal:
        switch (section) {
        case FileColumn:
            return i18nc("@title:column", "File");
        case ModifiedColumn:
            return i18nc("@title:column", "Modified");
        case DescriptionColumn:
            return i18nc("@title:column", "Description");
        }
    default:
        return {};
    }
}

void ConfigFileModel::slotFileChanged(const QString &path)
{
    for (int i = 0, count = m_configFiles.size(); i < count; i++) {
        if (m_configFiles[i].filePath == path) {
            Q_EMIT dataChanged(index(i, ModifiedColumn), index(i, ModifiedColumn), {Qt::DisplayRole});
            Q_EMIT dataChanged(index(i, 0), index(i, 0), {ModifiedRole});
            return;
        }
    }
}

void ConfigFileModel::openManPage(int index)
{
    const QString &manPage = m_configFiles.at(index).manPage;

    auto khelpCenter = KService::serviceByDesktopName(u"org.kde.khelpcenter"_s);
    if (!khelpCenter) {
        Q_EMIT errorOccurred(i18n("KHelpCenter executable not found in path. Please install KHelpCenter to view man pages."));
        return;
    }

    auto job = new KIO::ApplicationLauncherJob(khelpCenter);
    job->setUrls({QUrl(u"man:/%1"_s.arg(manPage))});
    job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
    job->start();
}
