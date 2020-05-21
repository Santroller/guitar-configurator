import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import QtQuick.Dialogs 1.0
import net.tangentmc 1.0
import QtGraphicalEffects 1.12
import "pins.js" as PinInfo
import "defines.js" as Defines
import "keys.js" as KeyInfo


ColumnLayout {
    id: column
    //Since the button is being pinned to the top of the page, we need to insert an empty line to overlap the version string
    Label {
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: " "
    }
    Button {
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        icon.source: scanner.isGraphical?"images/graphical.svg":"images/list.svg"
        onClicked: scanner.toggleGraphics()
        ToolTip.visible: hovered
        ToolTip.text: scanner.isGraphical?"Swap to List view":"Swap to Graphical view"
    }

    CloneHeroDialog {

    }

    ButtonConfigDialog {
        id:buttonDialog
    }

    InteractiveController {
        visible: scanner.isGraphical
        onHoveredButtonChanged: {
            if (hoveredButton) {
                buttonDialog.loadButton(hoveredButton,cursorX, cursorY);
            } else {
                buttonDialog.close();
            }
        }
    }
    ListController {
        visible: !scanner.isGraphical
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
                model: Defines.fillCombobox("InputType")
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
                model: Defines.fillCombobox("SubType")
                Binding { target: comboBox; property: "currentIndex"; value: {comboBox.model.findIndex(s => s.value === scanner.selected.type)} }

                onCurrentIndexChanged: {
                    scanner.selected.type = comboBox.model[comboBox.currentIndex].value
                    //When the controller type is changed, we need to disable any pins that are not used by that controller.
                    for (let pin of PinInfo.getUnused(scanner.selected.isGuitar, scanner.selected.isWii, scanner.selected.isLiveGuitar, scanner.selected.isRB)) {
                        scanner.selected.pins[pin] = 0xFF;
                    }
                }
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
            onClicked: tiltConfig.visible = true;
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
            onClicked: {
                mainStack.replace("Welcome.qml");
            }
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
    RowLayout {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        Button {
            id: leds
            text: qsTr("Configure LEDs")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: ledConfig.visible = true;
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
        title: "Writing Data"
        visible: !scanner.selected.ready
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        closePolicy: Popup.NoAutoClose
        ColumnLayout {
            Label {
                text: "Your configuration is writing, please wait"
            }

            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
    Dialog {
        id: ledConfig
        modal: true
        standardButtons: Dialog.Close
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: applicationWindow.width / 2
        height: applicationWindow.height /2

        ColumnLayout {
            id: mainContent
            anchors.fill: parent
            Label {
                text: qsTr("LED Type: ")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                fontSizeMode: Text.FixedSize
            }
            ComboBox {
                id: fretBox
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Defines.fillCombobox("FretLedMode")
                Binding { target: fretBox; property: "currentIndex"; value: fretBox.model.findIndex(s => s.value === scanner.selected.ledType) }

                onCurrentIndexChanged: scanner.selected.ledType = fretBox.model[fretBox.currentIndex].value
            }
            Label {
                visible: scanner.selected.hasAddressableLEDs
                text: qsTr("LED Order (drag to change)")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            ListView {
                property var labels: PinInfo.getLabels(scanner.selected.isGuitar, scanner.selected.isWii, scanner.selected.isLiveGuitar, scanner.selected.isRB, scanner.selected.isDrum);
                visible: scanner.selected.hasAddressableLEDs
                Layout.fillWidth: true
                Layout.fillHeight: true
                id: listView
                model: scanner.selected.leds
                orientation: ListView.Horizontal
                delegate: DraggableItem {
                    label: listView.labels[modelData] || ""
                    Rectangle {
                        height: {
                            var a = listView.width / scanner.selected.leds.length;
                            if (a > listView.height) return listView.height;
                            return a;
                        }

                        width: height
                        color: "#"+(scanner.selected.colours[modelData]).toString(16).padStart(6,"0");

                        // Bottom line border
                        Rectangle {
                            anchors {
                                left: parent.left
                                right: parent.right
                                bottom: parent.bottom
                            }
                            height: 1
                            color: "lightgrey"
                        }
                    }

                    draggedItemParent: mainContent

                    onMoveItemRequested: {
                        scanner.selected.moveLED(from, to);
                    }
                }
            }


        }
    }

    Dialog {
        id: tiltConfig
        modal: true
        standardButtons: Dialog.Close
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        ColumnLayout {
            Label {
                text: qsTr("Tilt Sensor Type: ")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                fontSizeMode: Text.FixedSize
            }
            ComboBox {
                id: tiltBox
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Defines.fillCombobox("TiltType")
                Binding { target: tiltBox; property: "currentIndex"; value: tiltBox.model.findIndex(s => s.value === scanner.selected.tiltType) }

                onCurrentIndexChanged: scanner.selected.tiltType = tiltBox.model[tiltBox.currentIndex].value
            }

            Label {
                id: orientation
                visible: scanner.selected.tiltType === ArdwiinoDefinesValues.MPU_6050;
                text: qsTr("Tilt Sensor Orientation: ")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                fontSizeMode: Text.FixedSize
            }
            ComboBox {
                id: orientationBox
                visible: scanner.selected.tiltType === ArdwiinoDefinesValues.MPU_6050;
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Defines.fillCombobox("GyroOrientation")
                Binding { target: orientationBox; property: "currentIndex"; value: orientationBox.model.findIndex(s => s.value === scanner.selected.orientation) }

                onCurrentIndexChanged: scanner.selected.orientation = orientationBox.model[orientationBox.currentIndex].value
            }

            Label {
                text: qsTr("Tilt Sensor Sensitivity: ")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }

            Slider {
                id: sliderTilt
                Layout.fillWidth: true
                to: 32767
                from: -32767
                Component.onCompleted: value = scanner.selected.sensitivity
                onValueChanged: scanner.selected.sensitivity = sliderTilt.value
                background: Rectangle {
                    y: 15
                    implicitWidth: 200
                    implicitHeight: 4
                    height: implicitHeight
                    radius: 2
                    color: "#bdbebf"
                }
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
