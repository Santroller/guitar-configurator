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
        icon.source: scanner.isGraphical?"/images/graphical.svg":"/images/list.svg"
        onClicked: scanner.toggleGraphics()
        ToolTip.visible: hovered
        ToolTip.text: scanner.isGraphical?"Swap to List view":"Swap to Graphical view"
    }

    CloneHeroDialog {
        id:cloneDialog
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
                Binding { target: inputBox; property: "currentIndex"; value: inputBox.model.findIndex(s => s.value === scanner.selected.config.mainInputType) }

                onCurrentIndexChanged: scanner.selected.config.mainInputType = inputBox.model[inputBox.currentIndex].value
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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Object.keys(Defines.getTypeCombobox())
                Binding { target: comboBox; property: "currentIndex"; value: comboBox.model.indexOf(Defines.findTypeDevice(scanner.selected.config.mainSubType)[0])}
                onActivated: {
                    scanner.selected.config.mainSubType = comboBox2.model[0].value
                    comboBox2.currentIndex = 0;
                    //When the controller type is changed, we need to disable any pins that are not used by that controller.
                    for (let pin of PinInfo.getUnused(scanner.selected.config.isGuitar, scanner.selected.config.isWii, scanner.selected.config.isLiveGuitar, scanner.selected.config.isRB)) {
                        scanner.selected.config[`pins${pin}`] = 0xFF;
                    }
                }
            }

            ComboBox {
                id: comboBox2
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                visible: Defines.getTypeCombobox()[comboBox.model[comboBox.currentIndex]][0].key.length
                model: Defines.getTypeCombobox()[comboBox.model[comboBox.currentIndex]]
                Binding { target: comboBox2; property: "currentIndex"; value: comboBox2.model.findIndex(s => s.value === scanner.selected.config.mainSubType)}

                onActivated: {
                    scanner.selected.config.mainSubType = comboBox2.model[comboBox2.currentIndex].value
                    //When the controller type is changed, we need to disable any pins that are not used by that controller.
                    for (let pin of PinInfo.getUnused(scanner.selected.config.isGuitar, scanner.selected.config.isWii, scanner.selected.config.isLiveGuitar, scanner.selected.config.isRB)) {
                        scanner.selected.config[`pins${pin}`] = 0xFF;
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
            id: tilt
            text: qsTr("Configure Tilt")
            visible: scanner.selected.config.isGuitar
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
                scanner.hasSelected = false;
                mainStack.replace("Welcome.qml");
            }
        }
        Button {
            id: startClone
            visible: scanner.selected.config.isGuitar || scanner.selected.config.isDrum
            text: qsTr("Link to Clone Hero")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            enabled: true
            onClicked: cloneDialog.open()
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
        title: "Controller Disconnected"
        visible: !scanner.hasSelected
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        closePolicy: Popup.NoAutoClose
        standardButtons: Dialog.Ok

        onAccepted:  {
            mainStack.replace("Welcome.qml");
        }
        ColumnLayout {
            Label {
                text: "Your controller has been disconnected."
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
                Binding { target: fretBox; property: "currentIndex"; value: fretBox.model.findIndex(s => s.value === scanner.selected.config.mainFretLEDMode) }

                onCurrentIndexChanged: scanner.selected.config.mainFretLEDMode = fretBox.model[fretBox.currentIndex].value
            }
            Label {
                visible: scanner.selected.config.hasAddressableLEDs
                text: qsTr("LED Order (drag to change)")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            ListView {
                property var labels: PinInfo.getLabels(scanner.selected.config.isGuitar, scanner.selected.config.isWii, scanner.selected.config.isLiveGuitar, scanner.selected.config.isRB, scanner.selected.config.isDrum, scanner.selected.config.isMouse);
                visible: scanner.selected.config.hasAddressableLEDs
                Layout.fillWidth: true
                Layout.fillHeight: true
                id: listView
                model: scanner.selected.config.leds
                orientation: ListView.Horizontal
                delegate: DraggableItem {
                    label: listView.labels[modelData] || ""
                    Rectangle {
                        height: {
                            var a = listView.width / scanner.selected.config.leds.length;
                            if (a > listView.height) return listView.height;
                            return a;
                        }

                        width: height
                        color: "#"+(scanner.selected.config.ledColours[modelData]).toString(16).padStart(6,"0");

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
                        scanner.selected.config.moveLED(from, to);
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
        property var pins: PinInfo.getBindings(scanner.selected.boardImage);
        property var labels: PinInfo.getLabels(scanner.selected.config.isGuitar, scanner.selected.config.isWii, scanner.selected.config.isLiveGuitar, scanner.selected.config.isRB, scanner.selected.config.isDrum, scanner.selected.config.isMouse);
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
                Binding { target: tiltBox; property: "currentIndex"; value: tiltBox.model.findIndex(s => s.value === scanner.selected.config.mainTiltType) }

                onCurrentIndexChanged: scanner.selected.config.mainTiltType = tiltBox.model[tiltBox.currentIndex].value
            }

            Label {
                id: orientation
                visible: scanner.selected.config.mainTiltType === ArdwiinoDefinesValues.MPU_6050;
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
                visible: scanner.selected.config.mainTiltType === ArdwiinoDefinesValues.MPU_6050;
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Defines.fillCombobox("GyroOrientation")
                Binding { target: orientationBox; property: "currentIndex"; value: orientationBox.model.findIndex(s => s.value === scanner.selected.config.axisMpu6050Orientation) }

                onCurrentIndexChanged: scanner.selected.config.axisMpu6050Orientation = orientationBox.model[orientationBox.currentIndex].value
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
                Component.onCompleted: value = scanner.selected.config.axisTiltSensitivity
                onValueChanged: scanner.selected.config.axisTiltSensitivity = sliderTilt.value
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
                text: "Pin Binding"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            RowLayout {
                PinBinding {
                    id: pinBinding
                    currentPin: "RY"
                }
                Button {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: tiltConfig.pins(scanner.selected.config[`pins${"RY"}`])
                    onClicked: pinBinding.open()
                    ToolTip.visible: hovered
                    ToolTip.text: tiltConfig.labels["RY"]
                }
            }
            Label {
                visible: scanner.selected.config.isKeyboard
                text: "Key Binding"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            RowLayout {
                visible: scanner.selected.config.isKeyboard
                id:rl
                Layout.fillHeight: true
                Layout.fillWidth: true
                Repeater {
                    id:r
                    property var buttonCount:0
                    property var label:""
                    model: {
                        label = tiltConfig.labels["RY"]
                        buttonCount = 2;
                        return ["RYPos","RYNeg"];
                    }
                    RowLayout {
                        KeyBinding {
                            id: keyBinding
                            currentKey: modelData
                        }

                        Button {
                            id: keyButton
                            Layout.preferredWidth: (tiltConfig.pWidth/tiltConfig.columns/r.buttonCount) - 6
                            Layout.fillHeight: true
                            visible: scanner.selected.config.isKeyboard
                            text: KeyInfo.getKeyName(scanner.selected.config[`keys${modelData}`])
                            onClicked: keyBinding.open()
                            ToolTip.visible: hovered
                            ToolTip.text: {
                                if (modelData.endsWith("Neg")) {
                                    return r.label + " -"
                                } else if (modelData.endsWith("Pos")) {
                                    return r.label +" +";
                                } else {
                                    return r.label;
                                }
                            }
                        }
                    }
                }
            }
            Label {
                text: "Invert Tilt Axis"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: enabled
                checked: !!scanner.selected.config[`pins${"RY"}Inverted`]
                onCheckedChanged: canner.selected.config[`pins${"RY"}Inverted`] = checked
            }
            Label {
                visible: scanner.selected.config.hasAddressableLEDs
                text: "LEDs"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            RowLayout {
                visible: scanner.selected.config.hasAddressableLEDs
                Layout.fillWidth: true
                Switch {
                    Component.onCompleted: checked = scanner.selected.config.leds.includes("RY")
                    onCheckedChanged: {
                        if (!checked) {
                            scanner.selected.config.clearLED("RY");
                        } else if (!scanner.selected.config.leds.includes("RY")){
                            scanner.selected.config.setLED("RY",0);
                        }
                    }
                }
                Rectangle {
                    radius: colorBt2.height
                    visible: scanner.selected.config.leds.includes("RY") && scanner.selected.config.hasAddressableLEDs
                    color: "#"+(scanner.selected.config.ledColours["RY"]).toString(16).padStart(6,"0")
                    width: colorBt2.height
                    height: colorBt2.height
                }
                ColorPickerDialog {
                    id: color
                    buttons: ["RY"]
                    colorVal: scanner.selected.config.ledColours["RY"]
                    onColorChanged: scanner.selected.config.setLED("RY",colorVal)
                }
                Button {
                    visible: scanner.selected.config.ledColours["RY"] && scanner.selected.config.hasAddressableLEDs
                    text: "Change colour"
                    onClicked: color.open()
                    id: colorBt2
                }
            }
            RowLayout {
                visible: scanner.selected.config.isMIDI
                Label {
                    text: qsTr("MIDI Type")
                    fontSizeMode: Text.FixedSize
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                }
                ComboBox {
                    Layout.fillWidth: true
                    textRole: "key"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    model: Defines.fillCombobox("MidiType")
                    currentIndex: Math.max(0,model.findIndex(s => s.value === scanner.selected.config.midiTypeMap["RY"]))
                    onActivated: scanner.selected.config.setMidiType("RY",model[currentIndex].value)
                }
            }

            RowLayout {
                visible: scanner.selected.config.isMIDI
                Label {
                    text: qsTr("MIDI Note")
                    fontSizeMode: Text.FixedSize
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                }
                SpinBox {
                    property var notes: ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B']
                    id: noteBox
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    value: scanner.selected.config.midiNoteMap["RY"] || 0
                    onValueChanged: scanner.selected.config.setMidiNoteValue("RY",value)

                    editable: true
                    inputMethodHints: Qt.ImhNone
                    textFromValue: (value)=>notes[(value)%12]+(((value+6)/12)-2).toFixed(0)
                    from: 0
                    to: 127
                    validator: RegExpValidator { regExp: /^(\w#?)(-?\d+)$/ }
                    valueFromText: (text)=>{
                                       var matches = text.match(/^(\w#?)(-?\d+)$/)
                                       var note = notes.indexOf(matches[1]);
                                       var octave = parseInt(matches[2]);
                                       return ((octave + 1)*12)+note;
                                   }
                }
            }

            RowLayout {
                visible: scanner.selected.config.isMIDI
                Label {
                    text: qsTr("MIDI Channel")
                    fontSizeMode: Text.FixedSize
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                }
                SpinBox {
                    id: chanBox
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    value: scanner.selected.config.midiChannelMap["RY"] || 0
                    onValueChanged: scanner.selected.config.setMidiChannelValue("RY",value)
                    from: 1
                    to: 10
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
