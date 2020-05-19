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

GridLayout {
    id: gl
    rows: Object.values(gl.labels).length+1
    columns: 3+scanner.selected.isKeyboard+scanner.selected.hasAddressableLEDs
    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    property var pins: PinInfo.getBindings(scanner.selected.boardImage);
    property var labels: PinInfo.getLabels(scanner.selected.isGuitar, scanner.selected.isWii, scanner.selected.isLiveGuitar, scanner.selected.isRB, scanner.selected.isDrum);
    property var pWidth: gl.parent.width-50
    flow: GridLayout.TopToBottom
    Label {
        text: "Actions"
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }

    Repeater {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        model: Object.values(gl.labels)
        Label {
            Layout.preferredWidth: gl.pWidth/gl.columns
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            id: label
            text: modelData
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
        }
    }
    Label {
        text: "Pin Binding"
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Repeater {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        model: Object.keys(gl.labels)
        RowLayout {
            PinBinding {
                id: pinBinding
                currentPin: modelData
            }
            Button {
                Layout.preferredWidth: gl.pWidth/gl.columns
                Layout.fillHeight: true
                text: gl.pins(scanner.selected.pins[modelData])
                onClicked: pinBinding.open()

                ToolTip.visible: hovered
                ToolTip.text: gl.labels[modelData]
            }
        }
    }
    Label {
        visible: scanner.selected.isKeyboard
        text: "Key Binding"
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Repeater {
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        model: scanner.selected.isKeyboard?Object.keys(gl.labels):[]
        RowLayout {
            id:rl
            Layout.fillHeight: true
            Layout.fillWidth: true
            Repeater {
                id:r
                property var buttonCount:0
                property var label:""
                model: {
                    label = gl.labels[modelData]
                    if (modelData.startsWith("l_") || modelData.startsWith("r_")) {
                        buttonCount = 2;
                        return [modelData+"_lt", modelData+"_gt"];
                    }
                    buttonCount = 1;
                    return [modelData];
                }
                RowLayout {
                    KeyBinding {
                        id: keyBinding
                        currentKey: modelData
                    }

                    Button {
                        id: keyButton
                        Layout.preferredWidth: (gl.pWidth/gl.columns/r.buttonCount) - 6
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
    }
    Label {
        text: "Invert Axis"
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Repeater {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        model: Object.keys(gl.labels)
        RowLayout {
            Switch {
                Layout.preferredWidth: gl.pWidth/gl.columns
                Layout.fillHeight: true
                enabled: scanner.selected.pin_inverts.hasOwnProperty(modelData)
                visible: enabled
                checked: !!scanner.selected.pin_inverts[modelData]
                onCheckedChanged: {
                    var pins = scanner.selected.pin_inverts;
                    pins[modelData] = checked;
                    scanner.selected.pin_inverts = pins;
                }
            }
        }
    }
    Label {
        visible: scanner.selected.hasAddressableLEDs
        text: "LEDs"
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Repeater {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        model: scanner.selected.hasAddressableLEDs?Object.keys(gl.labels):[]
        RowLayout {
            Switch {
                Component.onCompleted: checked = scanner.selected.leds.includes(modelData)
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
            Rectangle {
                radius: colorBt2.height
                visible: scanner.selected.leds.includes(modelData) && scanner.selected.hasAddressableLEDs
                color: "#"+(scanner.selected.colours[modelData] || 0).toString(16).padStart(6,"0")
                width: colorBt2.height
                height: colorBt2.height
            }
            ColorPickerDialog {
                id: color
                buttons: [modelData]
                colorVal: scanner.selected.colours[modelData]
                onColorChanged: {
                    var colours = scanner.selected.colours;
                    colours[modelData] = colorVal;
                    scanner.selected.colours = colours;
                }
            }
            Button {
                visible: scanner.selected.leds.includes(modelData) && scanner.selected.hasAddressableLEDs
                text: "Change colour"
                onClicked: color.open()
                id: colorBt2
            }
        }
    }

}
