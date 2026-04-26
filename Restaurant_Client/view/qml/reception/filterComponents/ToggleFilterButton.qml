// components/ToggleFilterButton.qml
import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    width: 100
    height: 30
    radius: 9
    
    required property string label
    required property color activeColor
    property color surfaceColor: "#1F2128"
    property color borderColor:  "#2A2D36"
    property color mutedColor:   "#6B7080"
    
    property bool active: false
    
    signal toggled(bool active)
    
    color: active ? Qt.alpha(activeColor, 0.16) : surfaceColor
    border.color: active ? activeColor : borderColor
    border.width: active ? 1.5 : 1
    
    Behavior on color { ColorAnimation { duration: 130 } }
    Behavior on border.color { ColorAnimation { duration: 130 } }
    
    Text {
        anchors.centerIn: parent
        text: root.label
        color: root.active ? root.activeColor : root.mutedColor
        font.pixelSize: 12
        font.weight: Font.DemiBold
        font.letterSpacing: 1.0
        Behavior on color { ColorAnimation { duration: 130 } }
    }
    
    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            root.active = !root.active
            root.toggled(root.active)
        }
    }
}