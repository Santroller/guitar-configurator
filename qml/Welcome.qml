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
        spacing: 5
        anchors.fill: parent



        Item {
            id: test2
            Layout.fillHeight: true
        }
        Image {
            id: image
            Layout.alignment: Qt.AlignHCenter
            source: {
                let index = devices.currentIndex;
                if (index === -1 || !devices.model[index].ready) return "/images/NoController.svg";
                return Defines.getBoardImage(devices.model[index].type);
            }
            sourceSize.width: applicationWindow.width/3
            fillMode: Image.PreserveAspectFit
            Layout.maximumHeight: applicationWindow.height/3
        }
        Label {
            text: " "
            id: test3
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
            text: qsTr("Please connect a supported device, such as a Ardwiino, Arduino Pro Micro, Arduino Leonardo or Arduino Uno")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            fontSizeMode: Text.Fit
            wrapMode: Text.WordWrap
            font.bold: false
            font.weight: Font.DemiBold
            font.capitalization: Font.MixedCase
        }

        ColumnLayout {
            id: instructions
            spacing: 5
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

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
            text: qsTr("If you like this project and want to contribute to its development, feel free to donate using the below link")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            fontSizeMode: Text.Fit
            wrapMode: Text.WordWrap
            font.bold: false
            font.weight: Font.DemiBold
            font.capitalization: Font.MixedCase
        }
        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Button {
                text: qsTr("Sponsor Project via GitHub Sponsors")
                onClicked: Qt.openUrlExternally("https://github.com/sponsors/sanjay900")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }



        Item {
            id: test
            Layout.fillHeight: true
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
                if (scanner.selected.configurable) {
                    mainStack.replace("Configure.qml");
                } else {
                    mainStack.replace("Programmer.qml");
                }
            }
        }









    }





}



























































































































/*##^##
Designer {
    D{i:0;autoSize:true;height:1080;width:1920}
}
##^##*/
