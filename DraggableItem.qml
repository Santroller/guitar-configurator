/*
 * listviewdragitem
 *
 * An example of reordering items in a ListView via drag'n'drop.
 *
 * Author: Aurélien Gâteau
 * License: BSD
 */
import QtQuick 2.0
import QtQuick.Controls 2.10

Item {
    id: root

    default property Item contentItem
    property string label
    // This item will become the parent of the dragged item during the drag operation
    property Item draggedItemParent

    signal moveItemRequested(int from, int to)

    // Size of the area at the left and bottom of the list where drag-scrolling happens
    property int scrollEdgeSize: 6

    // Internal: set to -1 when drag-scrolling up and 1 when drag-scrolling down
    property int _scrollingDirection: 0

    // Internal: shortcut to access the attached ListView from everywhere. Shorter than root.ListView.view
    property ListView _listView: ListView.view

    height: contentItem.height
    width: leftPlaceholder.width + wrapperParent.width + rightPlaceholder.width

    // Make contentItem a child of contentItemWrapper
    onContentItemChanged: {
        contentItem.parent = contentItemWrapper;
    }

    Rectangle {
        id: leftPlaceholder
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        width: 0
        color: "lightgrey"
    }

    Item {
        id: wrapperParent
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: leftPlaceholder.right
        }
        width: contentItem.width

        Rectangle {
            id: contentItemWrapper
            anchors.fill: parent
            Drag.active: dragArea.drag.active
            Drag.hotSpot {
                y: contentItem.height / 2
                x: contentItem.width / 2
            }

            ToolTip.visible: dragArea.containsMouse
            ToolTip.text: label
            MouseArea {
                id: dragArea
                anchors.fill: parent
                drag.target: parent
                // Disable smoothed so that the Item pixel from where we started the drag remains under the mouse cursor
                drag.smoothed: false
                hoverEnabled: true

                onReleased: {
                    if (drag.active) {
                        emitMoveItemRequested();
                    }
                }
            }
        }
    }

    Rectangle {
        id: rightPlaceholder
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: wrapperParent.right
        }
        width: 0
        color: "lightgrey"
    }

    SmoothedAnimation {
        id: upAnimation
        target: _listView
        property: "contentX"
        to: 0
        running: _scrollingDirection == -1
    }

    SmoothedAnimation {
        id: downAnimation
        target: _listView
        property: "contentX"
        to: _listView.contentwidth - _listView.width
        running: _scrollingDirection == 1
    }

    Loader {
        id: leftDropAreaLoader
        active: model.index === 0
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: wrapperParent.horizontalCenter
        }
        width: contentItem.width
        sourceComponent: Component {
            DropArea {
                property int dropIndex: 0
            }
        }
    }

    DropArea {
        id: rightDropArea
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: wrapperParent.horizontalCenter
        }
        property bool isLast: model.index === _listView.count - 1
        width: contentItem.width

        property int dropIndex: model.index + 1
    }

    states: [
        State {
            when: dragArea.drag.active
            name: "dragging"

            ParentChange {
                target: contentItemWrapper
                parent: draggedItemParent
            }
            PropertyChanges {
                target: contentItemWrapper
                opacity: 0.9
                anchors.fill: undefined
                height: contentItem.height
                width: contentItem.width
            }
            PropertyChanges {
                target: wrapperParent
                width: 0
            }
            PropertyChanges {
                target: root
                _scrollingDirection: {
                    var xCoord = _listView.mapFromItem(dragArea, dragArea.mouseX, 0).x;
                    if (xCoord < scrollEdgeSize) {
                        -1;
                    } else if (xCoord > _listView.width - scrollEdgeSize) {
                        1;
                    } else {
                        0;
                    }
                }
            }
        },
        State {
            when: rightDropArea.containsDrag
            name: "droppingBelow"
            PropertyChanges {
                target: rightPlaceholder
                width: contentItem.width
            }
            PropertyChanges {
                target: rightDropArea
                width: contentItem.width * 2
            }
        },
        State {
            when: leftDropAreaLoader.item && leftDropAreaLoader.item.containsDrag
            name: "droppingAbove"
            PropertyChanges {
                target: leftPlaceholder
                width: contentItem.width
            }
            PropertyChanges {
                target: leftDropAreaLoader
                width: contentItem.width * 2
            }
        }
    ]

    function emitMoveItemRequested() {
        var dropArea = contentItemWrapper.Drag.target;
        if (!dropArea) {
            return;
        }
        var dropIndex = dropArea.dropIndex;

        // If the target item is below us, then decrement dropIndex because the target item is going to move up when
        // our item leaves its place
        if (model.index < dropIndex) {
            dropIndex--;
        }
        if (model.index === dropIndex) {
            return;
        }
        root.moveItemRequested(model.index, dropIndex);

        // Scroll the ListView to ensure the dropped item is visible. This is required when dropping an item after the
        // last item of the view. Delay the scroll using a Timer because we have to wait until the view has moved the
        // item before we can scroll to it.
        makeDroppedItemVisibleTimer.start();
    }

    Timer {
        id: makeDroppedItemVisibleTimer
        interval: 0
        onTriggered: {
            _listView.positionViewAtIndex(model.index, ListView.Contain);
        }
    }
}
