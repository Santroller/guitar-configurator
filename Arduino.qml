import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Universal 2.13
import QtQuick.Layouts 1.13
import net.tangentmc 1.0

Page {
    id: page
    ColumnLayout {
        id: column
        anchors.fill: parent
        Image {
            id: image
            Layout.alignment: Qt.AlignHCenter
            source: scanner.selected.boardImage
            fillMode: Image.PreserveAspectFit
            Layout.maximumHeight: applicationWindow.height/3
            Layout.maximumWidth: applicationWindow.width/3
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Button {
                id: configureContinue
                text: qsTr("Set Pin")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {

        //                scanner.selected.writeConfig();
                    mainStack.pop();
                }
            }


            Button {
                id: cancel
                text: qsTr("Cancel")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    mainStack.pop();
                }
            }
        }


    }
}



















































































/*##^##
Designer {
    D{i:0;autoSize:true;height:1080;width:1920}D{i:2;anchors_height:400;anchors_y:232}
D{i:3;anchors_y:774}D{i:1;anchors_height:60;anchors_width:802;anchors_x:646;anchors_y:658}
}
##^##*/
