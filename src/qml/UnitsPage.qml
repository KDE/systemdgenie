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
        if (tableView.currentRow === -1) {
            return -1;
        }
        return sortFilter.mapToSource(tableView.selectionModel.currentIndex).row;
    }

    readonly property UnitInterface unitObject: {
        console.log("currentRow", currentRow)
        const unit = currentRow === -1 ? null : unitModel.unitObject(currentRow);
        console.log(unit)
        for (let i in unit) {
            console.log(i, unit[i]);
        }
        return unit;
    }
    property alias type: unitModel.type

    tableView.onCurrentRowChanged: console.log("tableView.currentRow", tableView.currentRow)

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
        required current

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
                delegate.clicked();
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
            enabled: (root.unitObject?.CanStart && root.unitObject.ActiveState !== 'inactive' && root.unitObject.ActiveState !== 'failed') ?? root.currentRow >= 0
        }

        Controls.Action {
            id: unitStop
            icon.name: 'media-playback-start-symbolic'
            text: i18nc("@action", "Stop Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'StopUnit')
            enabled: (root.unitObject?.CanStop && root.unitObject.ActiveState !== 'inactive' && root.unitObject.ActiveState !== 'failed') ?? false
        }

        Controls.Action {
            id: unitRestart
            icon.name: 'start-over-symbolic'
            text: i18nc("@action", "Restart Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'RestartUnit')
            enabled: (root.unitObject?.CanRestart && root.unitObject.ActiveState !== 'inactive' && root.unitObject.ActiveState !== 'failed') ?? false
        }

        Controls.Action {
            id: unitReload
            icon.name: 'view-refresh-symbolic'
            text: i18nc("@action", "Reload Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'ReloadUnit')
            enabled: (root.unitObject?.CanReload && root.unitObject.ActiveState !== 'inactive' && root.unitObject.ActiveState !== 'failed') ?? false
        }

        Kirigami.Action {
            separator: true
        }

        Kirigami.Action {
            id: unitEnable
            icon.name: 'archive-insert-symbolic'
            text: i18nc("@action", "Enable Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'EnableUnitFiles')
            enabled: root.unitObject?.UnitFileState === 'disabled' ?? false
            visible: root.unitObject?.UnitFileState.length > 0 ?? false
        }

        Kirigami.Action {
            id: unitDisable
            icon.name: 'document-close-symbolic'
            text: i18nc("@action", "Disable Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'DisableUnitFiles')
            enabled: root.unitObject?.UnitFileState === 'enabled' ?? false
            visible: root.unitObject?.UnitFileState.length > 0 ?? 0
        }

        Kirigami.Action {
            separator: true
            visible: unitDisable.visible
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
