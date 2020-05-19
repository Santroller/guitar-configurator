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
Item {
    id:root
    property var colorVal: 0
    property var colorString: ""
    property var buttons: []
    property var originalColorVal: 0;
    signal colorChanged(int color);
    function open() {
        cd.open();
    }

    ColorDialog {
        id:cd
        onVisibilityChanged: {
            if (visible) {
                originalColorVal = colorVal;
                currentColor = "#"+colorVal.toString(16).padStart(6,"0");
            }
        }
        onCurrentColorChanged: {
            var result = /^#?([a-f\d]{2}[a-f\d]{2}[a-f\d]{2})$/i.exec(currentColor);
            colorVal = parseInt(result[1],16);
            ledhandler.setColors(colorVal,buttons);
        }
        onAccepted: {
            ledhandler.setColors(0,buttons);
            root.colorChanged(colorVal);
        }
        onRejected: {
            ledhandler.setColors(0,buttons);
            colorVal=originalColorVal;
            root.colorChanged(colorVal);
        }
    }
}
