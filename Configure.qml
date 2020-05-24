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
                scanner.hasSelected = false;
                mainStack.replace("Welcome.qml");
            }
        }
        Button {
            id: startClone
            visible: scanner.selected.isGuitar || scanner.selected.isDrum
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
        title: "Controller Disconnected"
        visible: scanner.selected.disconnected
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        closePolicy: Popup.NoAutoClose
        standardButtons: Dialog.Ok
        onVisibleChanged: {
            scanner.hasSelected = false;
        }

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
        property var pins: PinInfo.getBindings(scanner.selected.boardImage);
        property var labels: PinInfo.getLabels(scanner.selected.isGuitar, scanner.selected.isWii, scanner.selected.isLiveGuitar, scanner.selected.isRB, scanner.selected.isDrum);
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
            Label {
                text: "Pin Binding"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            RowLayout {
                PinBinding {
                    id: pinBinding
                    currentPin: "r_y"
                }
                Button {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: tiltConfig.pins(scanner.selected.pins["r_y"])
                    onClicked: pinBinding.open()

                    ToolTip.visible: hovered
                    ToolTip.text: tiltConfig.labels["r_y"]
                }
            }
            Label {
                visible: scanner.selected.isKeyboard
                text: "Key Binding"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            RowLayout {
                visible: scanner.selected.isKeyboard
                id:rl
                Layout.fillHeight: true
                Layout.fillWidth: true
                Repeater {
                    id:r
                    property var buttonCount:0
                    property var label:""
                    model: {
                        label = tiltConfig.labels["r_y"]
                        buttonCount = 2;
                        return ["r_y_lt","r_y_gt"];
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
                            visible: scanner.selected.isKeyboard
                            text: KeyInfo.getKeyName(scanner.selected.keys[modelData])
                            onClicked: keyBinding.open()
                            ToolTip.visible: hovered
                            ToolTip.text: {
                                if (modelData.endsWith("_lt")) {
                                    return r.label + " -"
                                } else if (modelData.endsWith("_gt")) {
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
                checked: !!scanner.selected.pin_inverts["r_y"]
                onCheckedChanged: {
                    var pins = scanner.selected.pin_inverts;
                    pins["r_y"] = checked;
                    scanner.selected.pin_inverts = pins;
                }
            }
            Label {
                visible: scanner.selected.hasAddressableLEDs
                text: "LEDs"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            RowLayout {
                visible: scanner.selected.hasAddressableLEDs
                Layout.fillWidth: true
                Switch {
                    Component.onCompleted: checked = scanner.selected.leds.includes("r_y")
                    onCheckedChanged: {
                        var leds = scanner.selected.leds;
                        var colours = scanner.selected.colours;
                        if (!checked) {
                            leds.splice(leds.indexOf("r_y"),1);
                        } else if (!leds.includes("r_y")){
                            leds.push("r_y");
                            if (!colours["r_y"]) {
                                colours["r_y"] = 0;
                            }
                        }
                        scanner.selected.leds = leds;
                        scanner.selected.colours = colours;
                    }
                }
                Rectangle {
                    radius: colorBt2.height
                    visible: scanner.selected.leds.includes("r_y") && scanner.selected.hasAddressableLEDs
                    color: "#"+(scanner.selected.colours["r_y"] || 0).toString(16).padStart(6,"0")
                    width: colorBt2.height
                    height: colorBt2.height
                }
                ColorPickerDialog {
                    id: color
                    buttons: ["r_y"]
                    colorVal: scanner.selected.colours["r_y"]
                    onColorChanged: {
                        var colours = scanner.selected.colours;
                        colours["r_y"] = colorVal;
                        scanner.selected.colours = colours;
                    }
                }
                Button {
                    visible: scanner.selected.leds.includes("r_y") && scanner.selected.hasAddressableLEDs
                    text: "Change colour"
                    onClicked: color.open()
                    id: colorBt2
                }
            }
            RowLayout {
                visible: scanner.selected.isMIDI
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
                    currentIndex: Math.max(0,model.findIndex(s => s.value === scanner.selected.midi_type["r_y"]))
                    onActivated: {
                        var midi_type = scanner.selected.midi_type;
                        midi_type["r_y"] = model[currentIndex].value;
                        scanner.selected.midi_type = midi_type;
                    }
                }
            }

            RowLayout {
                visible: scanner.selected.isMIDI
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
                    value: scanner.selected.midi_note["r_y"] || 0
                    onValueChanged: {
                        var midi_note = scanner.selected.midi_note;
                        midi_note["r_y"] = value
                        scanner.selected.midi_note = midi_note;
                    }

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
                visible: scanner.selected.isMIDI
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
                    value: scanner.selected.midi_channel["r_y"] || 0
                    onValueChanged: {
                        var midi_channel = scanner.selected.midi_channel;
                        midi_channel["r_y"] = value;
                        scanner.selected.midi_channel = midi_channel;
                    }

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
