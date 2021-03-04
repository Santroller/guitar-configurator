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
    id: pinDialog
    property var pins: PinInfo.getBindings(scanner.selected.getDirectBoardImage());
    property var labels: PinInfo.getLabels(scanner.selected.config.isGuitar, scanner.selected.config.isWii, scanner.selected.config.isLiveGuitar, scanner.selected.config.isRB, scanner.selected.config.isDrum, scanner.selected.config.isMouse);
    property var currentPin: "";
    property var conflictingPin: "";
    property var currentValue: 0;
    property var isAnalog;
    property bool waitingForAnalog;
    property bool waitingForDigital;
    function getPins() {
        return Object.keys(pinDialog.labels).map(pin=>scanner.selected.config[`pins${pin}`]);
    }
    title: "Select a Pin for: "+labels[currentPin]
    x: (parent.width - width) / 2
    y: {
        //Base position on the parent position, but bump the dialog up if it hits the bottom of the screen.
        var y = (parent.height - height) / 2;
        var globalYTop = parent.mapToGlobal(x-width/2,y-height/2).y;
        var globalYBot = parent.mapToGlobal(x+width,y+height).y;
        if (globalYBot > applicationWindow.height) {
            y -= globalYBot-applicationWindow.height;
        }
        if (globalYTop < 0) {
            y -= globalYTop;
        }
        return y;
    }

    onOpened: currentValue = scanner.selected.config[`pins${currentPin}`]
    onRejected: currentValue = scanner.selected.config[`pins${currentPin}`]
    onAccepted: scanner.selected.config[`pins${currentPin}`] = currentValue

    modal: true
    ColumnLayout {
        anchors.fill: parent
        Dialog {
            id: overrideDialog
            title: "Pin Conflict"
            modal: true
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            standardButtons: Dialog.Ok | Dialog.Cancel
            onAccepted: {
                scanner.selected.config[`pins${pinDialog.conflictingPin}`] = 0xFF
                pinDialog.accept()
            }
            ColumnLayout {
                Label {
                    text: "Pin "+pinDialog.pins(pinDialog.currentValue)+" is already in use."
                }
                Label {
                    text: "Would you like to replace it?"
                }
            }
        }
        Dialog {
            id: waitingDigitalDialog
            title: "Waiting for a pin"
            visible: pinDialog.waitingForDigital
            modal: true
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            standardButtons: Dialog.Cancel
            ColumnLayout {
                Label {
                    text: "Toggle the pin you would like to assign to "+pinDialog.labels[pinDialog.currentPin]
                }
            }
            onRejected: {
                pinDialog.waitingForDigital = false;
                scanner.selected.cancelFind();
            }
        }

        Dialog {
            id: waitingAnalogDialog
            title: "Waiting for a pin"
            visible: pinDialog.waitingForAnalog
            modal: true
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            ColumnLayout {
                Label {
                    text: "Move an axis to assign it to "+pinDialog.labels[pinDialog.currentPin]
                }
            }
            onRejected: {
                pinDialog.waitingForAnalog = false;
                scanner.selected.cancelFind();
            }
        }
        Image {
            sourceSize.width: applicationWindow.width/3
            property var selected: PinInfo.pinLocations[scanner.selected.getDirectBoardImage()];
            property var scaleX: 1 / selected.width * paintedWidth
            property var scaleY: 1 / selected.height * paintedHeight
            property var startX: (width - paintedWidth) / 2
            property var startY: (height - paintedHeight) / 2
            property var r: selected.r * scaleX
            property var pins: selected.pins
            id: boardImage
            Layout.alignment: Qt.AlignHCenter
            source: scanner.selected.getDirectBoardImage()
            fillMode: Image.PreserveAspectFit
            Layout.maximumHeight: applicationWindow.height/3
            Layout.maximumWidth: applicationWindow.width/3
            Repeater {
                model: boardImage.pins.length
                Rectangle {
                    visible: !pinDialog.isAnalog || PinInfo.checkValid(boardImage.pins[index].id, scanner.selected.getDirectBoardImage())
                    width: boardImage.r; height: boardImage.r
                    x: boardImage.startX + boardImage.pins[index].x * boardImage.scaleX
                    y: boardImage.startY + boardImage.pins[index].y * boardImage.scaleY
                    radius: boardImage.r * 0.5
                    border.width: 1
                    color: (pinDialog.currentValue === boardImage.pins[index].id || mouseArea.containsMouse) ? "green" : pinDialog.getPins().includes(boardImage.pins[index].id)?"red":"yellow"
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: pinDialog.currentValue = boardImage.pins[index].id
                    }
                    ToolTip.visible: mouseArea.containsMouse && pinDialog.getPins().includes(boardImage.pins[index].id)
                    ToolTip.text: Object.values(pinDialog.labels)[pinDialog.getPins().indexOf(boardImage.pins[index].id)] || ""
                }
            }
        }
    }
    footer: RowLayout {
        id: test

        Button {
            text: qsTr("Automatically Find Pin Binding")
            Layout.fillWidth: true
            function received(i) {
                pinDialog.currentValue = i;
                pinDialog.waitingForAnalog = false;
                pinDialog.waitingForDigital = false;
            }
            onClicked: {
                var isAnalog = scanner.selected.config.hasOwnProperty(`pins${pinDialog.currentPin}Inverted`);
                //The tilt pin is weird, as it is sometimes analog and sometimes digital..
                if (pinDialog.labels[pinDialog.currentPin] === "Tilt Axis") {
                    isAnalog = scanner.selected.config.tiltType === ArdwiinoDefinesValues.ANALOGUE;
                }
                scanner.selected.startFind();
                if (isAnalog) {
                    scanner.selected.findAnalog(received)
                    pinDialog.waitingForAnalog = true;
                } else {
                    scanner.selected.findDigital(received)
                    pinDialog.waitingForDigital = true;
                }
            }
            ToolTip.visible: hovered && scanner.selected.boardName === "uno"
            ToolTip.text: "Note that automatic pin detection does not work with pin 13"
        }

        Button {
            text: qsTr("Disable Pin Binding")
            Layout.fillWidth: true
            onClicked: {
                pinDialog.currentValue = 0xFF;
                pinDialog.accept();
            }
        }
        Button {
            text: qsTr("Apply Changes")
            Layout.fillWidth: true
            onClicked: {
                if (scanner.selected.config[`pins${currentPin}`] !== currentValue && currentValue !== 0xFF) {
                    //If there is a conflict, show override dialog
                    conflictingPin = Object.keys(pinDialog.labels)[pinDialog.getPins().indexOf(currentValue)];
                    if (conflictingPin) {
                        overrideDialog.open();
                        return;
                    }
                }
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
