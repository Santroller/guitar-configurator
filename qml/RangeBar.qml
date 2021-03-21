import QtQuick 2.0

import QtQuick.Controls.Universal 2.10
Rectangle { // background
    id: root

// public
    property double maximum: 10
    property double valueMin: 0
    property double valueMax: 0
    property double minimum: 0

// private
    width: parent.width;  height: 20 // default size

    border.width: 0.05 * root.height
    color: Universal.baseLowColor
   
    Rectangle { // foreground
        x: Math.max(0,
               Math.min((valueMin - minimum) / (maximum - minimum) * (parent.width - 0.2 * root.height),
                        parent.width - 0.2 * root.height - width));  y: 0.1 * root.height
        width: Math.max(0,
               Math.min((valueMax - valueMin) / (maximum - minimum) * (parent.width - 0.2 * root.height),
                        parent.width - 0.2 * root.height)) // clip
        height: 0.8 * root.height
        color: Universal.accent
        radius: parent.radius
    }
}