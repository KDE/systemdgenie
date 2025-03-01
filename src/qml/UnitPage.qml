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

    implicitWidth: Kirigami.Units.gridUnit * 20

    title: unitObject?.Id

    Kirigami.ColumnView.fillWidth: false

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit
        FormCard.FormTextDelegate {
            id: descriptionDelegate
            text: i18nc("@label", "Description:")
            description: unitObject?.Description ?? ''
            visible: description.length > 0
        }

        FormCard.FormDelegateSeparator {
            visible: descriptionDelegate.text.length > 0
        }

        FormCard.FormTextDelegate {
            text: i18nc("@label", "Unit file:")
            description: unitObject?.FragmentPath ?? root.unitFile
            visible: description.length > 0
            // UnitFileState
        }
    }
}
