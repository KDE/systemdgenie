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

    model: SessionModel {
        id: sessionModel
    }
    delegate: TableDelegate {
        id: delegate

        required property int index
        required property string displayName
        required selected

        contextMenu: menu
        text: displayName
    }

    Components.ConvergentContextMenu {
        id: menu

        Controls.Action {
            id: sessionStart
            icon.name: 'media-playback-start-symbolic'
            text: i18nc("@action", "Activate Session")
            onTriggered: sessionModel.executeAction(root.currentRow, "Activate", root.Controls.ApplicationWindow.window);
            enabled: root.currentRow !== -1
        }

        Controls.Action {
            id: sessionStop
            icon.name: 'media-playback-stop-symbolic'
            text: i18nc("@action", "Terminate Session")
            onTriggered: sessionModel.executeAction(root.currentRow, "Terminate", root.Controls.ApplicationWindow.window);
            enabled: root.currentRow !== -1
        }

        Controls.Action {
            id: sessionLock
            icon.name: 'lock-symbolic'
            text: i18nc("@action", "Restart Session")
            onTriggered: sessionModel.executeAction(root.currentRow, "Lock", root.Controls.ApplicationWindow.window);
            enabled: root.currentRow !== -1
        }
    }
}