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
    
    //Add some newlines so that text doesn't overlap
    Label {
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: "\n\n"
    }

    Label {
        text: qsTr("You have unwritten changes!")
        fontSizeMode: Text.FixedSize
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
        visible: scanner.selected.config.hasChanged
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
                text: qsTr("Queue based inputs")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Layout.fillWidth: true
                Layout.fillHeight: true
                checked: scanner.selected.config.deque
                onCheckedChanged: {
                    scanner.selected.config.deque = checked
                    if (checked && scanner.selected.config.debounceButtons < 5) {
                        scanner.selected.config.debounceButtons = 5;
                    }
                    if (checked && scanner.selected.config.mainPollRate < 1) {
                        scanner.selected.config.mainPollRate = 1;
                    }
                    if (!checked) {
                        scanner.selected.config.debounceButtons = Math.round(scanner.selected.config.debounceButtons / 10) * 10;
                    }
                }
            }

            Label {
                text: qsTr("Controller Poll Rate (0 means as fast as possible) (" + ((1 / Math.max(scanner.selected.config.mainPollRate, 1)) * 1000).toFixed(0) + "hz)")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                visible: !scanner.selected.config.deque
            }
            Label {
                visible: scanner.selected.config.deque
                text: qsTr("Dequeue Rate (" + ((1 / scanner.selected.config.mainPollRate) * 1000).toFixed(0) + "+ fps required)")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            SpinBox {
                Layout.fillWidth: true
                value: scanner.selected.config.mainPollRate
                from: 0
                to: 10
                visible: !scanner.selected.config.deque
                onValueModified: {
                    scanner.selected.config.mainPollRate = value
                }
            }
            SpinBox {
                Layout.fillWidth: true
                value: scanner.selected.config.mainPollRate
                from: 1
                to: 10
                visible: scanner.selected.config.deque
                onValueModified: {
                    scanner.selected.config.mainPollRate = value
                }
            }

            Label {
                text: qsTr("Button Debounce (0 means no debounce)")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                visible: !scanner.selected.config.deque
                wrapMode: Text.WordWrap
            }

            Label {
                text: qsTr("Button Debounce")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                visible: scanner.selected.config.deque
                wrapMode: Text.WordWrap
            }
            SpinBox {
                Layout.fillWidth: true
                value: scanner.selected.config.debounceButtons
                from: 0
                to: 250
                stepSize: 10
                visible: !scanner.selected.config.deque
                textFromValue: function(value, locale) {
                    return parseFloat(value*1/10).toFixed(0)
                }
                onValueModified: {
                    scanner.selected.config.debounceButtons = value
                }
            }
            SpinBox {
                Layout.fillWidth: true
                value: scanner.selected.config.debounceButtons
                from: 5
                to: 250
                stepSize: 1
                visible: scanner.selected.config.deque
                textFromValue: function(value, locale) {
                    return parseFloat(value*1/10).toFixed(1)
                }
                onValueModified: {
                    scanner.selected.config.debounceButtons = value
                }
            }

            Label {
                text: qsTr("Strum Debounce (0 means no debounce)")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            SpinBox {
                id: spinboxsDB
                Layout.fillWidth: true
                value: scanner.selected.config.debounceStrum
                from: 0
                to: 100
                stepSize: 10
                textFromValue: function(value, locale) {
                    return parseFloat(value*1/10).toFixed(0)
                }
                onValueModified: {
                    scanner.selected.config.debounceStrum = value
                }
            }

            Label {
                text: qsTr("Combine Strum Debounce")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Layout.fillWidth: true
                Layout.fillHeight: true
                checked: scanner.selected.config.debounceCombinedStrum
                onCheckedChanged: scanner.selected.config.debounceCombinedStrum = checked
            }

            Label {
                visible: scanner.selected.config.isDrum
                text: qsTr("Drum Trigger Threshold")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            SpinBox {
                visible: scanner.selected.config.isDrum
                Layout.fillWidth: true
                value: scanner.selected.config.axisDrumThreshold
                from: 0
                to: 255
                onValueModified: {
                    scanner.selected.config.axisDrumThreshold = value
                }
            }

            Label {
                visible: scanner.selected.config.mainFretLEDMode == ArdwiinoDefinesValues.APA102
                text: qsTr("Note: You can not combine Playstation input and APA102 LEDs, due to them both using imcompatible connections.")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                fontSizeMode: Text.FixedSize
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
                    scanner.clearImages();
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
                    scanner.clearImages();
                }
            }

            Label {
                text: qsTr("Map Left Joystick to DPad")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Layout.fillWidth: true
                Layout.fillHeight: true
                checked: scanner.selected.config.mainMapLeftJoystickToDPad
                onCheckedChanged: scanner.selected.config.mainMapLeftJoystickToDPad = checked
            }

            Label {
                text: qsTr("Map Start + Select to Home")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Layout.fillWidth: true
                Layout.fillHeight: true
                checked: scanner.selected.config.mainMapStartSelectToHome
                onCheckedChanged: scanner.selected.config.mainMapStartSelectToHome = checked
            }
            ColumnLayout {
                visible: scanner.selected.config.isWii
                Label {
                    text: qsTr("Map Tap Bar to Frets")
                    fontSizeMode: Text.FixedSize
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                }
                Switch {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    checked: scanner.selected.config.neckWiiNeck
                    onCheckedChanged: scanner.selected.config.neckWiiNeck = checked
                }
            }
            ColumnLayout {
                visible: scanner.selected.config.isDirect
                Label {
                    text: qsTr("Map GH5 Neck to frets")
                    fontSizeMode: Text.FixedSize
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                }
                Switch {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    checked: scanner.selected.config.neckGh5Neck
                    onCheckedChanged: {
                        scanner.selected.config.neckGh5Neck = checked
                        if (checked) {
                            scanner.selected.config.neckGh5NeckBar = false
                        }
                    }
                }
                ColumnLayout {
                    visible: scanner.selected.boardName !== "uno"
                    Label {
                        text: qsTr("Map GH5 Neck to Frets (With Tap Bar)")
                        fontSizeMode: Text.FixedSize
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        wrapMode: Text.WordWrap
                    }
                    Switch {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        checked: scanner.selected.config.neckGh5NeckBar
                        onCheckedChanged: {
                            scanner.selected.config.neckGh5NeckBar = checked
                            if (checked) {
                                scanner.selected.config.neckGh5Neck = false
                            }
                        }
                    }
                }
                Label {
                    text: qsTr("Map WT Tap Bar to Frets")
                    fontSizeMode: Text.FixedSize
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                }
                Switch {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    checked: scanner.selected.config.neckWtNeck
                    onCheckedChanged: scanner.selected.config.neckWtNeck = checked
                }
            }
        }
    }
    Dialog {
        id: rfDiag
        modal: true
        standardButtons: Dialog.Close

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        ColumnLayout {
            Label {
                id: enableRF
                text: qsTr("Enable RF")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }

            
            Switch {
                Layout.fillWidth: true
                Layout.fillHeight: true
                checked: scanner.selected.config.rfRfInEnabled
                onCheckedChanged: scanner.selected.config.rfRfInEnabled = checked
            }

            Button {
                id: startRFProg
                text: qsTr("Program RF Transmitter")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                enabled: scanner.selected.config.rfRfInEnabled
                onClicked: {
                    programmer.prepareRF(scanner.selected)
                    mainStack.replace("Programmer.qml")
                }
        }
        }
    }
    Dialog {
        id: presetLoadDialog
        modal: true
        standardButtons: Dialog.Ok | Dialog.Discard | Dialog.Cancel | Dialog.Apply | Dialog.Reset

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        ColumnLayout {
            Label {
                text: qsTr("Presets")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }

            FileDialog {
                id: saveDialog
                title: "Export preset"
                folder: shortcuts.home
                selectExisting: false
                onAccepted: {
                    scanner.selected.exportPreset(c.currentValue, saveDialog.fileUrls)
                }
                nameFilters: [ "Preset file (*.json)" ]
            }

            FileDialog {
                id: openDialog
                title: "Import preset"
                folder: shortcuts.home
                onAccepted: {
                    scanner.selected.importPreset(saveDialog.fileUrls)
                }
                nameFilters: [ "Preset file (*.json)" ]
            }

            ComboBox {
                id: c
                model: scanner.selected.presets
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
        onDiscarded: {
            scanner.selected.removePreset(c.currentValue);
        }
        onApplied: {
            saveDialog.open();
        }
        onReset: {
            openDialog.open();
        }
        Component.onCompleted: {
            presetLoadDialog.standardButton(Dialog.Ok).text = qsTrId("Load")
            presetLoadDialog.standardButton(Dialog.Discard).text = qsTrId("Delete")
            presetLoadDialog.standardButton(Dialog.Apply).text = qsTrId("Export")
            presetLoadDialog.standardButton(Dialog.Reset).text = qsTrId("Import")
        }
        onAccepted: {
            let preset = JSON.parse(scanner.selected.getPreset(c.currentValue));
            let currentKeys = Object.keys(scanner.selected.config);
            let presetKeys = Object.keys(preset);
            for (let key of presetKeys) {
                if (currentKeys.includes(key)) {
                    try {
                        scanner.selected.config[key] = preset[key];
                    } catch (ex) {

                    }
                }
            }
            for (let key of currentKeys) {
                if (!presetKeys.includes(key)) {
                    try {
                        scanner.selected.config[key] = scanner.selected.defaultConfig[key];
                    } catch (ex) {

                    }
                }
            }
        }
    }
    Dialog {
        id: presetDialog
        modal: true
        standardButtons: Dialog.Save | Dialog.Close

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        ColumnLayout {
            Label {
                text: qsTr("What would you like to name your preset")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }

            TextField {
                id: text
                placeholderText: qsTr("")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
        onAccepted: {
            if (text.text.length == 0) {
                open();
                text.forceActiveFocus()
            } else {
                scanner.selected.savePreset(text.text, JSON.stringify(scanner.selected.config))
            }
        }
    }
    
    RowLayout {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        Button {
            id: change
            text: qsTr("Change Device Settings")
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
            onClicked: {
                scanner.selected.config.hasChanged = false;
                scanner.selected.writeConfig();
            }
        }
        Button {
            text: qsTr("Revert changes")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            enabled: true
            onClicked: {
                scanner.selected.config = scanner.selected.lastConfig;
            }
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
        Button {
            id: showRFDialog
            text: qsTr("Configure RF")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            enabled: true
            onClicked: rfDiag.open()
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
        Button {
            id: reset
            text: qsTr("Reset device config")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: scanner.selected.resetConfig();
        }
        
        Button {
            Layout.alignment: Qt.AlignRight | Qt.AlignTop
            text: scanner.isGraphical?"Swap to List view":"Swap to Graphical view"
            onClicked: scanner.toggleGraphics()
        }
        
        Button {
            Layout.alignment: Qt.AlignRight | Qt.AlignTop
            text: "Save Preset"
            onClicked: presetDialog.open()
        }
        
        Button {
            Layout.alignment: Qt.AlignRight | Qt.AlignTop
            text: "Load Preset"
            onClicked: presetLoadDialog.open()
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
                visible: scanner.selected.config.mainInputType == ArdwiinoDefinesValues.PS2
                text: qsTr("Note: You can not combine Playstation input and APA102 LEDs, due to them both using imcompatible connections.")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                fontSizeMode: Text.FixedSize
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
                property var labels: PinInfo.getLabels(scanner.selected.config.isGuitar, scanner.selected.config.isWii, scanner.selected.config.isLiveGuitar, scanner.selected.config.isRB, scanner.selected.config.isDrum, scanner.selected.config.isMouse, scanner.selected.config.isDJ);
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
        property var pins: PinInfo.getBindings(scanner.selected.getDirectBoardImage());
        property var labels: PinInfo.getLabels(scanner.selected.config.isGuitar, scanner.selected.config.isWii, scanner.selected.config.isLiveGuitar, scanner.selected.config.isRB, scanner.selected.config.isDrum, scanner.selected.config.isMouse, scanner.selected.config.isDJ);
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
                text: "Pin Binding"
                visible: scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.MPU_6050 && scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.NO_TILT;
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            RowLayout {
                visible: scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.MPU_6050 && scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.NO_TILT;
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
                    ToolTip.text: tiltConfig.labels["RY"] || ""
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
                visible: scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.DIGITAL && scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.NO_TILT;
                text: "Invert Tilt Axis"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Switch {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.DIGITAL && scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.NO_TILT;
                checked: !!scanner.selected.config[`pins${"RY"}Inverted`]
                onCheckedChanged: scanner.selected.config[`pins${"RY"}Inverted`] = checked
            }

            Button {
                visible: scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.DIGITAL && scanner.selected.config.mainTiltType !== ArdwiinoDefinesValues.NO_TILT;
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                text: "Calibrate Tilt"
                enabled: true
                onClicked: {
                    calibDialog2.open()
                }
            }
            CalibrationDialog {
                id: calibDialog2
                pin: scanner.selected.config.pinsRY
                axis: "RY"
                isWhammy: false
                skipDeadzone: true
                onCalibrationChanged: {
                    scanner.selected.config.axisScaleRYMultiplier = mulFactor * 1024
                    scanner.selected.config.axisScaleRYOffset = min
                    scanner.selected.config.axisScaleRYDeadzone = deadZone
                }
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
                    currentIndex: Math.max(0,model.findIndex(s => s.value === scanner.selected.config.midiType["RY"]))
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
                    value: scanner.selected.config.midiNote["RY"] || 0
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
                    value: scanner.selected.config.midiChannel["RY"] || 0
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
