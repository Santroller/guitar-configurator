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
    property var pins: PinInfo.getBindings(scanner.selected.getDirectBoardImage());
    title: "Clone Hero Connector"
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    standardButtons: Dialog.Close
    modal: true
    ColumnLayout {
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

        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Label {
                text: qsTr("CloneHero Version")
                fontSizeMode: Text.FixedSize
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            ComboBox {
                id: versions
                Layout.columnSpan: 1
                Layout.rowSpan: 1
                Layout.fillWidth: true
                focusPolicy: Qt.TabFocus
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: ledhandler.supportedVersions
                Binding { target: versions; property: "currentIndex"; value: versions.model.indexOf(ledhandler.version)}
                onActivated: ledhandler.version = currentValue
            }
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
            visible: scanner.selected.config.mainFretLEDMode == ArdwiinoDefinesValues.APA102
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
            visible: scanner.selected.config.mainFretLEDMode == ArdwiinoDefinesValues.APA102
            name: "Star Power"
        }
        CloneHeroDialogColorRow {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Component.onCompleted: {
                color = ledhandler.starPowerPhraseColor;
                checked = ledhandler.starPowerPhraseEnabled;
            }
            onCheckedChanged: ledhandler.starPowerEnabled = checked
            onColorChanged: ledhandler.starPowerColor = color
            visible: scanner.selected.config.mainFretLEDMode == ArdwiinoDefinesValues.APA102
            name: "Star Power Phrase"
        }
        PinBinding {
            id: pinBinding
            currentPin: "SP"
        }
        Button {
            id: bindSP
            text: qsTr("Set Star Power LED pin (Currently: "+dialog.pins(scanner.selected.config.pinsSP)+")")
            enabled: ledhandler.ready
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: pinBinding.open();
        }

    }
}

