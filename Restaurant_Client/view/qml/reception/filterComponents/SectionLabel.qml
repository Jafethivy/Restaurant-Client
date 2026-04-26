import QtQuick
import QtQuick.Layouts

Text {
    property color textColor: "#6B7080"
    
    font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
    font.letterSpacing: 1.6
    font.capitalization: Font.AllUppercase
    color: textColor
    Layout.alignment: Qt.AlignHCenter
}