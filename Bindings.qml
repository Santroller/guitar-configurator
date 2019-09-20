import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Universal 2.13
import QtQuick.Layouts 1.13
import net.tangentmc 1.0

Page {
    id: page
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
            property var bindings: {
                "images/uno.png": {14: "A0", 15: "A1", 16: "A2", 17: "A3", 255: "Disabled"},
                "images/micro.png": {0:"RX0", 1:"RX1", 18: "A0", 19: "A1", 20:"A2", 21: "A3", 255: "Disabled"},
                "images/leonardo.png": {14: "A0", 15: "A1", 16: "A2", 17: "A3", 255: "Disabled"},
            }
            property var current: bindings[scanner.selected.boardImage]
            property var guitarLabels : {
                "up": "Strum Up",
                "down": "Strum Down",
                "left": "DPad Left",
                "right": "DPad Right",
                "start": "Start Button",
                "back": "Select Button",
                "home": "Home Button",
                "a": "Green Fret",
                "b": "Red Fret",
                "y": "Yellow Fret",
                "x": "Blue Fret",
                "LB": "Orange Fret",
                "l_x": "Joystick X Axis",
                "l_y": "Joystick Y Axis",
                "r_x": "Whammy",
                "r_y": "Tilt Axis",
            }

            property var defLabels: {
                "up": "DPad Up",
                "down": "DPad Down",
                "left": "DPad Left",
                "right": "DPad Right",
                "start": "Start Button",
                "back": "Back Button",
                "left_stick": "Left Stick Click",
                "right_stick": "Right Stick Click",
                "LB": "Left Bumper",
                "RB": "Right Bumper",
                "home": "Home Button",
                "a": "A Button",
                "b": "B Button",
                "x": "X Button",
                "y": "Y Button",
                "lt": "Left Shoulder Axis",
                "rt": "Right Shoulder Axis",
                "l_x": "Left Joystick X Axis",
                "l_y": "Left Joystick Y Axis",
                "r_x": "Right Joystick X Axis",
                "r_y": "Right Joystick Y Axis",

            }
            property var labels: scanner.selected.isGuitar()?guitarLabels:defLabels;
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
                model: Object.keys(gl.labels).length
                Label {
                    Layout.row: index+1
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    property var key: Object.keys(gl.labels)[index]
                    id: label
                    text: gl.labels[key]
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                }
            }
            Repeater {
                model: Object.keys(gl.labels).length
                Button {
                    Layout.row: index+1
                    Layout.column: 1
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    property var key: Object.keys(gl.labels)[index]
                    id: bt
                    text: gl.current[scanner.selected.pins[key]] || scanner.selected.pins[key]
                    onClicked: {
                        scanner.selected.currentPin = key;
                        mainStack.push("PinSelect.qml");
                    }
                }
            }
            Repeater {
                model: Object.keys(gl.labels).length
                Switch {
                    Layout.row: index+1
                    Layout.column: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    property var key: Object.keys(gl.labels)[index]
                    enabled: scanner.selected.pin_inverts.hasOwnProperty(key)
                    visible: scanner.selected.pin_inverts.hasOwnProperty(key)
                    checked: !!scanner.selected.pin_inverts[key]
                    onCheckedChanged: {
                        var pins = scanner.selected.pin_inverts;
                        pins[key] = checked;
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
