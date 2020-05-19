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
    property var pins: PinInfo.getBindings(scanner.selected.boardImage);
    property var labels: PinInfo.getLabels(scanner.selected.isGuitar, scanner.selected.isWii, scanner.selected.isLiveGuitar, scanner.selected.isRB, scanner.selected.isDrum);
    property var currentPin: "";
    property var conflictingPin: "";
    property var currentValue: 0;
    property var isAnalog;
    property bool waitingForAnalog;
    property bool waitingForDigital;
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

    onOpened: {
        currentValue = scanner.selected.pins[currentPin]
    }
    onRejected: currentValue = scanner.selected.pins[currentPin]
    onAccepted: {
        var pins = scanner.selected.pins;
        pins[currentPin] = currentValue;
        scanner.selected.pins = pins;
    }

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
                var pins = scanner.selected.pins;
                pins[pinDialog.conflictingPin] = 0xFF;
                scanner.selected.pins = pins;
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
            closePolicy: Popup.NoAutoClose
            ColumnLayout {
                Label {
                    text: "Ground the pin you would like to assign to "+pinDialog.labels[pinDialog.currentPin]
                }
            }
        }

        Dialog {
            id: waitingAnalogDialog
            title: "Waiting for a pin"
            visible: pinDialog.waitingForAnalog
            modal: true
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            closePolicy: Popup.NoAutoClose
            ColumnLayout {
                Label {
                    text: "Move an axis to assign it to "+pinDialog.labels[pinDialog.currentPin]
                }
            }
        }
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
                    visible: !pinDialog.isAnalog || PinInfo.checkValid(boardImage.pins[index].id, scanner.selected.boardImage)
                    width: boardImage.r; height: boardImage.r
                    x: boardImage.startX + boardImage.pins[index].x * boardImage.scaleX
                    y: boardImage.startY + boardImage.pins[index].y * boardImage.scaleY
                    radius: boardImage.r * 0.5
                    border.width: 1
                    color: (pinDialog.currentValue === boardImage.pins[index].id || mouseArea.containsMouse) ? "green" : Object.values(scanner.selected.pins).includes(boardImage.pins[index].id)?"red":"yellow"
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: pinDialog.currentValue = boardImage.pins[index].id
                    }
                    ToolTip.visible: mouseArea.containsMouse && Object.values(scanner.selected.pins).includes(boardImage.pins[index].id)
                    ToolTip.text: pinDialog.labels[Object.keys(scanner.selected.pins)[Object.values(scanner.selected.pins).indexOf(boardImage.pins[index].id)]] || ""
                }
            }
        }
    }
    footer: RowLayout {
        id: test
        function received(i) {
            pinDialog.currentValue = i;
            pinDialog.waitingForAnalog = false;
            pinDialog.waitingForDigital = false;
        }

        Button {
            text: qsTr("Automatically Find Pin Binding")
            Layout.fillWidth: true
            onClicked: {
                var isAnalog = scanner.selected.pin_inverts.hasOwnProperty(pinDialog.currentPin);
                //The tilt pin is weird, as it is sometimes analog and sometimes digital..
                if (pinDialog.labels[pinDialog.currentPin] === "Tilt Axis") {
                    isAnalog = scanner.selected.tiltType === ArdwiinoDefinesValues.ANALOGUE;
                }
                if (isAnalog) {
                    scanner.selected.findAnalog(test.received)
                    pinDialog.waitingForAnalog = true;
                } else {
                    scanner.selected.findDigital(test.received)
                    pinDialog.waitingForDigital = true;
                }
            }
            ToolTip.visible: hovered && scanner.selected.boardShortName() === "uno"
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
                if (scanner.selected.pins[currentPin] !== currentValue && currentValue !== 0xFF) {
                    //If there is a conflict, show override dialog
                    conflictingPin = Object.keys(scanner.selected.pins).find(m => scanner.selected.pins[m] === currentValue);
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
