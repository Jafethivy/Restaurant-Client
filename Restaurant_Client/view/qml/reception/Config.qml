import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    width: 220
    height: 160
    color: "#2d2d2d"        // COLOR: fondo del panel
    radius: 8
    border.color: "#404040" // COLOR: borde del panel
    border.width: 1

    property int tableCount: 0

    signal tableAdded(int total)
    signal tableRemoved(int total)

    function onTableAdded() {
        tableCount++
        console.log("Mesa agregada | Total mesas:", tableCount)
        root.tableAdded(tableCount)
    }

    function onTableRemoved() {
        if (tableCount <= 0) return
        tableCount--
        console.log("Mesa eliminada | Total mesas:", tableCount)
        root.tableRemoved(tableCount)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Text {
            text: "Mesas"
            color: "white"              // COLOR: titulo del panel
            font.bold: true
            font.pixelSize: 13
            Layout.alignment: Qt.AlignHCenter
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#404040"            // COLOR: linea separadora
        }

        // Contador
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 64
            height: 32
            radius: 8
            color: "#1e1e1e"            // COLOR: fondo del contador
            border.color: "#404040"     // COLOR: borde del contador
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: root.tableCount
                color: "white"          // COLOR: numero del contador
                font.pixelSize: 16
                font.bold: true
            }
        }

        // Botones
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            TableButton {
                Layout.fillWidth: true
                label: "Agregar mesa"
                onAction: root.onTableAdded()
            }

            TableButton {
                Layout.fillWidth: true
                label: "Eliminar mesa"
                secondary: true
                enabled: root.tableCount > 0
                onAction: root.onTableRemoved()
            }
        }
    }

    component TableButton: Rectangle {
        property string label: ""
        property bool   secondary: false
        signal action()

        height: 34
        radius: 10
        color: !enabled
               ? "#2a2a2a"                                              // COLOR: boton deshabilitado
               : secondary
                 ? (btnArea.containsMouse ? "#5a2020" : "#3d1a1a")     // COLOR: boton eliminar (hover : normal)
                 : (btnArea.containsMouse ? "#1f5a2e" : "#1a3d24")     // COLOR: boton agregar  (hover : normal)
        border.width: 1
        border.color: !enabled
               ? "#333333"                                              // COLOR: borde deshabilitado
               : secondary
                 ? "#7a2020"                                            // COLOR: borde boton eliminar
                 : "#2e7a42"                                            // COLOR: borde boton agregar
        opacity: enabled ? 1.0 : 0.4

        Text {
            anchors.centerIn: parent
            text: label
            color: !enabled
                   ? "#666666"                                          // COLOR: texto deshabilitado
                   : secondary
                     ? "#e74c3c"                                        // COLOR: texto boton eliminar
                     : "#2ecc71"                                        // COLOR: texto boton agregar
            font.pixelSize: 13
            font.bold: true
        }

        MouseArea {
            id: btnArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: if (parent.enabled) action()
        }
    }
}