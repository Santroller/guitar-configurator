import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import net.tangentmc 1.0


ColumnLayout {
    id: column
    property var selected: programmer.rf ? devices.model[devices.currentIndex] : scanner.selected
    Label {
        text:""
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        font.pointSize: 30
        fontSizeMode: Text.FixedSize
    }
    Label {
        text: qsTr("Arduino Programmer")
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 30
        fontSizeMode: Text.FixedSize
    }
    Image {
        id: image
        Layout.alignment: Qt.AlignHCenter
        source: "/images/Upload.svg"
        fillMode: Image.PreserveAspectFit
        sourceSize.width: applicationWindow.width/10
    }

    ColumnLayout {
        visible: programmer.rf
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
            model: scanner.modelRF
        }
    }


    ColumnLayout {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        Layout.fillWidth: true
        ColumnLayout {
            visible: selected.boardName === "generic" && (!selected.isArdwiino || !selected.ready) && programmer.status === Status.NOT_PROGRAMMING
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Label {
                text: qsTr("An unknown device was selected.")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Label {
                text: qsTr("Please select the board that you are using")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Rectangle {
                color: "#00000000"
                border.color: "#00000000"
            }

            ComboBox {
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Component.onCompleted: {
                    for (let i =0; i < model3.count; ++i) {
                        let element = model3.get(i);
                        if (element.board == selected.boardName && element.freq == selected.boardFreq) {
                            currentIndex = i;
                            return;
                        }
                    }
                    if (programmer.rf) {
                        model3.append({ key: "Arduino Pro Mini 5V", board: "mini", freq: 16000000 })
                        model3.append({ key: "Arduino Pro Mini 3.3V", board: "mini", freq: 8000000 })
                    }
                }
                model: ListModel {
                    id: model3
                    ListElement { key: "Arduino Pro Micro 3.3V"; board: "micro"; freq: 8000000 }
                    ListElement { key: "Arduino Pro Micro 5V"; board: "micro"; freq: 16000000 }
                    ListElement { key: "Arduino Micro 5V"; board: "a-micro"; freq: 16000000 }
                    ListElement { key: "Arduino Leonardo 3.3V"; board: "leonardo"; freq: 8000000 }
                    ListElement { key: "Arduino Leonardo 5V"; board: "leonardo"; freq: 16000000 }
                    ListElement { key: "Arduino Uno"; board: "uno"; freq: 16000000 }
                    ListElement { key: "Arduino Mega 2560"; board: "mega2560"; freq: 16000000 }
                    ListElement { key: "Arduino Mega ADK"; board: "megaadk"; freq: 16000000 }
                }
                onActivated: {
                    selected.setBoardType(model3.get(currentIndex).board, model3.get(currentIndex).freq);
                }
            }
        }
        ColumnLayout {
            visible: (selected.boardName === "a-micro" ||selected.boardName === "micro" || selected.boardName === "leonardo") && (!selected.isArdwiino || !selected.ready) && programmer.status === Status.NOT_PROGRAMMING
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Label {
                text: qsTr("An Arduino Pro Micro or Leonardo was detected.")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Label {
                text: qsTr("Please select the board that you are using")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Rectangle {
                color: "#00000000"
                border.color: "#00000000"
            }

            ComboBox {
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Component.onCompleted: {
                    for (let i =0; i < model.count; ++i) {
                        let element = model.get(i);
                        if (element.board == selected.boardName && element.freq == selected.boardFreq) {
                            currentIndex = i;
                            return;
                        }
                    }
                }
                model: ListModel {
                    id: model
                    ListElement { key: "Arduino Pro Micro 3.3V"; board: "micro"; freq: 8000000 }
                    ListElement { key: "Arduino Pro Micro 5V"; board: "micro"; freq: 16000000 }
                    ListElement { key: "Arduino Micro 5V"; board: "a-micro"; freq: 16000000 }
                    ListElement { key: "Arduino Leonardo 3.3V"; board: "leonardo"; freq: 8000000 }
                    ListElement { key: "Arduino Leonardo 5V"; board: "leonardo"; freq: 16000000 }
                }
                onActivated: {
                    selected.setBoardType(model.get(currentIndex).board, model.get(currentIndex).freq);
                }
            }
        }
        ColumnLayout {
            visible: (selected.boardName.includes("at90usb82") || selected.boardName.includes("atmega16u2")) && programmer.status === Status.NOT_PROGRAMMING
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Label {
                text: qsTr("An Arduino Mega or Uno was detected in DFU mode.")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Label {
                text: qsTr("Please select the board that you are using")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Rectangle {
                color: "#00000000"
                border.color: "#00000000"
            }

            ComboBox {
                Layout.fillWidth: true
                textRole: "key"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: ListModel {
                    id: modelUno
                    ListElement { key: "Arduino Uno"; board: "uno"; freq: "16000000" }
                    ListElement { key: "Arduino Mega 2560"; board: "mega2560"; freq: "16000000" }
                    ListElement { key: "Arduino Mega ADK"; board: "megaadk"; freq: "16000000" }
                }
                onActivated: {
                    selected.setBoard(modelUno.get(currentIndex).board, modelUno.get(currentIndex).freq);
                }
            }
        }
        ColumnLayout {
            visible: programmer.status === Status.DFU_CONNECT_AVRDUDE
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            id: dfu
            Label {
                id: label6
                text: qsTr("Please bridge the reset and ground pins shown below")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
            Image {
                id: dfuImg
                Layout.alignment: Qt.AlignHCenter
                source: "/images/ArduinoUnoDFU.svg"
                fillMode: Image.PreserveAspectFit
                Layout.maximumHeight: applicationWindow.height/3
                Layout.maximumWidth: applicationWindow.width/3
            }
        }
        Label {
            visible: programmer.status === Status.DFU_DISCONNECT_AVRDUDE
            id: label7
            text: qsTr("Please disconnect and reconnect your device.")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Button {
            id: button
            text: qsTr("Start Programming")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                programmer.startProgramming();
                programmer.program(selected);
            }
            visible: programmer.status === Status.NOT_PROGRAMMING
        }

        Button {
            id: cntBtn
            text: qsTr(programmer.restore?"Finish Restore":"Start Configuring")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                if (programmer.restore) {
                    mainStack.replace("Welcome.qml")
                } else {
                    mainStack.replace("Configure.qml")
                }
            }
            enabled: programmer.status === Status.COMPLETE
        }

        Label {
            id: labelStatus
            text: programmer.statusDescription
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            value: programmer.process_percent
        }

        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentHeight: -1
            contentWidth: -1
            TextArea {
                id: toolOutput
                text: programmer.process_out
                activeFocusOnPress: false
                readOnly: true
                wrapMode: Text.NoWrap
            }

        }
    }

}






























































































































































/*##^## Designer {
    D{i:0;autoSize:true;height:1080;width:1920}D{i:2;anchors_height:400;anchors_width:802;anchors_x:646;anchors_y:232}
D{i:3;anchors_height:400;anchors_y:582}D{i:11;anchors_height:400;anchors_y:232}D{i:5;anchors_y:581}
D{i:16;anchors_height:200;anchors_width:200}D{i:4;anchors_y:581}D{i:17;anchors_height:157;anchors_x:234;anchors_y:531}
D{i:1;anchors_height:60;anchors_width:802;anchors_x:646;anchors_y:658}
}
 ##^##*/
