import QtQuick 2.0
import QtQuick.Templates 2.1 as T

T.Button {
    id: control

    property color textColor: "white"

    padding: 8
    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
    baselineOffset: contentItem.y + contentItem.baselineOffset

    background: Rectangle {
        id: buttonBackground
        color: "#000"
        opacity: control.hovered ? 0.1 : 0
    }

    contentItem: Text {
        id: textItem
        text: control.text

        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
