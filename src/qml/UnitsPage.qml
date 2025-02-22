// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import org.kde.systemdgenie
import org.kde.kirigami as Kirigami

TablePage {
    id: root

    property int type

    title: type === UnitsPage.System ? i18nc("@title", "System Units") : i18nc("@title", "User Units")

    enum Type {
        System,
        User
    }

    model: type === UnitsPage.System ? Controller.systemUnitModel : Controller.userUnitModel
    delegate: TableDelegate {
        id: delegate

        required property int index
        required property string name
        required selected

        text: name
    }

    Kirigami.PlaceholderMessage {
        parent: root.tableView
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: i18nc("@info:placeholder", "Loading")
        visible: root.tableView.rows === 0
    }
}
