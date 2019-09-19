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
        Label {
            id: title
            text: qsTr("Arduino Bindings")
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
            source: scanner.selected.image
            fillMode: Image.PreserveAspectFit
            Layout.maximumHeight: applicationWindow.height/3
            Layout.maximumWidth: applicationWindow.width/3
        }
        Label {
            id: dev
            text: qsTr("Connected Device: ")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.FixedSize
        }
        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Label {
                id: status
                text: scanner.selected.description
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                fontSizeMode: Text.FixedSize
            }
            Button {
                id: refreshBt
                text: qsTr("Refresh")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: programmer.getStatusDescription()
            }
        }

        ComboBox {
            id: orientationBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: {
                let model = []; 
                for (let i = 0; i <= MPU6050Orientations.END; i++) {
                    model.push({key: ArdwiinoLookup.getOrientationName(i), value:i});
                }
                return model
            }
            currentIndex: scanner.selected.getOrientation();

            onCurrentIndexChanged: scanner.selected.setOrientation(orientationBox.model[orientationBox.currentIndex].value)
        }

        ComboBox {
            id: tiltBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: {
                let model = [];
                for (let i = 0; i <= TiltTypes.END; i++) {
                    model.push({key: ArdwiinoLookup.getTiltTypeName(i), value:i});
                }
                return model
            }
            currentIndex: scanner.selected.getTiltType();

            onCurrentIndexChanged: scanner.selected.setTiltType(tiltBox.model[tiltBox.currentIndex].value)
        }

        ComboBox {
            id: inputBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: {
                let model = [];
                for (let i = 0; i <= InputTypes.END; i++) {
                    model.push({key: ArdwiinoLookup.getInputTypeName(i), value:i});
                }
                return model
            }
            currentIndex: scanner.selected.getInputType();

            onCurrentIndexChanged: scanner.selected.setInputType(inputBox.model[inputBox.currentIndex].value)
        }

        ComboBox {
            id: comboBox
            Layout.fillWidth: true
            textRole: "key"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            model: {
                let model = [];
                for (let i = 0; i <= Controllers.END; i++) {
                    let name = ArdwiinoLookup.getControllerTypeName(i);
                    if (name !== "Unknown Controller") {
                        model.push({key: name, value:i});
                    }
                }
                return model
            }
            currentIndex: comboBox.model.findIndex(s => s.value === scanner.selected.getType());

            onCurrentIndexChanged: scanner.selected.setType(comboBox.model[comboBox.currentIndex].value)
        }

        Button {
            id: restore
            text: qsTr("Restore Device back to Arduino")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            visible: scanner.selected.hasDFU
            onClicked: {
                programmer.setRestoring(true);
                mainStack.push("Programmer.qml");
            }
        }
        Button {
            id: configureContinue
            text: qsTr("Write")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            enabled: true
            onClicked: scanner.selected.writeConfig();
        }


    }
}



















































































/*##^##
Designer {
    D{i:0;autoSize:true;height:1080;width:1920}D{i:2;anchors_height:400;anchors_y:232}
D{i:3;anchors_y:774}D{i:1;anchors_height:60;anchors_width:802;anchors_x:646;anchors_y:658}
}
##^##*/
