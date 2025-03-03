// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.systemdgenie

FormCard.FormCardPage {
    id: root

    required property UnitInterface unitObject
    required property string unitFile
    required property string unitFileStatus
    required property UnitModel model

    implicitWidth: Kirigami.Units.gridUnit * 20

    title: unitObject?.Id

    Kirigami.ColumnView.fillWidth: false

    actions: Kirigami.Action {
        text: i18nc("@action:intoolbar", "Close")
        icon.name: 'dialog-close-symbolic'
        displayHint: Kirigami.DisplayHint.IconOnly

        onTriggered: root.Controls.ApplicationWindow.window.pageStack.pop()
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Information")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            id: descriptionDelegate
            text: i18nc("@label", "Description:")
            description: unitObject?.Description ?? ''
            visible: description.length > 0
        }

        FormCard.FormDelegateSeparator {
            visible: descriptionDelegate.visible
        }

        FormCard.FormTextDelegate {
            id: unitFileDelegate
            text: i18nc("@label", "Unit file:")
            description: unitObject?.FragmentPath ?? root.unitFile
            visible: description.length > 0
        }

        FormCard.FormDelegateSeparator {
            visible: unitFileDelegate.visible
        }

        FormCard.FormTextDelegate {
            id: sourcePathDelegate
            text: i18nc("@label", "Source path:")
            description: unitObject?.SourcePath ?? ''
            visible: description.length > 0
        }

        FormCard.FormDelegateSeparator {
            visible: sourcePathDelegate.visible
        }

        FormCard.FormTextDelegate {
            readonly property int enterTimestamp: unitObject ? unitObject.ActiveEnterTimestamp : 0
            text: i18nc("@label", "Activated:")
            description: enterTimestamp === 0 ? i18nc("invalid", "n/a") : Date(enterTimestamp).toLocaleString()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            readonly property int timestamp: unitObject ? unitObject.InactiveEnterTimestamp : 0
            text: i18nc("@label", "Desactivated:")
            description: timestamp === 0 ? i18nc("invalid", "n/a") : Date(timestamp).toLocaleString()
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Last log entries")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            readonly property var entries: unitObject ? root.model.lastJrnlEntries(unitObject.Id) : [];

            text: ''
            description: entries.join('<br />');
        }
    }
}
