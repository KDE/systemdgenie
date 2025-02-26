// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.systemdgenie

TablePage {
    id: root

    readonly property int currentRow: {
        if (tableView.selectionModel.selectedIndexes.length === 0) {
            return -1;
        }
        return sortFilter.mapToSource(tableView.selectionModel.selectedIndexes[0]).row;
    }
    property alias type: unitModel.type

    model: SortFilterUnitModel {
        id: sortFilter

        sourceModel: UnitModel {
            id: unitModel
        }

        function updateFilter(): void {
            if (!checkInactiveUnits.checked) {
                if (!checkUnloadedUnits.checked) {
                    addFilterRegExp(SortFilterUnitModel.ActiveState, "");
                } else {
                    addFilterRegExp(SortFilterUnitModel.ActiveState, "active");
                }
            } else {
                addFilterRegExp(SortFilterUnitModel.ActiveState, "^(active)");
            }
            invalidate();
        }
    }

    actions: [
        Kirigami.Action {
            id: checkInactiveUnits
            checkable: true
            text: i18nc("@option:check", "Hide inactive")
            displayHint: Kirigami.DisplayHint.AlwaysHide
            onCheckedChanged: sortFilter.updateFilter();
        },
        Kirigami.Action {
            id: checkUnloadedUnits
            checkable: true
            text: i18nc("@option:check", "Hide unloaded")
            enabled: !checkInactiveUnits.checked
            onCheckedChanged: sortFilter.updateFilter();
            displayHint: Kirigami.DisplayHint.AlwaysHide
        },
        Kirigami.Action {
            displayComponent: Kirigami.SearchField {
                onAccepted: {
                    sortFilter.addFilterRegExp(SortFilterUnitModel.UnitName, text);
                    sortFilter.invalidate();
                }
            }
        }
    ]

    titleDelegate: RowLayout {
        Controls.ToolButton {
            text: i18nc("@action:intoolbar", "Start")
            action: unitStart
        }

        Controls.ToolButton {
            text: i18nc("@action:intoolbar", "Stop")
            action: unitStop
        }

        Controls.ToolButton {
            text: i18nc("@action:intoolbar", "Reload")
            action: unitReload
        }
    }

    delegate: TableDelegate {
        id: delegate

        required property int index
        required property string displayName
        required property string iconName
        required property color textColor
        required property var model
        required selected

        text: displayName
        icon.name: iconName
        contentItem: Delegates.DefaultContentItem {
            itemDelegate: delegate
            labelItem.color: delegate.textColor
        }

        Controls.ToolTip {
            text: hovered ? model.tooltip : '' // only request tooltip when needed
            delay: Kirigami.Units.toolTipDelay
            visible: hovered && text.length > 0
        }

        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: {
                menu.popup();
            }
        }
    }

    Components.ConvergentContextMenu {
        id: menu

        Controls.Action {
            id: unitStart
            icon.name: 'media-playback-start-symbolic'
            text: i18nc("@action", "Start Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'StartUnit')
        }

        Controls.Action {
            id: unitStop
            icon.name: 'media-playback-start-symbolic'
            text: i18nc("@action", "Stop Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'StopUnit')
        }

        Controls.Action {
            id: unitRestart
            icon.name: 'start-over-symbolic'
            text: i18nc("@action", "Restart Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'RestartUnit')
        }

        Controls.Action {
            id: unitReload
            icon.name: 'view-refresh-symbolic'
            text: i18nc("@action", "Reload Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'ReloadUnit')
        }

        Kirigami.Action {
            separator: true
        }

        Controls.Action {
            id: unitEnable
            icon.name: 'archive-insert-symbolic'
            text: i18nc("@action", "Enable Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'EnableUnitFiles')
        }

        Controls.Action {
            id: unitDisnable
            icon.name: 'document-close-symbolic'
            text: i18nc("@action", "Enable Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'DisableUnitFiles')
        }

        Kirigami.Action {
            separator: true
        }

        Controls.Action {
            id: unitUnmask
            icon.name: 'password-show-off-symbolic'
            text: i18nc("@action", "Mask Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'MaskUnitFiles')
        }

        Controls.Action {
            id: unitMask
            icon.name: 'password-show-symbolic'
            text: i18nc("@action", "Unmask Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'UnmaskUnitFiles')
        }
    }

    Kirigami.PlaceholderMessage {
        parent: root.tableView
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: i18nc("@info:placeholder", "Loading")
        visible: root.tableView.rows === 0
    }
}
