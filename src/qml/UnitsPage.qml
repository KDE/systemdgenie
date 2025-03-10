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

    readonly property UnitInterface unitObject: currentRow === -1 ? null : unitModel.unitObject(currentRow)
    property alias unitModel: sortFilter.sourceModel

    Kirigami.ColumnView.fillWidth: true

    model: SortFilterUnitModel {
        id: sortFilter

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
            displayComponent: Controls.ComboBox {
                model: [
                    i18nc("@item:inlistbox", "All"),
                    i18nc("@item:inlistbox", "Services"),
                    i18nc("@item:inlistbox", "Automounts"),
                    i18nc("@item:inlistbox", "Devices"),
                    i18nc("@item:inlistbox", "Mounts"),
                    i18nc("@item:inlistbox", "Paths"),
                    i18nc("@item:inlistbox", "Scopes"),
                    i18nc("@item:inlistbox", "Slices"),
                    i18nc("@item:inlistbox", "Sockets"),
                    i18nc("@item:inlistbox", "Swaps"),
                    i18nc("@item:inlistbox", "Targets"),
                    i18nc("@item:inlistbox", "Timers")
                ]

                onActivated: (index) => {
                    const data = ['', '.service', '.automount', '.device', '.mount', '.path', '.scope', '.slice', '.socket', '.swap', '.target', '.timer'];
                    sortFilter.addFilterRegExp(SortFilterUnitModel.UnitType, `(${data[index]})$`);
                    sortFilter.invalidate();
                }
            }
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
        spacing: Kirigami.Units.smallSpacing

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

    footer: Controls.ToolBar {
        leftPadding: Kirigami.Units.smallSpacing
        rightPadding: Kirigami.Units.smallSpacing
        topPadding: Kirigami.Units.smallSpacing
        bottomPadding: Kirigami.Units.smallSpacing
        background.implicitHeight: 0
        contentItem: Controls.Label {
            text: {
                const units = i18ncp("First part of 'Total: %1, %2, %3'", "1 unit", "%1 units", unitModel.count);
                const active = i18ncp("Second part of 'Total: %1, %2, %3'", "1 active", "%1 active", unitModel.activeUnitsCount);
                const displayed = i18ncp("Third part of 'Total: %1, %2, %3'", "1 displayed", "%1 displayed", root.tableView.count);
                return i18nc("%1 is '%1 units' and %2 is '%2 active' and %3 is '%3 displayed'", "Total: %1, %2, %3", units, active, displayed);
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
        required current

        text: displayName
        contextMenu: menu
        icon.name: iconName
        contentItem: Delegates.DefaultContentItem {
            itemDelegate: delegate
            labelItem.color: delegate.textColor
        }

        onClicked: root.Controls.ApplicationWindow.window.pageStack.push(Qt.resolvedUrl("./UnitPage.qml"), {
            unitObject: root.unitObject,
            model: root.unitModel,
            unitFile: root.unitModel.unitFile(sortFilter.mapToSource(sortFilter.index(index, 0)).row),
            unitFileStatus: root.unitModel.unitFileStatus(sortFilter.mapToSource(sortFilter.index(index, 0)).row),
        })
    }

    Components.ConvergentContextMenu {
        id: menu

        Controls.Action {
            id: unitStart
            icon.name: 'media-playback-start-symbolic'
            text: i18nc("@action", "Start Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'StartUnit')
            enabled: root.unitObject?.CanStart ?? root.currentRow >= 0
        }

        Controls.Action {
            id: unitStop
            icon.name: 'media-playback-stop-symbolic'
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
            icon.name: 'password-show-on-symbolic'
            text: i18nc("@action", "Unmask Unit")
            onTriggered: unitModel.executeUnitAction(root.currentRow, 'UnmaskUnitFiles')
        }

        Kirigami.Action {
            separator: true
        }

        Controls.Action {
            id: editUnit
            readonly property string unitFile: root.currentRow === -1 ? '' : unitModel.data(unitModel.index(root.currentRow, 0), UnitModel.UnitPathRole)
            icon.name: 'document-edit-symbolic'
            text: i18nc("@action", "Edit Unit File…")
            enabled: unitFile.length > 0
            onTriggered: Editor.openEditor(unitFile, Controls.ApplicationWindow.window)
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
