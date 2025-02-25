// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQml.Models
import QtQuick.Layouts
import QtQuick.Controls as Controls
import QtQuick.Templates as T
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

T.ItemDelegate {
    id: root

    required property int row
    required property bool selected
    readonly property bool rowHovered: root.TableView.view.hoveredRow === row || hovered

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            implicitIndicatorWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding,
                             Kirigami.Units.gridUnit * 2)

    padding: Kirigami.Settings.tabletMode ? Kirigami.Units.largeSpacing : Kirigami.Units.mediumSpacing
    spacing: Kirigami.Settings.tabletMode ? Kirigami.Units.largeSpacing * 2 : Kirigami.Units.smallSpacing

    horizontalPadding: padding + Math.round(Kirigami.Units.smallSpacing / 2)
    leftPadding: horizontalPadding
    rightPadding: horizontalPadding

    verticalPadding: padding
    topPadding: verticalPadding
    bottomPadding: verticalPadding

    onClicked: {
        const selectionModel = root.TableView.view.selectionModel
        selectionModel.clear();
        selectionModel.select(root.TableView.view.model.index(delegate.row, 0), ItemSelectionModel.Select | ItemSelectionModel.Rows)

        console.log(root.TableView.view.currentRow, root.TableView.view.model.index(delegate.row, 0))
    }

    icon {
        width: if (contentItem instanceof Delegates.SubtitleContentItem) {
            return Kirigami.Units.iconSizes.large
        } else {
            return Kirigami.Settings.tabletMode ? Kirigami.Units.iconSizes.smallMedium : Kirigami.Units.iconSizes.sizeForLabels
        }

        height: if (contentItem instanceof Delegates.SubtitleContentItem) {
            return Kirigami.Units.iconSizes.large
        } else {
            return Kirigami.Settings.tabletMode ? Kirigami.Units.iconSizes.smallMedium : Kirigami.Units.iconSizes.sizeForLabels
        }
    }

    background: Rectangle {
        color: if (root.highlighted || root.selected || (root.down && !root.checked) || root.visualFocus) {
            const highlight = Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.highlightColor, 0.3);
            if (root.rowHovered) {
                return Kirigami.ColorUtils.tintWithAlpha(highlight, Kirigami.Theme.textColor, 0.10);
            } else if (highlight.valid) {
                return highlight;
            } else {
                return Kirigami.Theme.backgroundColor;
            }
        } else if (root.rowHovered) {
            return Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.10)
        } else {
            return root.row % 2 == 0 ? Kirigami.Theme.backgroundColor : Kirigami.Theme.alternateBackgroundColor
        }

        border {
            color: Kirigami.Theme.highlightColor
            width: root.visualFocus || root.activeFocus ? 1 : 0
        }
    }

    onHoveredChanged: if (hovered) {
        root.TableView.view.hoveredRow = delegate.row
    }

    hoverEnabled: true

    contentItem: Delegates.DefaultContentItem {
        itemDelegate: root
    }

    Accessible.role: Accessible.Cell
}
