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
    id: dialog
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
        CloneHeroDialogColorRow {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Component.onCompleted: {
                color = ledhandler.openColor;
                checked = ledhandler.openEnabled;
            }
            onCheckedChanged: ledhandler.openEnabled = checked
            onColorChanged: ledhandler.openColor = color
            name: "Open Note"
        }
        CloneHeroDialogColorRow {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Component.onCompleted: {
                color = ledhandler.starPowerColor;
                checked = ledhandler.starPowerEnabled;
            }
            onCheckedChanged: ledhandler.starPowerEnabled = checked
            onColorChanged: ledhandler.starPowerColor = color
            name: "Star Power"
        }

    }
}

