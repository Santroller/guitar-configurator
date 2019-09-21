import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Universal 2.13
import QtQuick.Layouts 1.13
import net.tangentmc 1.0
import "keys.js" as KeyInfo

Page {
    id: page


    Dialog {
        function setPin(value) {
            var pins = scanner.selected.pins;
            pins[scanner.selected.currentKey] = value;
            scanner.selected.pins = pins;
            pinDialog.accept()
        }
        id: pinDialog
        title: "Binding: "+KeyInfo.labels[scanner.selected.currentKey]
        visible: scanner.selected.currentKey
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        modal: true
        onAccepted: scanner.selected.currentKey = "";
        onRejected: scanner.selected.currentKey = "";
        ColumnLayout {
            Label {
                text: qsTr("Press a key to assign it to " + KeyInfo.labels[scanner.selected.currentKey])
            }
            Label {
                text: qsTr("Current Key: " + KeyInfo.getKeyName(gl.currentValue))
            }
            RowLayout {
                Button {
                    text: qsTr("Save Key")
                    onClicked: pinDialog.setPin(gl.currentValue)
                }
                Button {
                    onClicked: pinDialog.setPin(0xFF)
                    text: qsTr("Disable Key")
                }
                Button {
                    onClicked: pinDialog.reject()
                    text: qsTr("Cancel")
                }
            }
        }

    }
    ColumnLayout {
        id: column
        anchors.fill: parent
        Label {
            id: title
            text: qsTr("Keyboard Bindings")
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
            columns: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            property var currentValue: ""

            Label {
                text: "Actions"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Label {
                text: "Key"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Repeater {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Object.values(KeyInfo.labels)
                Label {
                    Layout.row: index+1
                    Layout.column: 0
                    Layout.preferredWidth: gl.parent.width/2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.fillHeight: true
                    property var key: Object.values(KeyInfo.labels)
                    id: label
                    text: modelData
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                }
            }
            Repeater {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Object.keys(KeyInfo.labels)
                Button {
                    Layout.row: index+1
                    Layout.column: 1
                    Layout.preferredWidth: gl.parent.width/2
                    Layout.fillHeight: true
                    id: bt
                    text: KeyInfo.getKeyName(scanner.selected.pins[modelData])
                    onClicked: {
                        gl.currentValue = scanner.selected.pins[modelData];
                        scanner.selected.currentKey = modelData;
                        focus=false;
                        it.focus = true;
                    }
                }
            }

        }
        Item {
            id: it
            focus: true
            Keys.onPressed: {
                gl.currentValue = KeyInfo.findValueForEvent(event) || gl.currentValue;
                event.accepted = true;
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
