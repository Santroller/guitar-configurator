import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import net.tangentmc 1.0

Page {
    id: page
    ColumnLayout {
        id: column
        anchors.fill: parent
        Label {
            id: title
            text: qsTr("Configuration")
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
                onClicked: scanner.selected.readDescription()
            }
        }

        ComboBox {
            id: orientationBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: {
                let model = []; 
                for (let i = 0; i <= MPU6050Orientations.END; i++) {
                    model.push({key: ArdwiinoLookup.getOrientationName(i), value:i});
                }
                return model
            }
            Binding { target: orientationBox; property: "currentIndex"; value: orientationBox.model.findIndex(s => s.value === scanner.selected.orientation) }

            onCurrentIndexChanged: scanner.selected.orientation = orientationBox.model[orientationBox.currentIndex].value
        }

        ComboBox {
            id: tiltBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: {
                let model = [];
                for (let i = 0; i <= TiltTypes.END; i++) {
                    model.push({key: ArdwiinoLookup.getTiltTypeName(i), value:i});
                }
                return model
            }
            Binding { target: tiltBox; property: "currentIndex"; value: tiltBox.model.findIndex(s => s.value === scanner.selected.tiltType) }

            onCurrentIndexChanged: scanner.selected.tiltType = tiltBox.model[tiltBox.currentIndex].value
        }

        ComboBox {
            id: inputBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: {
                let model = [];
                for (let i = 1; i <= InputTypes.END; i++) {
                    model.push({key: ArdwiinoLookup.getInputTypeName(i), value:i});
                }
                return model
            }
            Binding { target: inputBox; property: "currentIndex"; value: inputBox.model.findIndex(s => s.value === scanner.selected.inputType) }

            onCurrentIndexChanged: scanner.selected.inputType = inputBox.model[inputBox.currentIndex].value
        }

        ComboBox {
            id: comboBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: {
                let model = [];
                for (let i = 1; i <= Controllers.END; i++) {
                    let name = ArdwiinoLookup.getControllerTypeName(i);
                    if (name !== "Unknown Controller") {
                        model.push({key: name, value:i});
                    }
                }
                return model
            }
            Binding { target: comboBox; property: "currentIndex"; value: comboBox.model.findIndex(s => s.value === scanner.selected.type) }

            onCurrentIndexChanged: scanner.selected.type = comboBox.model[comboBox.currentIndex].value
        }
        Button {
            id: bind
            visible: scanner.selected.inputType === InputTypes.DIRECT_TYPE
            text: qsTr("Configure Arduino Pin Bindings")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                scanner.selected.loadPins();
                mainStack.push("PinBindings.qml");
            }
        }
        Button {
            id: keybind
            visible: scanner.selected.type === Controllers.KEYBOARD
            text: qsTr("Configure Keyboard Bindings")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                scanner.selected.loadKeys();
                mainStack.push("KeyBindings.qml");
            }
        }
        Button {
            id: restore
            text: qsTr("Restore Device back to Arduino")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            visible: scanner.selected.hasDFU
            onClicked: {
                programmer.setRestoring(true);
                mainStack.push("Programmer.qml");
            }
        }
        Button {
            id: configureContinue
            text: qsTr("Write")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            enabled: true
            onClicked: scanner.selected.writeConfig();
        }
        Dialog {
            id: detectionDialog
            title: "Updating device, please wait"
            visible: scanner.selected.waitingForNew
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            modal: true
            closePolicy: Popup.NoAutoClose

            ColumnLayout {
                Label {
                    text: qsTr("Please wait for your controller to reboot")
                }
                BusyIndicator {
                    Layout.alignment: Qt.AlignHCenter
                }
                Timer {
                    id: timer1
                    interval: 100
                    running: scanner.selected.waitingForNew
                    repeat: true
                    onTriggered: {
                        scanner.selected.findNew()
                    }
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
