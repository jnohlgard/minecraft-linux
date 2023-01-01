import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Templates 2.1 as T
import QtQuick.Window 2.3

T.ProgressBar {
    id: control

    padding: 2
    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: 28
    baselineOffset: contentItem.y + contentItem.baselineOffset

    background: BorderImage {
        anchors.fill: parent
        source: "qrc:/Resources/progressbar.png"
        smooth: false
        border { left: 4; top: 4; right: 4; bottom: 4 }
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch
    }

    contentItem: Item {
        BorderImage {
            width: parent.width * control.visualPosition
            height: parent.height
            source: "qrc:/Resources/progressbar-progress.png"
            smooth: false
            border { left: 2; top: 2; right: 2; bottom: 2 }
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
        }
    }
}
