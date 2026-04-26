// MiniToggle.qml
import QtQuick

MouseArea {
    id: root
    
    property bool checked: true
    signal toggled(bool checked)
    
    width: 36
    height: 20
    
    property color activeColor:   "#847371"
    property color inactiveColor: "#2A2D36"
    property color thumbColor:    "#DDDBF1"
    
    hoverEnabled: true
    cursorShape: Qt.PointingHandCursor

    onClicked: {
        root.checked = !root.checked
        root.toggled(root.checked)
    }
    
    Rectangle {
        id: track
        anchors.fill: parent
        radius: height / 2
        color: root.checked ? root.activeColor : root.inactiveColor

        Behavior on color {
            ColorAnimation { duration: 100 }
        }
    }

    Rectangle {
        id: thumb
        y: 2
        width: parent.height - 4
        height: width
        radius: width / 2
        color: root.thumbColor
        
        x: root.checked ? parent.width - width - 2 : 2

        Behavior on x {
            NumberAnimation {
                duration: 120
                easing.type: Easing.OutCubic
            }
        }
    }
}