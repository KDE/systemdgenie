// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components
import org.kde.systemdgenie

TablePage {
    id: root

    model: ConfigFileModel {
        id: configFileModel
    }

    delegate: TableDelegate {
        id: delegate

        required property int index
        required property string displayName
        required property string iconName
        required selected

        text: displayName
        icon.name: iconName

        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: {
                const selectionModel = root.tableView.selectionModel
                selectionModel.clear();
                selectionModel.setCurrentIndex(root.tableView.model.index(delegate.row, 0), ItemSelectionModel.SelectCurrent | ItemSelectionModel.Rows)
                menu.popup();
            }
        }
    }

    titleDelegate: RowLayout {
        spacing: Kirigami.Units.smallSpacing

        Controls.ToolButton {
            text: i18nc("@action:intoolbar", "Open Configuration")
            action: openConf
        }

        Controls.ToolButton {
            text: i18nc("@action:intoolbar", "Open Manual")
            action: openManual
        }
    }

    Components.ConvergentContextMenu {
        id: menu

        Controls.Action {
            id: opemManual
            icon.name: "help-contents-symbolic"
            text: i18n("Open Man Page")
            onTriggered: configFileModel.openManPage(root.tableView.currentRow)
            enabled: root.tableView.currentRow !== -1
        }

        Controls.Action {
            id: openConf
            readonly property string confFile: root.tableView.currentRow === -1 ? '' : configFileModel.index(root.tableView.currentRow, ConfigFileModel.FileColumn).data()
            icon.name: 'document-edit-symbolic'
            text: i18nc("@action", "Open Configuration")
            enabled: confFile.length > 0
            onTriggered: Editor.openEditor(confFile, Controls.ApplicationWindow.window)
        }
    }
}
