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

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    property var columnWidths: []
    property real defaultColumnWidth: 0.1
    property real minimumColumnWidth: Kirigami.Units.gridUnit * 4

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
            textRole: "name"
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

                contentWidth: availableWidth

                columnWidthProvider: function(index) {
                    let column = index
                    const isLast = index === model.columnCount() - 1;

                    // FIXME Until Tableview correctly reverses its columns, see QTBUG-90547
                    if (LayoutMirroring.enabled) {
                        column = root.columnWidths.length - index - 1
                    }

                    // Resizing sets the explicit column width and has no other trigger. If
                    // we don't make use of that value we can't resize. So read the value,
                    // convert it to a fraction of total width and write it back to
                    // columnWidths, then clear the explicit column width again so that
                    // resizing updates the column width properly. This isn't the prettiest
                    // of solutions but at least makes things work the way we want.
                    let explicitWidth = explicitColumnWidth(index)
                    if (explicitWidth >= 0) {
                        let w = explicitWidth / width
                        root.columnWidths[column] = w
                        root.columnWidthsChanged()
                        clearColumnWidths()
                    }

                    let columnWidth = root.columnWidths[column]
                    return Math.max(Math.floor((columnWidth ?? root.defaultColumnWidth) * scrollView.innerWidth), root.minimumColumnWidth)
                }

                delegate: root.delegate
            }
        }
    }
}
