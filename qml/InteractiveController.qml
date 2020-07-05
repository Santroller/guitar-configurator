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
        // It isnt possible to get the aspect ratio of an SVG image with qml easily.
        // We need it to scale in the right direction, so we just create a version of the image that is never rendered, just to grab its aspect ratio
        Image {
            id: hiddenImg
            source: Defines.getBoardImage(scanner.selected.config.mainSubType)
            visible: false
            sourceSize.width: applicationWindow.width/2
        }
        Image {
            x: (applicationWindow.width-image.paintedWidth)/2
            y: (applicationWindow.height-image.paintedHeight)/2
            property var base: Defines.getBoardBase(scanner.selected.config.mainSubType);
            id: image
            source: Defines.getBoardImage(scanner.selected.config.mainSubType)
            Layout.maximumHeight: applicationWindow.height/2
            Layout.maximumWidth: applicationWindow.width/2
            // We want to use the bigger dimension as the dimension that we scale to. 0 means automatically work out size.
            sourceSize.width: hiddenImg.paintedWidth > hiddenImg.paintedHeight ? applicationWindow.width/2: 0
            sourceSize.height: hiddenImg.paintedHeight > hiddenImg.paintedWidth ? applicationWindow.height/2: 0
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
            // We want to use the bigger dimension as the dimension that we scale to. 0 means automatically work out size.
            sourceSize.width: hiddenImg.paintedWidth > hiddenImg.paintedHeight ? applicationWindow.width/2: 0
            sourceSize.height: hiddenImg.paintedHeight > hiddenImg.paintedWidth ? applicationWindow.height/2: 0
        }
        ColorOverlay {
            visible: true
            anchors.fill: image2
            source: image2
            color: "#80800000"
        }
    }
}
