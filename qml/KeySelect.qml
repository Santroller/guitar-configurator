import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import net.tangentmc 1.0

Page {
    id: page
    ColumnLayout {
        id: column
        anchors.fill: parent
        property var current: scanner.selected.config[`pins${scanner.selected.currentPin}`]
        Image {
            function g(count, x, y, width, id) {
                var ret = [];
                for (var i = 0; i < count; i++) {
                    ret.push({x: x+i*width, y:y, id:i+id});
                }
                return ret;
            }
            property var locations: {
                "/images/uno.png": {pins: [...g(8,1083, 13, -40, 0), ...g(6,743, 13, -40, 8), ...g(6,873, 770, 40, 14)], r:30},
                "/images/micro.png": {pins: [
                        ...g(2,348, 5, -31, 0),
                        ...g(8,224, 5, -31, 2),
                        {x:8, y: 191, id: 10},
                        {x:39, y: 191, id: 16},
                        {x:72, y: 191, id: 14},
                        {x:103, y: 191, id: 15},
                        ...g(4, 132, 191, 32, 18)
                    ], r:25},
                "/images/leonardo.png": {pins: [...g(8,903, 18, -35, 0), ...g(6,597, 27, -35, 8), ...g(6,727, 697, 35, 14)], r:25},
            }
            property var scaleX: 1 / sourceSize.width * paintedWidth
            property var scaleY: 1 / sourceSize.height * paintedHeight
            property var startX: (width - paintedWidth) / 2
            property var startY: (height - paintedHeight) / 2
            property var selected: locations[scanner.selected.getDirectBoardImage()];
            property var r: selected.r * scaleX
            property var pins: selected.pins
            id: image
            Layout.alignment: Qt.AlignHCenter
            source: scanner.selected.getDirectBoardImage()
            fillMode: Image.PreserveAspectFit
            Layout.maximumHeight: applicationWindow.height/3
            Layout.maximumWidth: applicationWindow.width/3
            Repeater {
                model: image.pins.length
                id: rp
                Rectangle {
                    width: image.r; height: image.r
                    x: image.startX + image.pins[index].x * image.scaleX
                    y: image.startY + image.pins[index].y * image.scaleY
                    radius: image.r * 0.5
                    border.width: 1
                    color: column.current === image.pins[index].id ? "green" : mouseArea.containsMouse ? "red":"yellow"
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: column.current = image.pins[index].id
                    }
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Button {
                id: configureContinue
                text: qsTr("Set Pin Binding")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    var pins = scanner.selected.pins;
                    pins[scanner.selected.currentPin] = column.current;
                    scanner.selected.pins = pins;
                    scanner.selected.currentPin = "";
                    pinDialog.accept();
                }
            }

            Button {
                id: disable
                text: qsTr("Disable Pin Binding")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {var pins = scanner.selected.pins;
                    pins[scanner.selected.currentPin] = 0xFF;
                    scanner.selected.pins = pins;
                    scanner.selected.currentPin = "";
                    pinDialog.accept();
                }
            }

            Button {
                id: cancel
                text: qsTr("Cancel")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    scanner.selected.loadPins();
                    scanner.selected.currentPin = "";
                    pinDialog.reject();
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
