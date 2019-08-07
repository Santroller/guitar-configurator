import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Universal 2.13

Page {
    id: page
    Label {
        id: title
        x: 646
        width: 802
        text: qsTr("Configuration")
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

    Button {
        id: welcomeContinue
        x: 910
        text: qsTr("Continue")
        anchors.top: title.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: true
        onClicked: mainStack.push()
    }





}

















































































/*##^## Designer {
    D{i:0;autoSize:true;height:1080;width:1920}D{i:1;anchors_height:60;anchors_width:802;anchors_x:646;anchors_y:658}
D{i:2;anchors_height:400;anchors_y:232}D{i:3;anchors_y:774}
}
 ##^##*/
