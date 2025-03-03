// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QWindow>
#include <qqmlregistration.h>

class Editor : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit Editor(QObject *parent = nullptr);

    Q_INVOKABLE void openEditor(const QString &file, QWindow *parentWindow);

Q_SIGNALS:
    void errorOccurred(const QString &error);
};
