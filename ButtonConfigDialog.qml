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

Dialog {
    id: buttonDialog
    property var buttons: [];
    property var pins: PinInfo.getBindings(scanner.selected.boardImage);
    //We don't want the wii specific labels, as they are only needed for the list controller
    property var labels: PinInfo.getLabels(scanner.selected.isGuitar, false, scanner.selected.isLiveGuitar, scanner.selected.isRB, scanner.selected.isDrum, scanner.selected.isMouse);
    property var isAnalog: false;
    property var hasPosNeg: false;
    function loadButton(button,cursorX,cursorY) {
        buttons = [button];
        hasPosNeg = false;
        if (button === "strum") {
            title = "Strum";
            buttons = ["up","down"];
            isAnalog = false;
        } else if (button === "ljoy") {
            title = "Left Joystick";
            buttons = ["l_x", "l_y"];
            isAnalog = true;
        } else if (button === "rjoy") {
            title = "Right Joystick";
            buttons = ["r_x", "r_y"];
            isAnalog = true;
        } else if (button === "scroll") {
            title = "Mouse Scroll";
            buttons = ["r_x","r_y"];
            isAnalog = true;
        } else if (button === "mouse") {
            title = "Mouse Movement";
            buttons = ["l_x","l_y"];
            isAnalog = true;
        }else {
            title = labels[button];
            isAnalog = scanner.selected.pin_inverts.hasOwnProperty(button);
        }
        hasPosNeg = isAnalog && button !== "lt" && button !== "rt";
        x = cursorX-buttonDialog.width/2;
        y = cursorY;
        open();
    }

    ColumnLayout {
        Repeater {
            model: buttonDialog.buttons
            ColumnLayout {
                PinBinding {
                    id: pinBinding
                    currentPin: modelData
                }
                Button {
                    visible: scanner.selected.inputType === ArdwiinoDefinesValues.DIRECT
                    text: qsTr("Change Pin Binding for "+buttonDialog.labels[modelData]+" (Currently: "+buttonDialog.pins(scanner.selected.pins[modelData])+")")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onClicked: pinBinding.open()
                }
                RowLayout {
                    visible: buttonDialog.isAnalog
                    Label {
                        text: qsTr("Invert "+buttonDialog.labels[modelData])
                        fontSizeMode: Text.FixedSize
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        wrapMode: Text.WordWrap
                    }
                    Switch {
                        enabled: buttonDialog.isAnalog
                        checked: !!scanner.selected.pin_inverts[modelData]
                        onCheckedChanged: {
                            var pin_inverts = scanner.selected.pin_inverts;
                            pin_inverts[modelData] = checked;
                            scanner.selected.pin_inverts = pin_inverts;
                        }
                    }
                }
                Repeater {
                    model: hasPosNeg?[modelData+"_lt",modelData+"_gt"]:[modelData]
                    RowLayout {
                        KeyBinding {
                            id: keyBinding
                            currentKey: modelData
                        }
                        Button {
                            visible: scanner.selected.isKeyboard
                            text: qsTr("Change Key Binding for "+KeyInfo.labels[modelData]+" (Currently: "+KeyInfo.getKeyName(scanner.selected.keys[modelData])+")")
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            onClicked: keyBinding.open()
                        }
                    }
                }
                RowLayout {
                    visible: scanner.selected.hasAddressableLEDs
                    Label {
                        text: qsTr("Enable LEDs for "+buttonDialog.labels[modelData])
                        fontSizeMode: Text.FixedSize
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        wrapMode: Text.WordWrap
                    }
                    Switch {
                        id:sw
                        checked: scanner.selected.leds.includes(modelData)
                        onCheckedChanged: {
                            var leds = scanner.selected.leds;
                            var colours = scanner.selected.colours;
                            if (!checked) {
                                leds.splice(leds.indexOf(modelData),1);
                            } else if (!leds.includes(modelData)){
                                leds.push(modelData);
                                if (!colours[modelData]) {
                                    colours[modelData] = 0;
                                }
                            }
                            scanner.selected.leds = leds;
                            scanner.selected.colours = colours;
                        }
                    }
                }

                RowLayout {
                    visible: scanner.selected.hasAddressableLEDs && scanner.selected.leds.includes(modelData)
                    Button {
                        text: "Set LED colour"
                        onClicked: color.open()
                        id: colorBt
                    }
                    ColorPickerDialog {
                        id: color
                        colorVal: scanner.selected.colours[modelData] || 0
                        buttons: [modelData]
                        onColorChanged: {
                            var colours = scanner.selected.colours;
                            colours[modelData] = colorVal;
                            scanner.selected.colours = colours;
                        }
                    }
                    Rectangle {
                        id: colorRect
                        color: "#"+(scanner.selected.colours[modelData] || 0).toString(16).padStart(6,"0")
                        width: colorBt.height
                        height: colorBt.height
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
                        currentIndex: Math.max(0,model.findIndex(s => s.value === scanner.selected.midi_type[modelData]))
                        onActivated: {
                            var midi_type = scanner.selected.midi_type;
                            midi_type[modelData] = model[currentIndex].value;
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
                        value: scanner.selected.midi_note[modelData] || 0
                        onValueChanged: {
                            var midi_note = scanner.selected.midi_note;
                            midi_note[modelData] = value
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
                        value: scanner.selected.midi_channel[modelData] || 0
                        onValueChanged: {
                            var midi_channel = scanner.selected.midi_channel;
                            midi_channel[modelData] = value;
                            scanner.selected.midi_channel = midi_channel;
                        }

                        from: 1
                        to: 10
                    }
                }

            }

        }
        RowLayout {
            visible: buttonDialog.buttons[0] === "l_x" && (scanner.selected.isGuitar || scanner.selected.isDrum)
            Label {
                text: qsTr("Map Left Joystick to D-pad")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Component.onCompleted: checked = scanner.selected.mapJoystick
                onCheckedChanged: {
                    scanner.selected.mapJoystick = checked
                }
            }
        }
        RowLayout {
            visible: buttonDialog.buttons[0] === "l_x" && (scanner.selected.isGuitar || scanner.selected.isDrum)
            Label {
                text: "Joystick Mapping Threshold"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Slider {
                id: slider2
                to: 128
                from: 0
                live: false
                enabled: scanner.selected.mapJoystick
                Component.onCompleted: value = scanner.selected.joyThreshold
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
        }
        RowLayout {
            visible: buttonDialog.isAnalog && (scanner.selected.isKeyboard || scanner.selected.hasAddressableLEDs)
            Label {
                text: "Key Axis Threshold / LED Axis Threshold"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Slider {
                id: slider
                to: 128
                from: 0
                live: false
                Component.onCompleted: value = scanner.selected.triggerThreshold
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
        }


        RowLayout {
            visible: ["back","start","home"].includes(buttonDialog.buttons[0])
            Label {
                text: qsTr("Map Start + Select to PS Button / Home / Xbox Dashboard")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Component.onCompleted: checked = scanner.selected.mapStartSelectHome
                onCheckedChanged: scanner.selected.mapStartSelectHome = checked
            }
        }
    }
}
