import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 1280
    height: 1024
    Universal.theme: Universal.Dark
    Universal.accent: Universal.Violet
    Page {
        anchors.fill: parent
        //Since the button is being pinned to the top of the page, we need to insert an empty line to overlap the version string
        StackView {
            id: mainStack
            initialItem: welcome
            anchors.fill: parent
            Welcome {
                id:welcome
            }
        }
        header: Item {
            id: element
            ColumnLayout {
                visible: scanner.hasSelected
                RowLayout {
                    Label {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        text: qsTr("Connected Device: ")
                        wrapMode: Text.WordWrap
                        font.bold: true
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignRight
                        fontSizeMode: Text.FixedSize
                    }
                    Button {
                        text: qsTr("Refresh")
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        onClicked: scanner.selected.readDescription()
                    }
                }
                Label {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    text: (scanner.selected && scanner.selected.description || "").replace(/-/g,"<br/>")
                    wrapMode: Text.WordWrap
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                    fontSizeMode: Text.FixedSize
                }
            }
            Label {
                font.bold: true
                text: qsTr("Guitar Configurator")
                anchors.horizontalCenter: parent.horizontalCenter
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 30
            }
            Label {
                text: updateHandler.updateInfo
                textFormat: Text.PlainText
                renderType: Text.QtRendering
                lineHeight: 1
                wrapMode: Text.NoWrap
                font.bold: true
                anchors.right: parent.right
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
                fontSizeMode: Text.Fit
            }
        }
    }

}

















































































/*##^##
Designer {
    D{i:2;anchors_height:60;anchors_width:802;anchors_x:646;anchors_y:658}D{i:1;anchors_height:200;anchors_width:200;anchors_x:846;anchors_y:87}
}
##^##*/
