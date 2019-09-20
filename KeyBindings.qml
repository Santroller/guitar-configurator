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
            text: qsTr("Keyboard Bindings")
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

        GridLayout {
            id: gl
            columns: 3
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            property var keyMap: {
                0x04: [Qt.Key_A],
                0x05: [Qt.Key_B],
                0x06: [Qt.Key_C],
                0x07: [Qt.Key_D],
                0x08: [Qt.Key_E],
                0x09: [Qt.Key_F],
                0x0A: [Qt.Key_G],
                0x0B: [Qt.Key_H],
                0x0C: [Qt.Key_I],
                0x0D: [Qt.Key_J],
                0x0E: [Qt.Key_K],
                0x0F: [Qt.Key_L],
                0x10: [Qt.Key_M],
                0x11: [Qt.Key_N],
                0x12: [Qt.Key_O],
                0x13: [Qt.Key_P],
                0x14: [Qt.Key_Q],
                0x15: [Qt.Key_R],
                0x16: [Qt.Key_S],
                0x17: [Qt.Key_T],
                0x18: [Qt.Key_U],
                0x19: [Qt.Key_V],
                0x1A: [Qt.Key_W],
                0x1B: [Qt.Key_X],
                0x1C: [Qt.Key_Y],
                0x1D: [Qt.Key_Z],
                0x28: [Qt.Key_Enter],
                0x29: [Qt.Key_Escape],
                0x2A: [Qt.Key_Backspace],
                0x2B: [Qt.Key_Tab],
                0x2C: [Qt.Key_Space],
                0x2D: [Qt.Key_Minus],
                0x2D: [Qt.Key_Underscore],
                0x2E: [Qt.Key_Equal],
                0x2E: [Qt.Key_Plus],
                0x36: [Qt.Key_Comma],
                0x36: [Qt.Key_Less],
                0x37: [Qt.Key_Period],
                0x37: [Qt.Key_Greater],
                0x38: [Qt.Key_Slash],
                0x38: [Qt.Key_Question],
                0x39: [Qt.Key_CapsLock],
                0x3A: [Qt.Key_F1],
                0x3B: [Qt.Key_F2],
                0x3C: [Qt.Key_F3],
                0x3D: [Qt.Key_F4],
                0x3E: [Qt.Key_F5],
                0x3F: [Qt.Key_F6],
                0x40: [Qt.Key_F7],
                0x41: [Qt.Key_F8],
                0x42: [Qt.Key_F9],
                0x43: [Qt.Key_F10],
                0x44: [Qt.Key_F11],
                0x45: [Qt.Key_F12],
                0x46: [Qt.Key_Print],
                0x47: [Qt.Key_ScrollLock],
                0x48: [Qt.Key_Pause],
                0x49: [Qt.Key_Insert],
                0x4A: [Qt.Key_Home],
                0x4B: [Qt.Key_PageUp],
                0x4C: [Qt.Key_Delete],
                0x4D: [Qt.Key_End],
                0x4E: [Qt.Key_PageDown],
                0x4F: [Qt.Key_Right],
                0x50: [Qt.Key_Left],
                0x51: [Qt.Key_Down],
                0x52: [Qt.Key_Up],
                0x53: [Qt.Key_NumLock],
                0x68: [Qt.Key_F13],
                0x69: [Qt.Key_F14],
                0x6A: [Qt.Key_F15],
                0x6B: [Qt.Key_F16],
                0x6C: [Qt.Key_F17],
                0x6D: [Qt.Key_F18],
                0x6E: [Qt.Key_F19],
                0x6F: [Qt.Key_F20],
                0x70: [Qt.Key_F21],
                0x71: [Qt.Key_F22],
                0x72: [Qt.Key_F23],
                0x73: [Qt.Key_F24],
                0x74: [Qt.Key_Execute],
                0x75: [Qt.Key_Help],
                0x76: [Qt.Key_Menu],
                0x77: [Qt.Key_Select],
                0x78: [Qt.Key_Stop],
                0x79: [Qt.Key_Redo],
                0x7A: [Qt.Key_Undo],
                0x7B: [Qt.Key_Cut],
                0x7C: [Qt.Key_Copy],
                0x7D: [Qt.Key_Paste],
                0x7E: [Qt.Key_Find],
                0x7F: [Qt.Key_VolumeMute],
                0x80: [Qt.Key_VolumeUp],
                0x81: [Qt.Key_VolumeDown],
                0x9A: [Qt.Key_SysReq],
                0x9B: [Qt.Key_Cancel],
                0x9C: [Qt.Key_Clear],
                0x9E: [Qt.Key_Return],
                0xE8: [Qt.Key_MediaPlay],
                0xE9: [Qt.Key_MediaStop],
                0xEA: [Qt.Key_MediaPrevious],
                0xEB: [Qt.Key_MediaNext],
                0xF0: [Qt.Key_WWW],
                0xF4: [Qt.Key_Search],
                0xF8: [Qt.Key_Sleep],
                0xFB: [Qt.Key_Calculator],
                0x27: [Qt.Key_0,Qt.Key_ParenRight],
                0x59: [Qt.Key_1|Qt.KeypadModifier],
                0x5A: [Qt.Key_2|Qt.KeypadModifier],
                0x5B: [Qt.Key_3|Qt.KeypadModifier],
                0x5C: [Qt.Key_4|Qt.KeypadModifier],
                0x5D: [Qt.Key_5|Qt.KeypadModifier],
                0x5E: [Qt.Key_6|Qt.KeypadModifier],
                0x5F: [Qt.Key_7|Qt.KeypadModifier],
                0x60: [Qt.Key_8|Qt.KeypadModifier],
                0x61: [Qt.Key_9|Qt.KeypadModifier],
                0x62: [Qt.Key_0|Qt.KeypadModifier],
                0x55: [Qt.Key_Asterisk|Qt.KeypadModifier],
                0xB6: [Qt.Key_ParenLeft|Qt.KeypadModifier],
                0xB7: [Qt.Key_ParenRight|Qt.KeypadModifier],
                0x58: [Qt.Key_Enter|Qt.KeypadModifier],
                0x56: [Qt.Key_Minus|Qt.KeypadModifier],
                0x67: [Qt.Key_Equal|Qt.KeypadModifier],
                0x57: [Qt.Key_Plus|Qt.KeypadModifier],
                0x85: [Qt.Key_Comma|Qt.KeypadModifier],
                0x63: [Qt.Key_Period|Qt.KeypadModifier],
                0x54: [Qt.Key_Slash|Qt.KeypadModifier],
                0x62: [Qt.Key_Insert|Qt.KeypadModifier],
                0x5F: [Qt.Key_Home|Qt.KeypadModifier],
                0x61: [Qt.Key_PageUp|Qt.KeypadModifier],
                0x63: [Qt.Key_Delete|Qt.KeypadModifier],
                0x59: [Qt.Key_End|Qt.KeypadModifier],
                0x5B: [Qt.Key_PageDown|Qt.KeypadModifier],
                0x5E: [Qt.Key_Right|Qt.KeypadModifier],
                0x5C: [Qt.Key_Left|Qt.KeypadModifier],
                0x5A: [Qt.Key_Down|Qt.KeypadModifier],
                0x60: [Qt.Key_Up|Qt.KeypadModifier],
                //TODO: We have to work out how to deal with shift here...
                0x1E: [Qt.Key_1, Qt.Key_Exclam],
                0x1F: [Qt.Key_2, Qt.Key_At],
                0x20: [Qt.Key_3, Qt.Key_NumberSign],
                0x21: [Qt.Key_4, Qt.Key_Dollar],
                0x22: [Qt.Key_5, Qt.Key_Percent],
                0x23: [Qt.Key_6, Qt.Key_AsciiCircum],
                0x24: [Qt.Key_7, Qt.Key_Ampersand],
                0x25: [Qt.Key_8, Qt.Key_Asterisk],
                0x26: [Qt.Key_9, Qt.Key_ParenLeft],
                0x2F: [Qt.Key_BracketLeft, Qt.Key_BraceLeft],
                0x30: [Qt.Key_BracketRight, Qt.Key_BraceRight],
                0x31: [Qt.Key_Backslash, Qt.Key_Bar],
                0x33: [Qt.Key_Semicolon, Qt.Key_Colon],
                0x34: [Qt.Key_Apostrophe, Qt.Key_QuoteDbl],
                0x35: [Qt.Key_Agrave, Qt.Key_Tilde],
                0x65: [Qt.Key_ApplicationLeft, Qt.Key_ApplicationRight],
                0x66: [Qt.Key_PowerOff, Qt.Key_PowerDown],
            }

            property var test: {
                for (let i in keyMap) {
                    console.log(ArdwiinoLookup.getKeyName(keyMap[i]));
                }
                return "";
            }

            property var labels: {
                "up": "DPad Up",
                "down": "DPad Down",
                "left": "DPad Left",
                "right": "DPad Right",
                "start": "Start Button",
                "back": "Back Button",
                "left_stick": "Left Stick Click",
                "right_stick": "Right Stick Click",
                "LB": "Left Bumper",
                "RB": "Right Bumper",
                "home": "Home Button",
                "a": "A Button",
                "b": "B Button",
                "x": "X Button",
                "y": "Y Button",
                "lt": "Left Shoulder Axis",
                "rt": "Right Shoulder Axis",
                "l_x_lt": "Left Joystick X Axis (Positive)",
                "l_x_gt": "Left Joystick X Axis (Negative)",
                "l_y_lt": "Left Joystick Y Axis (Positive)",
                "l_y_gt": "Left Joystick Y Axis (Negative)",
                "r_x_lt": "Right Joystick X Axis (Positive)",
                "r_x_gt": "Right Joystick X Axis (Negative)",
                "r_y_lt": "Right Joystick Y Axis (Positive)",
                "r_y_gt": "Right Joystick Y Axis (Negative)",
            }

            Label {
                text: "Actions"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Label {
                text: "Pin Binding"
                font.pointSize: 15
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
            }
            Repeater {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Object.keys(gl.labels).length
                Label {
                    Layout.row: index+1
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    property var key: Object.keys(gl.labels)[index]
                    id: label
                    text: gl.labels[key]
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                }
            }
            Repeater {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: Object.keys(gl.labels).length
                Button {
                    Layout.row: index+1
                    Layout.column: 1
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    property var key: Object.keys(gl.labels)[index]
                    id: bt
                    text: scanner.selected.pins[key] === 255 ? "No Key" : ArdwiinoLookup.getKeyName(gl.keyMap[scanner.selected.pins[key]])
                    onClicked: {
                        scanner.selected.currentKey = key;
                    }
                    Keys.onPressed: {

                    }
                }
            }

        }

        Dialog {
            id: pinDialog
            title: "Select a Pin"
            visible: scanner.selected.currentKey
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            modal: true
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Button {
                id: configureContinue
                text: qsTr("Save Bindings")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    mainStack.pop();
                }
            }

            Button {
                id: cancel
                text: qsTr("Cancel")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    mainStack.pop();
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
