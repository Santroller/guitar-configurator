import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.15
import QtQuick.Controls.Universal 2.10
import QtQuick.Layouts 1.10
import QtQuick.Dialogs 1.0
import net.tangentmc 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls.Styles 1.4
import "pins.js" as PinInfo
import "defines.js" as Defines
import "keys.js" as KeyInfo

Dialog {
    id: calibrationDialog
    readonly property var minValue: -32768;
    readonly property var maxValue: 32767;
    property var pin: "";
    property var axis: "";
    property var axisRaw: 0;
    property var min: minValue;
    property var max: maxValue;
    property var value: 0;
    property var state: 0;
    property var mulFactor: 0.0;
    property var isWhammy: false;
    signal calibrationChanged();
    width: 500
    Timer {
        interval: 100; running: visible; repeat: true
        onTriggered: {
            let raw = scanner.selected.readAnalog(axisRaw)
            if (state == 0) {
                calibrationDialog.min = raw
            } else if (state == 1) {
                calibrationDialog.max = raw
            } else {
                if (isWhammy) {
                    value = (raw - min) * mulFactor
                } else {
                    value = (raw - min) * mulFactor + minValue
                }
            }
            console.log(mulFactor)
            console.log(min)
        }
    }

    onOpened: {
        axisRaw = ["Lt", "Rt", "LX", "LY", "RX", "RY"].indexOf(axis)
    }
    onRejected: {
        state = 0
        max = maxValue
    }
    onAccepted: {
        state++;
        if (state == 2) {
            let minV = isWhammy ? 0 : minValue;
            mulFactor = (((maxValue-minV) / (max-min)))
            console.log(mulFactor)
        } 
        if(state < 3) {
            reset()
            open()
        } else {
            state = 0
            max = maxValue
            calibrationChanged(mulFactor, min)
        }
    }

    RangeBar {
        visible: calibrationDialog.state < 2
        valueMin: calibrationDialog.min
        valueMax: calibrationDialog.max
        minimum: minValue
        maximum: maxValue
    }
    RangeBar {
        visible: calibrationDialog.state == 2
        valueMin: value-5
        valueMax: value+5
        minimum: minValue
        maximum: maxValue
    }
    standardButtons: Dialog.Ok | Dialog.Cancel
    title: ["Move your axis to the minimum value", "Move your axis to the maximum value", "Test the value"][state]
    x: (parent.width - width) / 2
    y: {
        //Base position on the parent position, but bump the dialog up if it hits the bottom of the screen.
        var y = (parent.height - height) / 2;
        var globalYTop = parent.mapToGlobal(x-width/2,y-height/2).y;
        var globalYBot = parent.mapToGlobal(x+width,y+height).y;
        if (globalYBot > applicationWindow.height) {
            y -= globalYBot-applicationWindow.height;
        }
        if (globalYTop < 0) {
            y -= globalYTop;
        }
        return y;
    }
}
