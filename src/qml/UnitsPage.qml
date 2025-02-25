// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.systemdgenie

TablePage {
    id: root

    property int type

    title: type === UnitsPage.System ? i18nc("@title", "System Units") : i18nc("@title", "User Units")

    enum Type {
        System,
        User
    }

    model: SortFilterUnitModel {
        id: sortFilter

        sourceModel: root.type === UnitsPage.System ? Controller.systemUnitModel : Controller.userUnitModel

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
            displayComponent: Controls.Switch {
                checkable: true
                text: i18nc("@option:check", "Hide inactive")
                onCheckedChanged: checkInactiveUnits.checked = checked;
            }
            onCheckedChanged: sortFilter.updateFilter();
        },
        Kirigami.Action {
            id: checkUnloadedUnits
            checkable: true
            displayComponent: Controls.Switch {
                checkable: true
                text: i18nc("@option:check", "Hide unloaded")
                enabled: !checkInactiveUnits.checked
                onCheckedChanged: checkUnloadedUnits.checked = checked;
            }
            onCheckedChanged: sortFilter.updateFilter();
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

    Component.onCompleted: if (type === UnitsPage.System) {
        Controller.slotRefreshSystemUnitsList();
    } else {
        Controller.slotRefreshUserUnitsList();
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
