import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import QtQuick.Dialogs 1.0
import net.tangentmc 1.0
import QtGraphicalEffects 1.12
import "defines.js" as Defines


ColumnLayout {
    id: column
    ColumnLayout {
        Layout.alignment: Qt.AlignHCenter
        Item {
            Image {
                x: (applicationWindow.width-image.paintedWidth)/2
                y: (applicationWindow.height-image.paintedHeight)/2
                property var base: Defines.getBoardBase(scanner.selected.type);
                id: image
                source: Defines.getBoardImage(scanner.selected.type)
                fillMode: Image.PreserveAspectFit
                Layout.maximumHeight: applicationWindow.height/2
                Layout.maximumWidth: applicationWindow.width/2
                sourceSize.width: applicationWindow.width/2
                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onPositionChanged: {
                        var s = scanner.findElement(image.base, image.width, image.height, mouseX, mouseY);
                        if (s !== "") {
                            image2.source = s;
                            image2.visible = true;
                            overlay.visible = true;
                        } else {
                            image2.visible = false;
                            overlay.visible = false;
                        }
                    }
                }
            }
            Image {
                x: (applicationWindow.width-image.paintedWidth)/2
                y: (applicationWindow.height-image.paintedHeight)/2
                id: image2
                visible: false
                fillMode: Image.PreserveAspectFit
                Layout.maximumHeight: applicationWindow.height/2
                Layout.maximumWidth: applicationWindow.width/2
                sourceSize.width: applicationWindow.width/2
            }
            ColorOverlay {
                id: overlay
                visible: false
                anchors.fill: image2
                source: image2
                color: "#80800000"
            }
        }
    }
    RowLayout {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        Button {
            id: change
            text: qsTr("Change Device Type")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {type.visible = true;}
        }
        Button {
            id: updateBt
            text: qsTr("Update Device")
            visible: scanner.selected.isOutdated
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {scanner.selected.prepareUpdate(); mainStack.replace("Programmer.qml")}
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
            id: configureContinue
            text: qsTr("Write")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            enabled: true
            onClicked: scanner.selected.writeConfig();
        }
        Button {
            id: returnMenu
            text: qsTr("Program a different device")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: mainStack.replace("Welcome.qml");
        }
        Button {
            id: startClone
            visible: scanner.selected.isGuitar || scanner.selected.isDrum
            text: qsTr("Link to Clone Hero")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            enabled: true
            onClicked: cloneDialog.visible = true;
        }
    }
    Dialog {
        id: type
        modal: true
        standardButtons: Dialog.Close

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        ColumnLayout {
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
                id: comboBox
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Defines.fillCombobox("subtype")
                Binding { target: comboBox; property: "currentIndex"; value: {comboBox.model.findIndex(s => s.value === scanner.selected.type)} }

                onCurrentIndexChanged: scanner.selected.type = comboBox.model[comboBox.currentIndex].value
            }
        }
    }

    RowLayout {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        Button {
            id: leds
            text: qsTr("Configure LEDs")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                mainStack.push("leds.qml");
            }
        }
        Button {
            id: bind
            visible: scanner.selected.inputType === ArdwiinoDefinesValues.DIRECT || scanner.selected.isGuitar
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
            id: mappings
            text: qsTr("Configure additional settings")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                mainStack.push("Mappings.qml");
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

    Dialog {
        id: cloneDialog
        title: "Clone Hero Connector"
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Close
        modal: true
        ColumnLayout {
            Label {
                text: qsTr("Clone Hero Connector - Game Version: "+ledhandler.version)
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            RowLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Label {
                    text: qsTr("Game Location: "+ledhandler.gameFolder)
                    fontSizeMode: Text.FixedSize
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                }
                Button {
                    id: findGame
                    text: qsTr("Locate your game directory")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onClicked: folderDialog.open()
                }
            }


            FileDialog {
                id: folderDialog
                folder: ledhandler.gameFolder
                title: "Please locate your game directory"
                selectFolder: true
                onAccepted: ledhandler.gameFolder = fileUrl
            }

            Button {
                id: startGame
                text: qsTr("Start Clone Hero")
                enabled: ledhandler.ready
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: ledhandler.startGame();
            }
        }
    }


}
































































/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.6600000262260437;height:1080;width:1920}D{i:3;anchors_y:774}
D{i:2;anchors_height:400;anchors_y:232}D{i:1;anchors_height:60;anchors_width:802;anchors_x:646;anchors_y:658}
}
##^##*/
