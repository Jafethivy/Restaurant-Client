import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

SpinBox {
    id: control
    // Propiedades de tema inyectables
    property color clrSurface:   "#1F2128"
    property color clrBorder:    "#2A2D36"
    property color clrAccent:    "#4F8EF7"
    property color clrText:      "#E8EAF0"
    property color clrMuted:     "#6B7080"
    property int animDuration:   100          // <- Unificado a 100ms
    
    editable: true
    implicitHeight: 30
    leftPadding: down.indicator.width
    rightPadding: up.indicator.width
    
    // Validacion: asegurar siempre 2 digitos al imprimir
    textFromValue: function(value, locale) {
        return value < 10 ? "0" + value : value
    }
    
    background: Rectangle {
        radius: 8
        color: control.clrSurface
        border.color: control.activeFocus ? control.clrAccent : control.clrBorder
        border.width: control.activeFocus ? 1.5 : 1
        
        Behavior on border.color { ColorAnimation { duration: control.animDuration } }
    }
    
    contentItem: TextInput {
        text: control.textFromValue(control.value, control.locale)
        font.pixelSize: 14
        font.weight: Font.Medium
        color: control.clrText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: Qt.ImhDigitsOnly
        
        // UX: Seleccionar todo al enfocar para reemplazo rapido
        onActiveFocusChanged: if (activeFocus) selectAll()
    }
    
    up.indicator: Rectangle {
        x: control.mirrored ? 0 : control.width - width
        height: control.height
        width: 30
        radius: 8
        color: control.up.pressed ? control.clrBorder : "transparent"
        Behavior on color { ColorAnimation { duration: control.animDuration } }
        
        Text {
            anchors.centerIn: parent
            text: "\u25B2"
            font { pixelSize: 10; family: "Rockwell"; weight: Font.Normal }
            color: control.up.pressed ? control.clrAccent : control.clrMuted
            Behavior on color { ColorAnimation { duration: control.animDuration } }
        }
    }
    
    down.indicator: Rectangle {
        x: control.mirrored ? control.width - width : 0
        height: control.height
        width: 30
        radius: 8
        color: control.down.pressed ? control.clrBorder : "transparent"
        Behavior on color { ColorAnimation { duration: control.animDuration } }
        
        Text {
            anchors.centerIn: parent
            text: "\u25BC"
            font { pixelSize: 10; family: "Rockwell"; weight: Font.Normal }
            color: control.down.pressed ? control.clrAccent : control.clrMuted
            Behavior on color { ColorAnimation { duration: control.animDuration } }
        }
    }
}