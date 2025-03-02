// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.systemdgenie

TablePage {
    id: root

    Kirigami.ColumnView.fillWidth: true

    delegate: TableDelegate {
        id: delegate

        required property int index
        required property string displayName
        required property string iconName
        required selected
        required current

        text: displayName
        icon.name: iconName
    }

    Kirigami.PlaceholderMessage {
        parent: root.tableView
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: i18nc("@info:placeholder", "Loading")
        visible: root.tableView.rows === 0
    }
}

