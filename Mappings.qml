import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import net.tangentmc 1.0
import "defines.js" as Defines

Page {
    id: page
    title: "Mapping Configuration"
    ColumnLayout {
        id: column
        anchors.fill: parent
        Label {
            id: title
            text: qsTr("Mapping Configuration")
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
            source: Defines.getBoardImage(scanner.selected.type)
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
                onClicked: scanner.selected.readDescription()
            }
        }

        GridLayout {
            id: gl
            columns: 2
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Label {
                text: "Setting"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Label {
                text: "Value"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Label {
                text: "Map Joystick to DPad"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Layout.preferredWidth: gl.parent.width/3
                checked: scanner.selected.mapJoystick
                onCheckedChanged: {
                    scanner.selected.mapJoystick = checked
                }
            }
            Label {
                text: "Map Start + Select to PS Button / Home / XBox Dashboard"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Layout.preferredWidth: gl.parent.width/3
                checked: scanner.selected.mapStartSelectHome
                onCheckedChanged: {
                    scanner.selected.mapStartSelectHome = checked
                }
            }
            Label {
                text: "Trigger Threshold"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Slider {
                id: slider
                Layout.preferredWidth: gl.parent.width/3
                to: 128
                from: 0
                live: false
                value: scanner.selected.triggerThreshold
                onValueChanged: scanner.selected.triggerThreshold = slider.value
                background: Rectangle {
                    y: 15
                    implicitWidth: 200
                    implicitHeight: 4
                    height: implicitHeight
                    radius: 2
                    color: "#bdbebf"
                }
            }
            Label {
                text: "Joystick Threshold"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Slider {
                id: slider2
                Layout.preferredWidth: gl.parent.width/3
                to: 128
                from: 0
                live: false
                value: scanner.selected.joyThreshold
                onValueChanged: scanner.selected.joyThreshold = slider2.value
                background: Rectangle {
                    y: 15
                    implicitWidth: 200
                    implicitHeight: 4
                    height: implicitHeight
                    radius: 2
                    color: "#bdbebf"
                }
            }
            Label {
                text: "Poll Rate"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            SpinBox {
                Layout.preferredWidth: gl.parent.width/3
                id: spinbox
                from: 0
                to: 4
                textFromValue: function(value, locale) { return Number(1 << value).toLocaleString(locale, 'f', 0); }
                value: Math.log2(scanner.selected.pollRate);
                onValueChanged: scanner.selected.pollRate = 1 << value

            }


        }

        Button {
            id: configureContinue
            text: qsTr("Back to Configuration")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                mainStack.pop();
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
