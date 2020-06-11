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
    id: keyDialog
    property var currentKey;
    property var currentValue;
    property var existingKey;
    title: "Binding: "+KeyInfo.labels[currentKey]
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    modal: true
    onAccepted: {
        var keys = scanner.selected.keys;
        keys[currentKey] = currentValue;
        scanner.selected.keys = keys;
    }

    onRejected: currentValue = scanner.selected.keys[currentKey]
    onOpened: currentValue = scanner.selected.keys[currentKey]
    ColumnLayout {
        Dialog {
                id: keyOverrideDialog
                title: "Key Conflict"
                x: (parent.width - width) / 2
                y: (parent.height - height) / 2
                modal: true
                standardButtons: Dialog.Ok | Dialog.Cancel
                onAccepted: {
                    var keys = scanner.selected.keys;
                    keys[keyDialog.existingKey] = 0xFF;
                    scanner.selected.keys = keys;
                    keyDialog.accept();

                }
                ColumnLayout {
                    Label {
                        text: "Key "+KeyInfo.getKeyName(keyDialog.currentValue)+" is already in use."
                    }
                    Label {
                        text: "Would you like to replace it?"
                    }
                }
            }
        Label {
            text: qsTr("Press a key to assign it to " + KeyInfo.labels[keyDialog.currentKey])

            focus: true
            onVisibleChanged: forceActiveFocus()
            Keys.onPressed: {
                currentValue = KeyInfo.findValueForEvent(event);
                event.accepted = true;
            }
        }
        Label {
            text: qsTr("Current Key: " + KeyInfo.getKeyName(keyDialog.currentValue))
        }
        RowLayout {
            Button {
                text: qsTr("Save Key")
                onClicked: {
                    if (keyDialog.currentValue !== 0xFF) {
                        //We need to make sure we compare numbers, as we have qt enums here for keys, and those are not directly equivilant
                        keyDialog.existingKey = Object.keys(scanner.selected.keys).find(m => Number(scanner.selected.keys[m]) === Number(keyDialog.currentValue));
                        if (keyDialog.existingKey) {
                            keyOverrideDialog.open();
                            return;
                        }
                    }
                    keyDialog.accept()
                }
            }
            Button {
                onClicked: {
                    keyDialog.currentValue = 0xFF;
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
