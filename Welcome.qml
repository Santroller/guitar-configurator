import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import "defines.js" as Defines

Page {
    id: page

    ColumnLayout {
        id: column
        anchors.fill: parent

        Image {
            id: image
            Layout.alignment: Qt.AlignHCenter
            source: {
                let index = devices.currentIndex;
                if (index === -1) return "images/controller.png";
                return Defines.getBoardImage(devices.model[index].type);
            }
            fillMode: Image.PreserveAspectFit
            Layout.maximumHeight: applicationWindow.height/3
            Layout.maximumWidth: applicationWindow.height/3
        }

        Label {
            id: title
            text: qsTr("Welcome to the Ardwiino Configuration Tool")
            textFormat: Text.PlainText
            renderType: Text.QtRendering
            lineHeight: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.NoWrap
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.Fit
        }

        Label {
            id: updates
            text: updateHandler.updateInfo
            textFormat: Text.PlainText
            renderType: Text.QtRendering
            lineHeight: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.NoWrap
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.Fit
        }

        Label {
            visible: updateHandler.hasUpdate
            id: updatesAvail
            text: "An update is available!"
            textFormat: Text.PlainText
            renderType: Text.QtRendering
            lineHeight: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.NoWrap
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.Fit
        }

        Button {
            visible: updateHandler.hasUpdate
            id: updateNow
            text: qsTr("Update Application")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                updateHandler.startUpdate()
            }
        }

        Label {
            id: please
            text: qsTr("Please connect an Ardwiino controller, or an Arduino")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            fontSizeMode: Text.Fit
            wrapMode: Text.WordWrap
            font.bold: false
            font.weight: Font.DemiBold
            font.capitalization: Font.MixedCase
        }

        ColumnLayout {
            id: instructions
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Label {
                id: supported
                text: qsTr("The following Arduinos are supported:")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                fontSizeMode: Text.Fit
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                font.capitalization: Font.MixedCase
                font.weight: Font.DemiBold
                font.bold: false
            }

            Label {
                id: micro
                text: qsTr("Arduino Pro Micro")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                fontSizeMode: Text.Fit
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.capitalization: Font.MixedCase
                font.weight: Font.Light
                font.bold: false
            }

            Label {
                id: leo
                text: qsTr("Arduino Leonardo")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                fontSizeMode: Text.Fit
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                font.capitalization: Font.MixedCase
                font.weight: Font.Light
                font.bold: false
                verticalAlignment: Text.AlignVCenter
            }

            Label {
                id: uno
                text: qsTr("Arduino Uno")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                fontSizeMode: Text.Fit
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                font.capitalization: Font.MixedCase
                font.weight: Font.Light
                font.bold: false
                verticalAlignment: Text.AlignVCenter
            }

            Label {
                id: os
                text: qsTr(scanner.getOSString())
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                fontSizeMode: Text.Fit
                horizontalAlignment: Text.AlignHCenter
                font.capitalization: Font.MixedCase
                font.weight: Font.Light
                font.bold: false
                verticalAlignment: Text.AlignVCenter
                onLinkActivated: Qt.openUrlExternally(link)

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // we don't want to eat clicks on the Text
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
            Button {
                id: configureXpad
                text: qsTr("Fix controller")
                visible: scanner.isLinux()
                onClicked: scanner.fixLinux()
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        Label {
            id: label
            y: 0
            text: qsTr("Found Devices: ")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            fontSizeMode: Text.Fit
            font.weight: Font.DemiBold
        }

        ComboBox {
            id: devices
            Layout.columnSpan: 1
            Layout.rowSpan: 1
            Layout.fillWidth: true
            focusPolicy: Qt.TabFocus
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            textRole: "description"
            model: scanner.model
        }

        Button {
            id: welcomeContinue
            text: qsTr("Continue")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            enabled: devices.model[devices.currentIndex].ready
            onClicked: {
                scanner.selected = devices.model[devices.currentIndex];
                programmer.setRestoring(false);
                if (scanner.selected.isArdwiino) {
                    scanner.selected.startConfiguring();
                    mainStack.replace("Configure.qml");
                } else {
                    mainStack.replace("Programmer.qml");
                }
            }
        }









    }





}



























































































































/*##^## Designer {
    D{i:0;autoSize:true;height:1080;width:1920}
}
 ##^##*/
