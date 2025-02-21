// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QAbstractTableModel>

class ConfigFileModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ExtraRoles {
        FileRole = Qt::DisplayRole,
        ModifiedRole = Qt::UserRole + 1,
        DescriptionRole,
    };

    enum Columns {
        FileColumn = 0,
        ModifiedColumn,
        DescriptionColumn,
    };

    explicit ConfigFileModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void openManPage(int index);

Q_SIGNALS:
    void errorOccurred(const QString &error);

private:
    struct ConfigFile {
        QString filePath;
        QString manPage;
        QString description;
    };

    void slotFileChanged(const QString &path);

    std::vector<ConfigFile> m_configFiles;
};
