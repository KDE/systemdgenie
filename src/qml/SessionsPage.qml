// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import org.kde.systemdgenie

TablePage {
    id: root

    model: SessionModel {}
    delegate: TableDelegate {
        id: delegate

        required property int index
        required property string displayName
        required selected

        text: displayName
    }
}