// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
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
        Repeater {
            readonly property list<Controls.Action> actions: [
                Controls.Action {
                    id: unitStart
                    icon.name: 'media-playback-start-symbolic'
                    text: i18nc("@action", "Start")
                    onTriggered: unitModel.executeUnitAction(root.currentRow, 'StartUnit')
                },
                Controls.Action {
                    id: unitStop
                    icon.name: 'media-playback-start-symbolic'
                    text: i18nc("@action", "Stop")
                    onTriggered: unitModel.executeUnitAction(root.currentRow, 'StopUnit')
                },
                Controls.Action {
                    id: unitReload
                    icon.name: 'view-refresh-symbolic'
                    text: i18nc("@action", "Reload")
                    onTriggered: unitModel.executeUnitAction(root.currentRow, 'RestartUnit')
                }
            ]

            model: actions
            delegate: Controls.ToolButton {
                action: modelData
            }
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
    }

    Kirigami.PlaceholderMessage {
        parent: root.tableView
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: i18nc("@info:placeholder", "Loading")
        visible: root.tableView.rows === 0
    }
}
