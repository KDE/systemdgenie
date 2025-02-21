// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

Kirigami.ApplicationWindow {
    id: root

    readonly property bool wideMode: root.width >= Kirigami.Units.gridUnit * 50

    minimumWidth: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 22
    minimumHeight: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit * 20

    pageStack {
        defaultColumnWidth: root.width

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

        initialPage: Kirigami.Page {
            Kirigami.InlineMessage {
                anchors.centerIn: parent
                text: i18nc("@title", "Welcome")
            }
        }
    }

    Kirigami.Action {
        id: systemUnitsAction

        text: i18nc("@action:button", "System Units")
    }

    Kirigami.Action {
        id: userUnitsAction

        text: i18nc("@action:button", "User Units")
    }

    Kirigami.Action {
        id: configFilesAction

        text: i18nc("@action:button", "Config Files")
        onTriggered: root.pageStack.replace(Qt.resolvedUrl('./ConfigFilesPage.qml'));
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
                Layout.bottomMargin: Kirigami.Units.smallSpacing / 2

                leftPadding: 3
                rightPadding: 3
                topPadding: 3
                bottomPadding: 3

                visible: !drawer.shouldCollapse
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

                        model: [systemUnitsAction, userUnitsAction, configFilesAction]

                        delegate: Delegates.RoundedItemDelegate {
                            required property var modelData

                            Controls.ButtonGroup.group: pageButtonGroup
                            padding: Kirigami.Units.largeSpacing
                            Layout.fillWidth: true
                            activeFocusOnTab: true

                            action: modelData
                            visible: modelData.visible
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
