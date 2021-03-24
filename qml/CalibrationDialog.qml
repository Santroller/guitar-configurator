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
    property var skipDeadzone: false;
    property var deadZone: 0;
    property var value: 0;
    property var state: 0;
    property var mulFactor: 0.0;
    property var isWhammy: false;
    property var isTrigger: axisRaw < 2;
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
            } else if (state == 2) {
                value = (raw - min) * mulFactor + minValue
                if (!isTrigger && !isWhammy) {
                    value = Math.abs(value);
                }
                calibrationDialog.deadZone = value
            } else {
                calibrationDialog.value = (raw - min) * mulFactor + minValue
                if (isTrigger || isWhammy) {
                    if (value < deadZone) {
                        value = minValue;
                    }
                } else {
                    if (value < deadZone && value > -deadZone) {
                        value = 0;
                    }
                }
            }
        }
    }

    onOpened: {
        axisRaw = ["Lt", "Rt", "LX", "LY", "RX", "RY"].indexOf(axis)
        if (state == 0) {
            deadZone = 0
            min = minValue
            max = maxValue
        }
    }
    onRejected: {
        state = 0
        max = maxValue
    }
    onAccepted: {
        state++;
        if (state == 2) {
            mulFactor = (((maxValue-minValue) / (max-min)))
        } 
        if (state == 3) {
            if (skipDeadzone) state++;
            // if (isTrigger) {
            //     deadZone = -deadZone
            // } 
        }
        if(state < 4) {
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
        minimum: minValue-1
        maximum: maxValue
    }
    RangeBar {
        visible: calibrationDialog.state == 2
        valueMin: (isTrigger || isWhammy) ? minValue : -deadZone
        valueMax: deadZone
        minimum: minValue
        maximum: maxValue
    }
    RangeBar {
        visible: calibrationDialog.state == 3
        valueMin: value-1000
        valueMax: value+1000
        minimum: minValue
        maximum: maxValue
    }
    standardButtons: Dialog.Ok | Dialog.Cancel
    title: ["Move your axis to the minimum value", "Move your axis to the maximum value", "Set the deadzone", "Test the value"][state]
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
