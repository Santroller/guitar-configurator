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
    rows: {
        if ((scanner.selected.config.hasAddressableLEDs || scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT)) { 
            return Object.values(gl.labels).length+1
        } else if (scanner.selected.config.isGuitar) {
            return 2;
        }
        return 0;
    }
    columns: 2+scanner.selected.config.isKeyboard+scanner.selected.config.hasAddressableLEDs+((scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || scanner.selected.config.isGuitar) * 2)
    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    property var pins: PinInfo.getBindings(scanner.selected.getDirectBoardImage());
    property var labels: PinInfo.getLabels(scanner.selected.config.isGuitar, scanner.selected.config.isWii, scanner.selected.config.isLiveGuitar, scanner.selected.config.isRB, scanner.selected.config.isDrum, scanner.selected.config.isMouse);
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
        model: Object.keys(gl.labels)
        Label {
            Layout.preferredWidth: gl.pWidth/gl.columns
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            id: label
            text: gl.labels[modelData]
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            visible: {
                return scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || scanner.selected.config.hasAddressableLEDs || (modelData == "RY" && scanner.selected.config.isGuitar)
            }
        }
    }
    Label {
        visible: scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || (scanner.selected.config.isGuitar)
        text: "Pin Binding"
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Repeater {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        model: Object.keys(gl.labels)
        RowLayout {
            visible: scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || (modelData == "RY" && scanner.selected.config.isGuitar)
            PinBinding {
                id: pinBinding
                currentPin: modelData
            }
            Button {
                Layout.preferredWidth: gl.pWidth/gl.columns
                Layout.fillHeight: true
                text: gl.pins(scanner.selected.config[`pins${modelData}`])
                onClicked: pinBinding.open()

                ToolTip.visible: hovered
                ToolTip.text: gl.labels[modelData]
            }
        }
    }
    Label {
        visible: scanner.selected.config.isKeyboard
        text: "Key Binding"
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Repeater {
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        model: scanner.selected.config.isKeyboard?Object.keys(gl.labels):[]
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
                    if (modelData.match("[LR][A-Z].+")) {
                        buttonCount = 2;
                        return [modelData+"Neg", modelData+"Pos"];
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
    }
    Label {
        text: "Invert Axis"
        enabled: scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || scanner.selected.config.isGuitar
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Repeater {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        model: Object.keys(gl.labels)
        RowLayout {
            enabled: scanner.selected.config.hasOwnProperty(`pins${modelData}Inverted`) && (scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || (modelData == "RY" && scanner.selected.config.isGuitar)) || scanner.selected.config.hasAddressableLEDs
            Switch {
                Layout.preferredWidth: gl.pWidth/gl.columns
                Layout.fillHeight: true
                enabled: scanner.selected.config.hasOwnProperty(`pins${modelData}Inverted`) && (scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || (modelData == "RY" && scanner.selected.config.isGuitar)) 
                visible: enabled
                checked: !!scanner.selected.config[`pins${modelData}Inverted`]
                onCheckedChanged: scanner.selected.config[`pins${modelData}Inverted`] = checked
            }
        }
    }
    Label {
        text: "Calibrate Axis"
        enabled: scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || scanner.selected.config.isGuitar
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Repeater {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        model: Object.keys(gl.labels)
        RowLayout {
            enabled: scanner.selected.config.hasOwnProperty(`pins${modelData}Inverted`) && (scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || (modelData == "RY" && scanner.selected.config.isGuitar)) || scanner.selected.config.hasAddressableLEDs
            Button {
                Layout.preferredWidth: gl.pWidth/gl.columns
                Layout.fillHeight: true
                enabled: scanner.selected.config.hasOwnProperty(`pins${modelData}Inverted`) && (scanner.selected.config.mainInputType === ArdwiinoDefinesValues.DIRECT || (modelData == "RY" && scanner.selected.config.isGuitar)) 
                visible: enabled
                text: "Calibrate " + gl.labels[modelData]
                onClicked: {
                    calibDialog.open()
                }
            }
            CalibrationDialog {
                id:calibDialog
                pin: scanner.selected.config[`pins${modelData}`]
                axis: modelData
                isWhammy: gl.labels[modelData]==="Whammy"
                onCalibrationChanged: {
                    scanner.selected.config[`axisScale${modelData}Multiplier`] = mulFactor * 1000
                    scanner.selected.config[`axisScale${modelData}Offset`] = min
                }
            }
        }
    }
    Label {
        visible: scanner.selected.config.hasAddressableLEDs
        text: "LEDs"
        font.pointSize: 15
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Repeater {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        model: scanner.selected.config.hasAddressableLEDs?Object.keys(gl.labels):[]
        RowLayout {
            Switch {
                Component.onCompleted: checked = scanner.selected.config.leds.includes(modelData)
                onCheckedChanged: {
                    if (!checked) {
                        scanner.selected.config.clearLED(modelData);
                    } else if (!scanner.selected.config.leds.includes(modelData)) {
                        scanner.selected.config.setLED(modelData, 0);
                    }
                }
            }
            Rectangle {
                id: ledRect
                radius: colorBt2.height
                visible: scanner.selected.config.leds.includes(modelData) && scanner.selected.config.hasAddressableLEDs
                color: "#"+(scanner.selected.config.ledColours[modelData]).toString(16).padStart(6,"0")
                width: colorBt2.height
                height: colorBt2.height
            }
            ColorPickerDialog {
                id: color
                buttons: [modelData]
                colorVal: scanner.selected.config.ledColours[modelData]
                onColorChanged: scanner.selected.config.setLED(modelData,colorVal)
            }
            Button {
                visible: scanner.selected.config.leds.includes(modelData) && scanner.selected.config.hasAddressableLEDs
                text: "Change colour"
                onClicked: color.open()
                id: colorBt2
            }
        }
    }

}
