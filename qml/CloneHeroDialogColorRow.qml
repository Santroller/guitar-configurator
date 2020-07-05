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

RowLayout {
    function getColor() {
        return "#"+(scanner.selected.ghColours[modelData]).toString(16).padStart(6,"0");
    }
    id: root
    visible: scanner.selected.config.hasAddressableLEDs
    property bool checked;
    property int color;
    property string name;
    Label {
        text: qsTr(name+" LED Config")
        fontSizeMode: Text.FixedSize
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        wrapMode: Text.WordWrap
    }
    Switch {
        Component.onCompleted: checked = root.checked
        onCheckedChanged: root.checked = checked;
    }
    Rectangle {
        radius: colorBt3.height
        color: "#"+(root.color).toString(16).padStart(6,"0")
        width: colorBt3.height
        height: colorBt3.height
    }
    Button {
        visible: scanner.selected.config.hasAddressableLEDs
        text: "Change colour"
        onClicked: {
            color.open()
        }
        id: colorBt3
    }
    ColorPickerDialog {
        id: color
        buttons: ["a","b","y","x","LB"]
        colorVal: root.color
        onColorChanged: root.color = colorVal
    }
}
