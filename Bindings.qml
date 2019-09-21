import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Universal 2.13
import QtQuick.Layouts 1.13
import net.tangentmc 1.0
import "pins.js" as PinInfo

Page {
    id: page

    Dialog {
        function loadPins(value) {
            var pins = scanner.selected.pins;
            pins[scanner.selected.currentPin] = value;
            scanner.selected.pins = pins;
            scanner.selected.currentPin = "";
            pinDialog.accept();
        }
        id: pinDialog
        title: "Select a Pin for: "+gl.labels[scanner.selected.currentPin]
        visible: scanner.selected.currentPin
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        onAccepted: scanner.selected.currentPin = "";
        onRejected: scanner.selected.currentPin = "";
        modal: true
        ColumnLayout {
            anchors.fill: parent
            Image {
                property var current: scanner.selected.pins[scanner.selected.currentPin]
                property var scaleX: 1 / sourceSize.width * paintedWidth
                property var scaleY: 1 / sourceSize.height * paintedHeight
                property var startX: (width - paintedWidth) / 2
                property var startY: (height - paintedHeight) / 2
                property var selected: PinInfo.pinLocations[scanner.selected.boardImage];
                property var r: selected.r * scaleX
                property var pins: selected.pins
                id: boardImage
                Layout.alignment: Qt.AlignHCenter
                source: scanner.selected.boardImage
                fillMode: Image.PreserveAspectFit
                Layout.maximumHeight: applicationWindow.height/3
                Layout.maximumWidth: applicationWindow.width/3
                Repeater {
                    model: boardImage.pins.length
                    Rectangle {
                        width: boardImage.r; height: boardImage.r
                        x: boardImage.startX + boardImage.pins[index].x * boardImage.scaleX
                        y: boardImage.startY + boardImage.pins[index].y * boardImage.scaleY
                        radius: boardImage.r * 0.5
                        border.width: 1
                        color: boardImage.current === boardImage.pins[index].id ? "green" : mouseArea.containsMouse ? "red":"yellow"
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: boardImage.current = boardImage.pins[index].id
                        }
                    }
                }
            }
        }
        footer: RowLayout {
            Button {
                text: qsTr("Set Pin Binding")
                Layout.fillWidth: true
                onClicked: pinDialog.loadPins(c.current)
            }

            Button {
                text: qsTr("Disable Pin Binding")
                Layout.fillWidth: true
                onClicked: pinDialog.loadPins(0xFF)
            }

            Button {
                text: qsTr("Cancel")
                Layout.fillWidth: true
                onClicked: pinDialog.reject()
            }
        }
    }
    ColumnLayout {
        id: column
        anchors.fill: parent
        Label {
            id: title
            text: qsTr("Arduino Bindings")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 30
            fontSizeMode: Text.FixedSize
        }
        Image {
            id: image
            Layout.alignment: Qt.AlignHCenter
            source: scanner.selected.image
            fillMode: Image.PreserveAspectFit
            Layout.maximumHeight: applicationWindow.height/3
            Layout.maximumWidth: applicationWindow.width/3
        }
        Label {
            id: dev
            text: qsTr("Connected Device: ")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.FixedSize
        }
        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Label {
                id: status
                text: scanner.selected.description
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                fontSizeMode: Text.FixedSize
            }
            Button {
                id: refreshBt
                text: qsTr("Refresh")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: programmer.getStatusDescription()
            }
        }

        GridLayout {
            id: gl
            columns: 3
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            property var current: PinInfo.bindings[scanner.selected.boardImage]
            property var labels: PinInfo.getLabels(scanner.selected.isGuitar());
            Label {
                text: "Actions"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Label {
                text: "Pin Binding"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Label {
                text: "Invert Axis"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Repeater {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Object.values(gl.labels)
                Label {
                    Layout.row: index+1
                    Layout.column: 0
                    Layout.preferredWidth: gl.parent.width/3
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    id: label
                    text: modelData
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                }
            }
            Repeater {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Object.keys(gl.labels)
                Button {
                    Layout.row: index+1
                    Layout.column: 1
                    Layout.preferredWidth: gl.parent.width/3
                    Layout.fillHeight: true
                    id: bt
                    text: gl.current[scanner.selected.pins[modelData]] || scanner.selected.pins[modelData]
                    onClicked: scanner.selected.currentPin = modelData;
                }
            }
            Repeater {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Object.keys(gl.labels)
                Switch {
                    Layout.row: index+1
                    Layout.column: 2
                    Layout.preferredWidth: gl.parent.width/3
                    Layout.fillHeight: true
                    enabled: scanner.selected.pin_inverts.hasOwnProperty(modelData)
                    visible: enabled
                    checked: !!scanner.selected.pin_inverts[modelData]
                    onCheckedChanged: {
                        var pins = scanner.selected.pin_inverts;
                        pins[modelData] = checked;
                        scanner.selected.pin_inverts = pins;
                    }
                }
            }

        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Button {
                id: configureContinue
                text: qsTr("Save Bindings")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    mainStack.pop();
                }
            }

            Button {
                id: cancel
                text: qsTr("Cancel")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    mainStack.pop();
                }
            }
        }


    }
}



















































































/*##^##
Designer {
    D{i:0;autoSize:true;height:1080;width:1920}D{i:2;anchors_height:400;anchors_y:232}
D{i:3;anchors_y:774}D{i:1;anchors_height:60;anchors_width:802;anchors_x:646;anchors_y:658}
}
##^##*/
