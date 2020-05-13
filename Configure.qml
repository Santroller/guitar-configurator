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

    Item {
        id:keyListener
        focus: true
        Keys.onPressed: {
            buttonConfig.currentKeyValue = KeyInfo.findValueForEvent(event) || page.currentKeyValue;
            event.accepted = true;
        }
    }
    GridLayout {
        visible: !scanner.isGraphical
        id: gl
        rows: Object.values(gl.labels).length+1
        columns: 3+scanner.selected.isKeyboard+scanner.selected.hasAddressableLEDs
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        property var current: PinInfo.bindings[scanner.selected.boardImage]
        property var labels: PinInfo.getLabels(scanner.selected.isGuitar, scanner.selected.isWii, scanner.selected.isLiveGuitar, scanner.selected.isRB);
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
            Button {
                Layout.preferredWidth: gl.pWidth/gl.columns
                Layout.fillHeight: true
                id: bt
                text: gl.current[scanner.selected.pins[modelData]] || scanner.selected.pins[modelData]
                onClicked: buttonConfig.currentPin = modelData;
                ToolTip.visible: hovered
                ToolTip.text: gl.labels[modelData]
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
                    Button {
                        Layout.preferredWidth: (gl.pWidth/gl.columns/r.buttonCount) - 6
                        Layout.fillHeight: true
                        visible: scanner.selected.isKeyboard
                        text: KeyInfo.getKeyName(scanner.selected.keys[modelData])
                        onClicked: {
                            buttonConfig.currentKey = modelData;
                            buttonConfig.currentKeyValue = scanner.selected.keys[modelData];
                            keyListener.forceActiveFocus();
                        }
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
                id: ledRl
                function getColor() {
                    if (!scanner.selected.colours[modelData]) return "";
                    return "#"+(scanner.selected.colours[modelData]).toString(16).padStart(6,"0");
                }
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
                    color: ledRl.getColor()
                    width: colorBt2.height
                    height: colorBt2.height
                }
                Button {
                    visible: scanner.selected.leds.includes(modelData) && scanner.selected.hasAddressableLEDs
                    text: "Change colour"
                    onClicked: {
                        ledhandler.color = scanner.selected.colours[modelData];
                        color.currentColor = ledRl.getColor();
                        buttonConfig.currentLED = modelData;
                        color.open()
                    }
                    id: colorBt2
                }
            }
        }

    }
    ColumnLayout {
        visible: scanner.isGraphical
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
                        var newSource = scanner.findElement(image.base, image.width, image.height, mouseX, mouseY);
                        if (newSource === "") {
                            buttonConfig.visible = false;
                            image2.source = "";
                        } else if (!image2.source.toString().endsWith(newSource)) {
                            buttonConfig.visible = false;
                            image2.source  = newSource;
                            buttonConfig.visible = true;
                            var a = mapToItem(column, mouseX, mouseY);
                            buttonConfig.x = a.x-buttonConfig.width/2;
                            buttonConfig.y = a.y;
                            buttonConfig.loadImage(image2.source);
                        }


                    }
                }
            }
            Image {
                x: (applicationWindow.width-image.paintedWidth)/2
                y: (applicationWindow.height-image.paintedHeight)/2
                id: image2
                visible: true
                fillMode: Image.PreserveAspectFit
                Layout.maximumHeight: applicationWindow.height/2
                Layout.maximumWidth: applicationWindow.width/2
                sourceSize.width: applicationWindow.width/2
            }
            ColorOverlay {
                id: overlay
                visible: true
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
                model: Defines.fillCombobox("fret_mode")
                Binding { target: fretBox; property: "currentIndex"; value: fretBox.model.findIndex(s => s.value === scanner.selected.ledType) }

                onCurrentIndexChanged: scanner.selected.ledType = fretBox.model[fretBox.currentIndex].value
            }
            Label {
                text: qsTr("LED Order (drag to change)")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            ListView {
                visible: scanner.selected.hasAddressableLEDs
                Layout.fillWidth: true
                Layout.fillHeight: true
                id: listView
                model: scanner.selected.leds
                orientation: ListView.Horizontal
                delegate: DraggableItem {
                    label: buttonConfig.allLabels[modelData]
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
                model: Defines.fillCombobox("tilt")
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
                model: Defines.fillCombobox("gyro")
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

    ColorDialog {
        id: color
        onCurrentColorChanged: {
            var result = /^#?([a-f\d]{2}[a-f\d]{2}[a-f\d]{2})$/i.exec(color.currentColor);
            ledhandler.color = parseInt(result[1],16);
        }
        onAccepted: {
            if (ghLedRp.currentOption) {
                var ghColours = scanner.selected.ghColours;
                ghColours[ghLedRp.currentOption] = ledhandler.color;
                ledhandler.color = 0;
                scanner.selected.ghColours = ghColours;
                ghLedRp.currentOption = "";
            } else {
                var colours = scanner.selected.colours;
                colours[buttonConfig.currentLED] = ledhandler.color;
                ledhandler.color = 0;
                scanner.selected.colours = colours;
            }
            scanner.selected.saveLEDs();
        }
        onRejected: {
            ledhandler.color = 0;
        }
    }
    Dialog {
        id: buttonConfig
        property var current: PinInfo.bindings[scanner.selected.boardImage];
        property var allLabels: PinInfo.getLabels(scanner.selected.isGuitar, scanner.selected.isWii, scanner.selected.isLiveGuitar, scanner.selected.isRB, scanner.selected.isDrum);
        property var isAnalog: false;
        property var button;
        property var label: "";
        property var pinLabel;
        property var pin;
        property var existingPin: ""
        property var existingCurrentPin: ""
        property var currentPin: "";
        property var currentValue: "";
        property var waitingForDigital: false;
        property var waitingForAnalog: false;
        property var buttons: [];
        property var labels: [];
        property var pinLabels: [];
        property var keyLabels: [];
        property var currentLED: "";
        property var currentKey: "";
        property var existingCurrentKey: ""
        property var existingKey: "";
        property var currentKeyValue: "";
        Binding { target: buttonConfig; property: "labels"; value: buttonConfig.buttons.map(b => buttonConfig.allLabels[b]) }
        Binding { target: buttonConfig; property: "pinLabels"; value: buttonConfig.buttons.map(b => buttonConfig.current[scanner.selected.pins[b]] || scanner.selected.pins[b]) }
        Binding { target: buttonConfig; property: "keyLabels"; value: buttonConfig.buttons.map(b => KeyInfo.getKeyName(scanner.selected.keys[b])) }
        function loadImage(image) {
            button = image.toString().replace(/.*\/g(.*).svg/,"$1");
            buttons = [button];
            isAnalog = scanner.selected.pin_inverts.hasOwnProperty(button);
            label = allLabels[button];
            if (button === "strum") {
                label = "Strum";
                buttons = ["up","down"];
            }
            if (button === "ljoy") {
                label = "Left Joystick";
                buttons = ["l_x", "l_y"];
                isAnalog = true;
            }
            if (button === "rjoy") {
                label = "Right Joystick";
                buttons = ["r_x", "r_y"];
                isAnalog = true;
            }
        }
        title: label || ""
        ColumnLayout {
            Repeater {
                model: buttonConfig.buttons
                ColumnLayout {
                    id:buttonConfigButton
                    function getColor() {
                        if (!scanner.selected.colours[modelData]) return "";
                        return "#"+(scanner.selected.colours[modelData]).toString(16).padStart(6,"0");
                    }
                    Button {
                        visible: scanner.selected.inputType === ArdwiinoDefinesValues.DIRECT || scanner.selected.isGuitar
                        text: qsTr("Change Pin Binding for "+buttonConfig.labels[index]+" (Currently: "+buttonConfig.pinLabels[index]+")")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        onClicked: {
                            buttonConfig.currentValue = scanner.selected.pins[modelData];
                            buttonConfig.currentPin = modelData
                        }
                    }
                    RowLayout {
                        visible: buttonConfig.isAnalog
                        Label {
                            text: qsTr("Invert "+buttonConfig.labels[index])
                            fontSizeMode: Text.FixedSize
                            verticalAlignment: Text.AlignVCenter
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            wrapMode: Text.WordWrap
                        }
                        Switch {
                            enabled: buttonConfig.isAnalog
                            checked: !!scanner.selected.pin_inverts[modelData]
                            onCheckedChanged: {
                                var pins = scanner.selected.pin_inverts;
                                pins[modelData] = checked;
                                scanner.selected.pin_inverts = pins;
                            }
                        }
                    }
                    Button {
                        visible: scanner.selected.isKeyboard
                        text: qsTr("Change Key Binding for "+buttonConfig.labels[index]+" (Currently: "+buttonConfig.keyLabels[index]+")")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        onClicked: {
                            buttonConfig.currentKey = modelData;
                            buttonConfig.currentKeyValue = scanner.selected.keys[modelData];
                            keyListener.forceActiveFocus();
                        }
                    }
                    RowLayout {
                        visible: scanner.selected.hasAddressableLEDs
                        Label {
                            text: qsTr("Enable LEDs for Pin")
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
                        Button {
                            enabled: scanner.selected.leds.includes(modelData)
                            visible: scanner.selected.hasAddressableLEDs
                            text: "Set LED colour"
                            onClicked: {
                                ledhandler.color = scanner.selected.colours[modelData];
                                color.currentColor = buttonConfigButton.getColor();
                                buttonConfig.currentLED = modelData;
                                color.open()
                            }
                            id: colorBt
                        }
                        Rectangle {
                            color: buttonConfigButton.getColor()
                            width: colorBt.height
                            height: colorBt.height
                        }
                    }


                }
            }
            RowLayout {
                visible: buttonConfig.button === "ljoy" && (scanner.selected.isGuitar || scanner.selected.isDrum)
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
                visible: buttonConfig.button === "ljoy" && (scanner.selected.isGuitar || scanner.selected.isDrum)
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
                visible: buttonConfig.isAnalog && (scanner.selected.isKeyboard || scanner.selected.hasAddressableLEDs)
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
                visible: ["back","start","home"].includes(buttonConfig.button)
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
                    onCheckedChanged: {
                        scanner.selected.mapStartSelectHome = checked
                    }
                }
            }
            ComboBox {
                visible: scanner.selected.isMIDI
                id: midiBox
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Defines.fillCombobox("midi_type")
                Binding { target: midiBox; property: "currentIndex"; value: {midiBox.model.findIndex(s => s.value === scanner.selected.midi_type[buttonConfig.button])} }

                onCurrentIndexChanged: {
                }
            }
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
                
                onCurrentIndexChanged: {
                    scanner.selected.type = comboBox.model[comboBox.currentIndex].value
                    //When the controller type is changed, we need to disable any pins that are not used by that controller.
                    var pins = scanner.selected.pins;
                    for (let pin of PinInfo.getUnused(scanner.selected.isGuitar, scanner.selected.isWii, scanner.selected.isLiveGuitar, scanner.selected.isRB)) {
                        pins[pin] = 0xFF;
                    }
                    scanner.selected.pins = pins;
                }
            }
        }
    }


    Dialog {
        id: keyOverrideDialog
        title: "Key Conflict"
        visible: buttonConfig.existingKey
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            var pins = scanner.selected.keys;
            pins[buttonConfig.existingCurrentKey] = buttonConfig.currentKeyValue;
            pins[buttonConfig.existingKey] = 0xFF;
            scanner.selected.keys = pins;
            buttonConfig.existingKey = "";
            scanner.selected.saveKeys();
        }
        onRejected: buttonConfig.existingKey = "";
        ColumnLayout {
            Label {
                text: "Key "+KeyInfo.getKeyName(buttonConfig.currentKeyValue)+" is already in use."
            }
            Label {
                text: "Would you like to replace it?"
            }
        }
    }

    Dialog {
        id: keyDialog
        title: "Binding: "+KeyInfo.labels[buttonConfig.currentKey]
        visible: buttonConfig.currentKey
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        modal: true
        onAccepted: {
            var pins = scanner.selected.keys;
            if (pins[buttonConfig.currentKey] === buttonConfig.currentKeyValue) return;
            if (buttonConfig.currentKeyValue !== 0xFF) {
                //We need to make sure we compare numbers, as we have qt enums here for keys, and those are not directly equivilant
                var existing = Object.keys(pins).find(m => Number(pins[m]) === Number(buttonConfig.currentKeyValue));
                if (existing) {
                    buttonConfig.existingKey = existing;
                    buttonConfig.existingCurrentKey = buttonConfig.currentKey;
                    buttonConfig.currentKey = "";
                    return;
                }
            }

            pins[buttonConfig.currentKey] = buttonConfig.currentKeyValue;
            scanner.selected.keys = pins;
            scanner.selected.saveKeys();
            buttonConfig.currentKey = "";
        }
        onRejected: {
            buttonConfig.currentKey = "";}
        ColumnLayout {
            Label {
                text: qsTr("Press a key to assign it to " + KeyInfo.labels[buttonConfig.currentKey])
            }
            Label {
                text: qsTr("Current Key: " + KeyInfo.getKeyName(buttonConfig.currentKeyValue))
            }
            RowLayout {
                Button {
                    text: qsTr("Save Key")
                    onClicked: keyDialog.accept()
                }
                Button {
                    onClicked: {
                        buttonConfig.currentKeyValue = 0xFF;
                        keyDialog.accept();
                    }
                    text: qsTr("Disable Key")
                }
                Button {
                    onClicked: keyDialog.reject()
                    text: qsTr("Cancel")
                }
            }
        }
    }
    Dialog {
        id: overrideDialog
        title: "Pin Conflict"
        visible: buttonConfig.existingPin
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            var pins = scanner.selected.pins;
            pins[buttonConfig.existingCurrentPin] = buttonConfig.currentValue;
            pins[buttonConfig.existingPin] = 0xFF;
            scanner.selected.pins = pins;
            scanner.selected.savePins();
            buttonConfig.currentPin = "";
        }
        onRejected: buttonConfig.existingPin = "";
        ColumnLayout {
            Label {
                function getBinding() {
                    var bindings = PinInfo.bindings[scanner.selected.boardImage];
                    return bindings[buttonConfig.currentValue] || buttonConfig.currentValue;
                }
                text: "Pin "+getBinding()+" is already in use."
            }
            Label {
                text: "Would you like to replace it?"
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
        id: waitingDigitalDialog
        title: "Waiting for a pin"
        visible: buttonConfig.waitingForDigital
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        closePolicy: Popup.NoAutoClose
        ColumnLayout {
            Label {
                text: "Ground the pin you would like to assign to "+buttonConfig.allLabels[buttonConfig.currentPin]
            }
        }
    }
    
    Dialog {
        id: waitingAnalogDialog
        title: "Waiting for a pin"
        visible: buttonConfig.waitingForAnalog
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        closePolicy: Popup.NoAutoClose
        ColumnLayout {
            Label {
                text: "Move an axis to assign it to "+buttonConfig.allLabels[buttonConfig.currentPin]
            }
        }
    }
    
    Dialog {
        id: pinDialog
        title: "Select a Pin for: "+buttonConfig.allLabels[buttonConfig.currentPin]
        visible: buttonConfig.currentPin
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        onAccepted: {
            var pins = scanner.selected.pins;
            if (pins[buttonConfig.currentPin] === buttonConfig.currentValue) return;
            if (buttonConfig.currentValue !== 0xFF) {
                var existing = Object.keys(pins).find(m => pins[m] === buttonConfig.currentValue);
                if (existing) {
                    buttonConfig.existingPin = existing;
                    buttonConfig.existingCurrentPin = buttonConfig.currentPin;
                    buttonConfig.currentPin = "";
                    return;
                }
            }
            pins[buttonConfig.currentPin] = buttonConfig.currentValue;
            scanner.selected.pins = pins;
            scanner.selected.savePins();
            buttonConfig.currentPin = "";
        }
        onRejected: buttonConfig.currentPin = "";
        modal: true
        ColumnLayout {
            anchors.fill: parent
            Image {
                sourceSize.width: applicationWindow.width/3
                property var selected: PinInfo.pinLocations[scanner.selected.boardImage];
                property var scaleX: 1 / selected.width * paintedWidth
                property var scaleY: 1 / selected.height * paintedHeight
                property var startX: (width - paintedWidth) / 2
                property var startY: (height - paintedHeight) / 2
                property var r: selected.r * scaleX
                property var pins: selected.pins
                id: boardImage
                Layout.alignment: Qt.AlignHCenter
                source: scanner.selected.boardImage
                fillMode: Image.PreserveAspectFit
                Layout.maximumHeight: applicationWindow.height/3
                Layout.maximumWidth: applicationWindow.width/3
                Repeater {
                    model: boardImage.pins.length
                    Rectangle {
                        visible: !buttonConfig.isAnalog || PinInfo.checkValid(boardImage.pins[index].id, scanner.selected.boardImage)
                        width: boardImage.r; height: boardImage.r
                        x: boardImage.startX + boardImage.pins[index].x * boardImage.scaleX
                        y: boardImage.startY + boardImage.pins[index].y * boardImage.scaleY
                        radius: boardImage.r * 0.5
                        border.width: 1
                        color: (buttonConfig.currentValue === boardImage.pins[index].id || mouseArea.containsMouse) ? "green" : Object.values(scanner.selected.pins).includes(boardImage.pins[index].id)?"red":"yellow"
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: buttonConfig.currentValue = boardImage.pins[index].id
                        }
                        ToolTip.visible: mouseArea.containsMouse && Object.values(scanner.selected.pins).includes(boardImage.pins[index].id)
                        ToolTip.text: buttonConfig.allLabels[Object.keys(scanner.selected.pins)[Object.values(scanner.selected.pins).indexOf(boardImage.pins[index].id)]] || ""
                    }
                }
            }
        }
        footer: RowLayout {
            id: test
            function received(i) {
                buttonConfig.currentValue = i;
                buttonConfig.waitingForAnalog = false;
                buttonConfig.waitingForDigital = false;
            }
            
            Button {
                text: qsTr("Automatically Find Pin Binding")
                Layout.fillWidth: true
                visible: scanner.selected.hasAutoBind
                onClicked: {
                    var isAnalog = scanner.selected.pin_inverts.hasOwnProperty(buttonConfig.currentPin);
                    //The tilt pin is weird, as it is sometimes analog and sometimes digital..
                    if (buttonConfig.allLabels[buttonConfig.currentPin] === "Tilt Axis") {
                        isAnalog = scanner.selected.tiltType === ArdwiinoDefinesValues.ANALOGUE;
                    }
                    if (isAnalog) {
                        scanner.selected.findAnalog(test.received)
                        buttonConfig.waitingForAnalog = true;
                    } else {
                        scanner.selected.findDigital(test.received)
                        buttonConfig.waitingForDigital = true;
                    }
                }
            }
            Button {
                text: qsTr("Set Pin Binding")
                Layout.fillWidth: true
                onClicked: pinDialog.accept()
            }
            
            Button {
                text: qsTr("Disable Pin Binding")
                Layout.fillWidth: true
                onClicked: {
                    buttonConfig.currentValue = 0xFF;
                    pinDialog.accept();
                }
            }
            
            Button {
                text: qsTr("Cancel")
                Layout.fillWidth: true
                onClicked: pinDialog.reject()
            }
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
            Repeater {
                id: ghLedRp
                property var currentOption;
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Object.keys(scanner.selected.ghColours)
                RowLayout {
                    visible: scanner.selected.hasAddressableLEDs
                    id: ghLedRl
                    Label {
                        text: qsTr("Led Config: "+modelData)
                        fontSizeMode: Text.FixedSize
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        wrapMode: Text.WordWrap
                    }
                    function getColor() {
                        return "#"+(scanner.selected.ghColours[modelData]).toString(16).padStart(6,"0");
                    }
                    Layout.row: index+1
                    Layout.column: gl.columns-1
                    Switch {
                        Component.onCompleted: {
                            switch (modelData) {
                            case "Note Hit": checked = ledhandler.hitEnabled; break;
                            case "Open Note": checked = ledhandler.openEnabled; break;
                            case "Star Power": checked = ledhandler.starPowerEnabled; break;
                            }
                        }

                        onCheckedChanged: {
                            switch (modelData) {
                            case "Note Hit": ledhandler.hitEnabled = checked; break;
                            case "Open Note": ledhandler.openEnabled = checked; break;
                            case "Star Power": ledhandler.starPowerEnabled = checked; break;
                            }
                        }
                    }
                    Rectangle {
                        radius: colorBt3.height
                        visible: scanner.selected.hasAddressableLEDs
                        color: ghLedRl.getColor()
                        width: colorBt3.height
                        height: colorBt3.height
                    }
                    Button {
                        visible: scanner.selected.hasAddressableLEDs
                        text: "Change colour"
                        onClicked: {
                            ghLedRp.currentOption = modelData
                            ledhandler.color = scanner.selected.ghColours[modelData];
                            color.currentColor = ghLedRl.getColor();
                            color.open()
                        }
                        id: colorBt3
                    }
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
