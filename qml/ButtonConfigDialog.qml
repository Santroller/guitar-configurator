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
    property var pins: PinInfo.getBindings(scanner.selected.getDirectBoardImage());
    //We don't want the wii specific labels, as they are only needed for the list controller
    property var labels: PinInfo.getLabels(scanner.selected.config.isGuitar, false, scanner.selected.config.isLiveGuitar, scanner.selected.config.isRB, scanner.selected.config.isDrum, scanner.selected.config.isMouse);
    property var isAnalog: false;
    property var hasPosNeg: false;
    function loadButton(button,cursorX,cursorY) {
        buttons = [button];
        hasPosNeg = false;
        if (button === "strum") {
            title = "Strum";
            buttons = ["Up","Down"];
            isAnalog = false;
        } else if (button === "ljoy") {
            title = "Left Joystick";
            buttons = ["LX", "LY"];
            isAnalog = true;
        } else if (button === "rjoy") {
            title = "Right Joystick";
            buttons = ["RX", "RY"];
            isAnalog = true;
        } else if (button === "scroll") {
            title = "Mouse Scroll";
            buttons = ["RX","RY"];
            isAnalog = true;
        } else if (button === "mouse") {
            title = "Mouse Movement";
            buttons = ["LX","LY"];
            isAnalog = true;
        }else {
            title = labels[button];
            isAnalog = scanner.selected.config.hasOwnProperty(`pins${button}Inverted`);
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
                    visible: scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT
                    text: qsTr("Change Pin Binding for "+buttonDialog.labels[modelData]+" (Currently: "+buttonDialog.pins(scanner.selected.config[`pins${modelData}`])+")")
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
                        checked: !!scanner.selected.config[`pins${modelData}Inverted`]
                        onCheckedChanged: scanner.selected.config[`pins${modelData}Inverted`] = checked
                    }
                }
                Repeater {
                    model: hasPosNeg?[modelData+"Pos",modelData+"Neg"]:[modelData]
                    RowLayout {
                        KeyBinding {
                            id: keyBinding
                            currentKey: modelData
                        }
                        Button {
                            visible: scanner.selected.config.isKeyboard
                            text: qsTr("Change Key Binding for "+KeyInfo.labels[modelData]+" (Currently: "+KeyInfo.getKeyName(scanner.selected.config[`keys${modelData}`])+")")
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            onClicked: keyBinding.open()
                        }
                    }
                }
                RowLayout {
                    visible: scanner.selected.config.hasAddressableLEDs
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
                        checked: scanner.selected.config.leds.includes(modelData)
                        onCheckedChanged: {
                            if (!checked) {
                                scanner.selected.config.clearLED(modelData);
                            } else if (!scanner.selected.config.leds.includes(modelData)) {
                                scanner.selected.config.setLED(modelData);
                            }
                        }
                    }
                }

                RowLayout {
                    visible: scanner.selected.config.hasAddressableLEDs && scanner.selected.config.leds.includes(modelData)
                    Button {
                        text: "Set LED colour"
                        onClicked: color.open()
                        id: colorBt
                    }
                    ColorPickerDialog {
                        id: color
                        colorVal: scanner.selected.config.ledColours[modelData]
                        buttons: [modelData]
                        onColorChanged: scanner.selected.config.setLED(modelData,colorVal)
                    }
                    Rectangle {
                        id: colorRect
                        color: "#"+(scanner.selected.config.ledColours[modelData]).toString(16).padStart(6,"0")
                        width: colorBt.height
                        height: colorBt.height
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
                        currentIndex: Math.max(0,model.findIndex(s => s.value === scanner.selected.config.midiType[modelData]))
                        onActivated: scanner.selected.config.setMidiType(modelData,model[currentIndex].value)
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
                        value: scanner.selected.config.midiNote[modelData] || 0
                        onValueChanged: scanner.selected.config.setMidiNoteValue(modelData,value)

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
                        value: scanner.selected.config.midiChannel[modelData] || 0
                        onValueChanged: scanner.selected.config.setMidiChannelValue(modelData,value)

                        from: 1
                        to: 10
                    }
                }

            }

        }
        RowLayout {
            visible: buttonDialog.buttons[0] === "LX"
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
                Component.onCompleted: checked = scanner.selected.config.mainMapLeftJoystickToDPad
                onCheckedChanged: {
                    scanner.selected.config.mainMapLeftJoystickToDPad = checked
                }
            }
        }
        RowLayout {
            visible: buttonDialog.buttons[0] === "LX"
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
                enabled: scanner.selected.config.mainMapLeftJoystickToDPad
                Component.onCompleted: value = scanner.selected.config.axisJoyThreshold
                onValueChanged: scanner.selected.config.axisJoyThreshold = slider2.value
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
            visible: buttonDialog.isAnalog && (scanner.selected.config.isKeyboard || scanner.selected.config.hasAddressableLEDs)
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
                Component.onCompleted: value = scanner.selected.config.axisTriggerThreshold
                onValueChanged: scanner.selected.config.axisTriggerThreshold = slider.value
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
                Component.onCompleted: checked = scanner.selected.config.mainMapStartSelectToHome
                onCheckedChanged: scanner.selected.config.mainMapStartSelectToHome = checked
            }
        }
    }
}
