import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import net.tangentmc 1.0

Page {
    id: page
    title: "Tilt Configuration"
    ColumnLayout {
        id: column
        anchors.fill: parent
        Label {
            id: title
            text: qsTr("Tilt Configuration")
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
            /*transform: Rotation{
                id: rotateImagePhoto
                angle: 0
                origin.x: image.width/2
                origin.y: image.height/2
            }
            Timer {
                    interval: 100; running: true; repeat: true
                    onTriggered: rotateImagePhoto.angle = scanner.selected.getTilt()
            }*/
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
                onClicked: scanner.selected.readDescription()
            }
        }

        Label {
            id: type
            text: qsTr("Tilt Sensor Type: ")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.FixedSize
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
            Binding { target: tiltBox; property: "currentIndex"; value: tiltBox.model.findIndex(s => s.value === scanner.selected.tiltType) }

            onCurrentIndexChanged: scanner.selected.tiltType = tiltBox.model[tiltBox.currentIndex].value
        }

        Label {
            id: orientation
            visible: scanner.selected.tiltType === TiltTypes.MPU_6050_SENSOR;
            text: qsTr("Tilt Sensor Orientation: ")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.FixedSize
        }
        ComboBox {
            id: orientationBox
            visible: scanner.selected.tiltType === TiltTypes.MPU_6050_SENSOR;
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
            Binding { target: orientationBox; property: "currentIndex"; value: orientationBox.model.findIndex(s => s.value === scanner.selected.orientation) }

            onCurrentIndexChanged: scanner.selected.orientation = orientationBox.model[orientationBox.currentIndex].value
        }

        Label {
            id: orientation1
            text: qsTr("Tilt Sensor Sensitivity: ")
            fontSizeMode: Text.FixedSize
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
            visible: scanner.selected.tiltType === TiltTypes.MPU_6050_SENSOR
        }

        Slider {
            id: slider
            Layout.fillWidth: true
            to: -32767
            from: 32767
            value: scanner.selected.sensitivity
            onValueChanged: scanner.selected.sensitivity = slider.value
        }

        Button {
            id: configureContinue
            text: qsTr("Back to Configuration")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                mainStack.pop();
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
