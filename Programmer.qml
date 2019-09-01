import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Universal 2.13

Page {
    id: page
    Image {
        id: image
        x: 786
        width: 400
        anchors.horizontalCenterOffset: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 510
        anchors.top: parent.top
        anchors.topMargin: 170
        anchors.horizontalCenter: parent.horizontalCenter
        source: "images/controller.png"
        fillMode: Image.PreserveAspectFit
    }

    Label {
        id: title
        x: 646
        width: 802
        text: qsTr("Arduino Programmer")
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        wrapMode: Text.WordWrap
        font.bold: true
        anchors.top: parent.top
        anchors.topMargin: 510
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 510
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 30
        fontSizeMode: Text.FixedSize
    }


    Column {
        id: column1
        x: 591
        y: 567
        width: 200
        height: 216
        anchors.horizontalCenterOffset: 0
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom

        Column {
            visible: scanner.selected.boardName() === "micro"
            id: micro
            width: 200
            height: 120
            anchors.topMargin: 12

            Label {
                id: label
                text: qsTr("An Arduino Pro Micro was detected.")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                id: label1
                text: qsTr("Please select the voltage your Pro Micro is running at")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                id: rectangle
                width: 200
                height: 20
                color: "#00000000"
                border.color: "#00000000"
            }

            ComboBox {
                textRole: "key"
                id: comboBox
                anchors.horizontalCenter: parent.horizontalCenter
                model: ListModel {
                    id: model
                    ListElement { key: "3.3V"; freq: "8000000" }
                    ListElement { key: "5V"; freq: "16000000" }
                }
                onActivated: {
                    scanner.selected.setBoardFreq(model.get(currentIndex).freq)
                }
            }





        }

        Label {
            id: label2
            color: "#29262b"
            text: qsTr("Programming Progress")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ProgressBar {
            id: progressBar
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            value: 0
        }

        Rectangle {
            id: rectangle1
            width: 200
            height: 20
            color: "#00000000"
            border.color: "#00000000"
        }

        Button {
            id: button
            text: qsTr("Start Programming")
            anchors.horizontalCenter: parent.horizontalCenter
        }



    }

    ScrollView {
        id: scrollView
        contentHeight: -1
        contentWidth: -1
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.top: column1.bottom
        anchors.topMargin: 10

        TextArea {
            id: toolOutput
            text: programmer.process_out
            activeFocusOnPress: false
            readOnly: true
            wrapMode: Text.NoWrap
        }
    }














}







































































































































/*##^## Designer {
    D{i:0;autoSize:true;height:1080;width:1920}D{i:1;anchors_height:60;anchors_width:802;anchors_x:646;anchors_y:658}
D{i:2;anchors_height:400;anchors_width:802;anchors_x:646;anchors_y:232}D{i:5;anchors_y:581}
D{i:11;anchors_height:400;anchors_y:232}D{i:4;anchors_y:581}D{i:3;anchors_height:400;anchors_y:582}
D{i:17;anchors_height:157;anchors_x:234;anchors_y:531}D{i:16;anchors_height:200;anchors_width:200}
}
 ##^##*/
