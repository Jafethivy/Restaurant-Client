import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// ============================================================
// POPUP/MENÚ PARA GESTIÓN DE ESTADOS DE MESA
// Layout horizontal con patrón de cierre mejorado
// ============================================================

Rectangle {
    id: tablePopup

    // === PROPIEDADES CONFIGURABLES ===
    property int currentState: 0
    property int tableId: 0

    // === ESTADOS ===
    property bool isOpen: false

    // === SEÑALES (sin modificar) ===
    signal reserveTable(int tableId)
    signal occupyTable(int tableId)

    // === FUNCIONES DE APERTURA/CIERRE ===
    function open() {
        if (isOpen) return
        isOpen = true
        visible = true
        opacityAnimation.restart()
        scaleAnimation.restart()
    }

    function close() {
        if (!isOpen) return
        isOpen = false
        opacityAnimationReverse.restart()
        scaleAnimationReverse.restart()
    }

    // === PALETA DE COLORES ===
    QtObject {
        id: theme
        readonly property color available:  "#4CAF50"
        readonly property color reserved:   "#FFC107"
        readonly property color occupied:   "#F44336"
        readonly property color unknown:    "#9E9E9E"
        readonly property color background: "#293651"
        readonly property color title:      "#DDDBF1"
        readonly property color white:      "#ffffff"
        readonly property color shadow:     "#000000"
        readonly property color buttonBg:   "#3d4a63"
        readonly property color buttonHover: "#4a5a75"
    }

    // === CONFIGURACIÓN VISUAL ===
    width: parent ? parent.width : 400
    height: parent ? parent.height : 300
    color: "#80000000"
    visible: false
    opacity: 0
    radius: 8
    z: 100  // Asegurar que esté arriba de todo

    // Click fuera del popup para cerrar
    MouseArea {
        anchors.fill: parent
        onClicked: tablePopup.close()
    }

    // Contenedor del popup
    Rectangle {
        id: popupContainer
        width: contentLayout.implicitWidth + 48
        height: contentLayout.implicitHeight + 40
        anchors.centerIn: parent
        color: theme.background
        radius: 12
        border.color: theme.white
        border.width: 1
        scale: 0.8

        // Sombra del popup
        Rectangle {
            anchors.fill: parent
            color: theme.shadow
            opacity: 0.4
            radius: 12
            z: -1
            anchors.margins: -6
        }

        // Layout principal HORIZONTAL
        RowLayout {
            id: contentLayout
            anchors {
                fill: parent
                margins: 20
            }
            spacing: 24

            // ============================================================
            // COLUMNA IZQUIERDA: Información de estado + Cerrar
            // ============================================================
            ColumnLayout {
                spacing: 16
                Layout.alignment: Qt.AlignVCenter

                // Título del popup
                Text {
                    text: "Mesa #" + tablePopup.tableId
                    font { pixelSize: 18; bold: true; family: "Rockwell" }
                    color: theme.title
                    Layout.alignment: Qt.AlignHCenter
                }

                // Indicador visual del estado actual
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 120
                    height: 28
                    radius: 14
                    color: {
                        if (tablePopup.currentState === 0) return theme.available
                        if (tablePopup.currentState === 1) return theme.reserved
                        if (tablePopup.currentState === 2) return theme.occupied
                        return theme.unknown
                    }

                    Text {
                        anchors.centerIn: parent
                        text: {
                            if (tablePopup.currentState === 0) return "Disponible"
                            if (tablePopup.currentState === 1) return "Reservado"
                            if (tablePopup.currentState === 2) return "Ocupado"
                            return "Desconocido"
                        }
                        font { pixelSize: 12; bold: true }
                        color: theme.white
                    }
                }

                // Separador vertical (visual)
                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: theme.white
                    opacity: 0.2
                }

                // Botón cancelar/cerrar
                Rectangle {
                    id: btnCancel
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    radius: 8
                    visible: tablePopup.currentState !== 2
                    color: mouseAreaCancel.containsPress ? "#555555" : "#444444"

                    Text {
                        anchors.centerIn: parent
                        text: "Cancelar"
                        font { pixelSize: 13 }
                        color: theme.white
                    }

                    MouseArea {
                        id: mouseAreaCancel
                        anchors.fill: parent
                        onClicked: tablePopup.close()
                    }
                }

                // Mensaje cuando está ocupada (estado final)
                Text {
                    text: "Mesa ocupada - No se pueden realizar cambios"
                    visible: tablePopup.currentState === 2
                    Layout.preferredHeight: visible ? implicitHeight : 0
                    font { pixelSize: 12; italic: true }
                    color: theme.occupied
                    Layout.alignment: Qt.AlignHCenter
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            // Separador vertical entre columnas
            Rectangle {
                Layout.fillHeight: true
                width: 1
                color: theme.white
                opacity: 0.2
                visible: tablePopup.currentState !== 2
            }

            // ============================================================
            // COLUMNA DERECHA: Botones de acción
            // ============================================================
            ColumnLayout {
                spacing: 12
                Layout.alignment: Qt.AlignVCenter
                visible: tablePopup.currentState !== 2

                // Botón: Reservar (solo visible si está Disponible)
                Rectangle {
                    id: btnReserve
                    Layout.fillWidth: true
                    width: 140
                    Layout.preferredHeight: 40
                    visible: tablePopup.currentState === 0
                    radius: 8
                    color: mouseAreaReserve.containsPress ? theme.buttonHover : theme.buttonBg
                    border.color: theme.reserved
                    border.width: 2

                    Text {
                        anchors.centerIn: parent
                        text: "Reservar mesa"
                        font { pixelSize: 14; bold: true }
                        color: theme.white
                    }

                    MouseArea {
                        id: mouseAreaReserve
                        anchors.fill: parent
                        onClicked: {
                            tablePopup.reserveTable(tablePopup.tableId)
                            tablePopup.close()
                        }
                    }
                }

                // Botón: Ocupar (visible si está Disponible o Reservado)
                Rectangle {
                    id: btnOccupy
                    Layout.fillWidth: true
                    width: 140
                    Layout.preferredHeight: 40
                    visible: tablePopup.currentState === 0 || tablePopup.currentState === 1
                    radius: 8
                    color: mouseAreaOccupy.containsPress ? theme.buttonHover : theme.buttonBg
                    border.color: theme.occupied
                    border.width: 2

                    Text {
                        anchors.centerIn: parent
                        text: "Ocupar mesa"
                        font { pixelSize: 14; bold: true }
                        color: theme.white
                    }

                    MouseArea {
                        id: mouseAreaOccupy
                        anchors.fill: parent
                        onClicked: {
                            tablePopup.occupyTable(tablePopup.tableId)
                            tablePopup.close()
                        }
                    }
                }
            }
        }
    }

    // === ANIMACIONES ===
    NumberAnimation {
        id: opacityAnimation
        target: tablePopup
        property: "opacity"
        from: 0
        to: 1
        duration: 200
        easing.type: Easing.OutQuad
    }

    NumberAnimation {
        id: opacityAnimationReverse
        target: tablePopup
        property: "opacity"
        from: 1
        to: 0
        duration: 150
        easing.type: Easing.InQuad
        onFinished: tablePopup.visible = false
    }

    NumberAnimation {
        id: scaleAnimation
        target: popupContainer
        property: "scale"
        from: 0.8
        to: 1
        duration: 200
        easing.type: Easing.OutBack
    }

    NumberAnimation {
        id: scaleAnimationReverse
        target: popupContainer
        property: "scale"
        from: 1
        to: 0.8
        duration: 150
        easing.type: Easing.InQuad
    }
}