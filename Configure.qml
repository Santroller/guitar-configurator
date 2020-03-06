import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import net.tangentmc 1.0
import "defines.js" as Defines


Page {
    id: page
    title: "Configuration"
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
            Button {
                id: updateBt
                text: qsTr("Update")
                visible: scanner.selected.isOutdated
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: mainStack.replace("Programmer.qml")
            }
        }

        Label {
            id: inputLb
            text: qsTr("Controller Connectivity Type")
            fontSizeMode: Text.FixedSize
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
        }

        ComboBox {
            id: inputBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: Defines.fillCombobox("input")
            Binding { target: inputBox; property: "currentIndex"; value: inputBox.model.findIndex(s => s.value === scanner.selected.inputType) }

            onCurrentIndexChanged: scanner.selected.inputType = inputBox.model[inputBox.currentIndex].value
        }

        Label {
            id: inputLb1
            text: qsTr("Controller Output Type")
            fontSizeMode: Text.FixedSize
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
        }

        ComboBox {
            property var inited: false
            id: comboBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: Defines.fillCombobox("subtype")
            Binding { target: comboBox; property: "currentIndex"; value: comboBox.model.findIndex(s => s.value === scanner.selected.type) }

            onCurrentIndexChanged: {
                if (!inited) {
                    inited = true;
                    return;
                }

                scanner.selected.type = comboBox.model[comboBox.currentIndex].value
            }
        }
        Button {
            id: tilt
            text: qsTr("Configure Tilt")
            visible: scanner.selected.isGuitar
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                mainStack.push("Tilt.qml");
            }
        }
        Button {
            id: bind
            visible: scanner.selected.inputType === ArdwiinoDefinesValues.DIRECT
            text: qsTr("Configure Arduino Pin Bindings")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                scanner.selected.loadPins();
                mainStack.push("PinBindings.qml");
            }
        }
        Button {
            id: keybind
            visible: scanner.selected.type === ArdwiinoDefinesValues.KEYBOARD
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
                mainStack.replace("Programmer.qml");
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
            visible: mainStack.currentItem.title === "Configuration" && !scanner.selected.isOpen
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
            }
        }
        Button {
            id: returnMenu
            text: qsTr("Program a different device")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: mainStack.replace("Welcome.qml");
        }



    }
}



















































































/*##^##
Designer {
    D{i:0;autoSize:true;height:1080;width:1920}D{i:2;anchors_height:400;anchors_y:232}
D{i:3;anchors_y:774}D{i:1;anchors_height:60;anchors_width:802;anchors_x:646;anchors_y:658}
}
##^##*/
