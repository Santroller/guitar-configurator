import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import QtQuick.Dialogs 1.0
import net.tangentmc 1.0
import QtGraphicalEffects 1.12
import "pins.js" as PinInfo
import "defines.js" as Defines
import "keys.js" as KeyInfo

ColumnLayout {
    id: main
    Layout.alignment: Qt.AlignHCenter
    property string hoveredButton;
    property var cursorX;
    property var cursorY;
    Item {
        Image {
            x: (applicationWindow.width-image.paintedWidth)/2
            y: (applicationWindow.height-image.paintedHeight)/2
            property var base: Defines.getBoardBase(scanner.selected.type);
            id: image
            source: Defines.getBoardImage(scanner.selected.type)
            fillMode: Image.PreserveAspectFit
            Layout.maximumHeight: applicationWindow.height/2
            Layout.maximumWidth: applicationWindow.width/2
            sourceSize.width: applicationWindow.width/2
            MouseArea {
                hoverEnabled: true
                anchors.fill: parent

                onPositionChanged: {
                    var newSource = scanner.findElement(image.base, image.width, image.height, mouseX, mouseY);
                    if (newSource === "") {
                        main.hoveredButton = null;
                        image2.source = "";
                    } else if (!image2.source.toString().endsWith(newSource)) {
                        image2.source  = newSource;
                        var a = mapToItem(main, mouseX, mouseY);
                        main.cursorX = a.x;
                        main.cursorY = a.y;
                        main.hoveredButton = newSource.toString().replace(/.*\/g(.*).svg/,"$1");
                    }


                }
            }
        }
        Image {
            x: (applicationWindow.width-image.paintedWidth)/2
            y: (applicationWindow.height-image.paintedHeight)/2
            id: image2
            visible: true
            fillMode: Image.PreserveAspectFit
            Layout.maximumHeight: applicationWindow.height/2
            Layout.maximumWidth: applicationWindow.width/2
            sourceSize.width: applicationWindow.width/2
        }
        ColorOverlay {
            visible: true
            anchors.fill: image2
            source: image2
            color: "#80800000"
        }
    }
}
