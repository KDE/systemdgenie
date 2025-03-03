// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.coreaddons as Core
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.components as Components
import org.kde.systemdgenie

Kirigami.ApplicationWindow {
    id: root

    readonly property bool wideMode: root.width >= Kirigami.Units.gridUnit * 50

    minimumWidth: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 22
    minimumHeight: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 20

    pageStack {
        globalToolBar {
            canContainHandles: true
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: if (root.pageStack.currentIndex > 0
                || root.pageStack.currentIndex > 0) {
                Kirigami.ApplicationHeaderStyle.ShowBackButton
            } else {
                0
            }
        }

        columnView.columnResizeMode: wideMode ? Kirigami.ColumnView.DynamicColumns : Kirigami.ColumnView.SingleColumn

        initialPage: Kirigami.Page {} // just so that it can be replaced
    }

    Component.onCompleted: systemUnitsAction.trigger();

    UnitModel {
        id: systemUnitModel
        type: UnitModel.SystemUnits
    }

    UnitModel {
        id: userUnitModel
        type: UnitModel.UserUnits
    }

    TimerModel {
        id: timerModel
        userModel: userUnitModel
        systemModel: systemUnitModel
    }

    Kirigami.Action {
        id: systemUnitsAction

        icon.name: 'emblem-system-symbolic'
        text: i18nc("@action:button", "System Units")
        onTriggered: {
            root.pageStack.clear();
            root.pageStack.push(Qt.resolvedUrl('./UnitsPage.qml'), {
                unitModel: systemUnitModel,
            });
        }
    }

    Kirigami.Action {
        id: userUnitsAction

        icon.name: 'user-symbolic'
        text: i18nc("@action:button", "User Units")
        onTriggered: {
            root.pageStack.clear();
            root.pageStack.push(Qt.resolvedUrl('./UnitsPage.qml'), {
                unitModel: userUnitModel,
            });
        }
    }

    Kirigami.Action {
        id: configFilesAction

        icon.name: 'configure-symbolic'
        text: i18nc("@action:button", "Config Files")
        onTriggered: {
            root.pageStack.clear();
            root.pageStack.push(Qt.resolvedUrl('./ConfigFilesPage.qml'));
        }
    }

    Kirigami.Action {
        id: sessionsAction

        icon.name: 'system-users-symbolic'
        text: i18nc("@action:button", "Sessions")
        onTriggered: {
            root.pageStack.clear();
            root.pageStack.push(Qt.resolvedUrl('./SessionsPage.qml'));
        }
    }

    Kirigami.Action {
        id: timersAction

        text: i18nc("@action:button", "Timers")
        icon.name: 'player-time-symbolic'
        onTriggered: {
            root.pageStack.clear();
            root.pageStack.push(Qt.resolvedUrl('./TimersPage.qml'), {
                model: timerModel,
            });
        }
    }

    globalDrawer: Kirigami.OverlayDrawer {
        id: drawer

        enabled: true
        edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
        modal: !root.wideMode || !enabled
        z: modal ? Math.round(position * 10000000) : 100
        width: Kirigami.Units.gridUnit * 14
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        handleVisible: modal && enabled
        onModalChanged: drawerOpen = !modal

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        Behavior on width {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        contentItem: ColumnLayout {
            spacing: 0

            Controls.ToolBar {
                Layout.fillWidth: true
                Layout.preferredHeight: pageStack.globalToolBar.preferredHeight

                leftPadding: Kirigami.Units.largeSpacing
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0

                visible: !drawer.shouldCollapse

                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Heading {
                        text: Core.AboutData.displayName
                    }

                    Controls.ToolButton {
                        icon.name: 'application-menu-symbolic'
                        onClicked: menu.popup()
                        checked: menu.opened

                        Components.ConvergentContextMenu {
                            id: menu

                            Controls.Action {
                                text: i18nc("@action:inmenu", "Reload systemd")
                                icon.name: 'applications-system-symbolic'
                                onTriggered: Controller.executeSystemDaemonAction("Reload")
                            }

                            Controls.Action {
                                text: i18nc("@action:inmenu", "Re-execute systemd")
                                icon.name: 'applications-system-symbolic'
                                onTriggered: Controller.executeSystemDaemonAction("Reexecute")
                            }

                            Controls.Action {
                                text: i18nc("@action:inmenu", "Reload user systemd")
                                icon.name: 'user-identity-symbolic'
                                onTriggered: Controller.executeUserDaemonAction("Reload")
                            }

                            Controls.Action {
                                text: i18nc("@action:inmenu", "Re-execute user systemd")
                                icon.name: 'user-identity-symbolic'
                                onTriggered: Controller.executeUserDaemonAction("Reexecute")
                            }

                            Controls.Action {
                                text: i18nc("@action:inmenu", "View Logs")
                                icon.name: "folder-log-symbolic"
                                onTriggered: Controller.viewLogs()
                                enabled: Controller.canViewLogs
                            }
                        }
                    }
                }
            }

            Controls.ScrollView {
                id: scrollView

                contentWidth: availableWidth
                topPadding: Math.round(Kirigami.Units.smallSpacing / 2)
                bottomPadding: Math.round(Kirigami.Units.smallSpacing / 2)

                Controls.ScrollBar.vertical.interactive: false

                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    spacing: 0

                    width: scrollView.contentWidth
                    height: Math.max(scrollView.availableHeight, implicitHeight)

                    Controls.ButtonGroup {
                        id: pageButtonGroup
                    }

                    Repeater {
                        id: actionsRepeater

                        model: [systemUnitsAction, userUnitsAction, configFilesAction, sessionsAction, timersAction]

                        delegate: Delegates.RoundedItemDelegate {
                            required property var modelData

                            Controls.ButtonGroup.group: pageButtonGroup
                            padding: Kirigami.Units.largeSpacing
                            Layout.fillWidth: true
                            activeFocusOnTab: true

                            action: modelData
                            visible: modelData.visible
                            checkable: true
                            onClicked: checked = true;
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
}
