// CloseButton.qml
import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    
    signal clicked()
    
    property color normalColor:   "#2A2D36"
    property color hoverColor:    Qt.lighter(normalColor, 1.3)
    property color textColor:     "#6B7080"
    
    property string buttonText:   "\u02C5"
    property int    textPixelSize: 20
    property string textFamily:   "Rockwell"
    property int    textWeight:   Font.ExtraBold
    
    property int    buttonRadius: 8
    property int    buttonHeight: 30
    
    Layout.fillWidth: true
    Layout.leftMargin: 5
    Layout.rightMargin: 5
    Layout.alignment: Qt.AlignHCenter
    
    height: buttonHeight
    implicitHeight: buttonHeight
    radius: buttonRadius
    color: mouseArea.containsMouse ? hoverColor : normalColor

    Behavior on color { ColorAnimation { duration: 100 } }

    Text {
        anchors.centerIn: parent
        text: root.buttonText
        color: root.textColor
        font {
            pixelSize: root.textPixelSize
            family:    root.textFamily
            weight:    root.textWeight
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}