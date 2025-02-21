// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQml.Models
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigami.platform as Platform
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.systemdgenie

TablePage {
    id: root

    title: i18nc("@title", "Config Files")

    model: ConfigFileModel {}
    delegate: TableDelegate {
        id: delegate

        required property int index
        required property string name
        required selected

        text: name
    }
}
