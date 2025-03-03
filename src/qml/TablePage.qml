// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQml.Models
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigami.platform as Platform
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.systemdgenie

Kirigami.Page {
    id: root

    required property var model
    required property Component delegate
    readonly property alias tableView: tableView

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    contentItem: Rectangle {
        // The background color will show through the cell
        // spacing, and therefore become the grid line color.
        color: Kirigami.Theme.backgroundColor

        Controls.HorizontalHeaderView {
            id: heading

            width: scrollView.width

            syncView: tableView
            clip: true
            textRole: "displayName"
        }

        Controls.ScrollView {
            id: scrollView

            anchors.fill: parent
            anchors.topMargin: heading.height

            TableView {
                id: tableView

                property int hoveredRow: -1

                model: root.model

                clip: true
                pixelAligned: true
                boundsBehavior: Flickable.StopAtBounds

                selectionBehavior: TableView.SelectRows
                selectionMode: TableView.ExtendedSelection

                selectionModel: ItemSelectionModel {
                    id: selectionModel
                }

                delegate: root.delegate

                columnWidthProvider: function(column) {
                    const w = explicitColumnWidth(column)
                    if (w >= 0) {
                        return Math.min(w, width / 2);
                    }
                    const implicit = implicitColumnWidth(column)
                    const headingImplicit = heading.implicitColumnWidth(column)
                    const max = Math.max(implicit, headingImplicit);
                    return Math.min(width / 2, max)
                }
            }
        }
    }
}
