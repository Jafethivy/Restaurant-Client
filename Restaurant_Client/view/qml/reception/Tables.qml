import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "tableComponents"

Item {
    id: root
    anchors.fill: parent

    // ============================================================
    // PALETA DE COLORES - QtObject reutilizable
    // ============================================================
    QtObject {
        id: theme
        readonly property color available:  "#4CAF50"
        readonly property color reserved:   "#FFC107"
        readonly property color occupied:   "#F44336"
        readonly property color unknown:    "#9E9E9E"
        readonly property color background: "#293651"
        readonly property color title:      "#DDDBF1"
        readonly property color tooltipBg:  "#2d2d44"
        readonly property color white:      "#ffffff"
        readonly property color shadow:     "#000000"
    }

    // Propiedades de configuración de animación
    readonly property int stageDelay:    35
    readonly property int stageDuration: 200

    // Función para obtener color según estado
    function getColor(state) {
        if (state === 0) return theme.available
        if (state === 1) return theme.reserved
        if (state === 2) return theme.occupied
        return theme.unknown
    }
    // Función para obtener texto de estado
    function getStatusText(state) {
        if (state === 0) return "Disponible"
        if (state === 1) return "Reservado"
        if (state === 2) return "Ocupado"
        return "Desconocido"
    }
    // Modelo de mesas
    ListModel {
        id: tableModel
    }
    // Funcion para cargar mesas (desde base de datos o manual)
    function loadTables(tablesData) {
        tableModel.clear()
        for (var i = 0; i < tablesData.length; i++) {
            tableModel.append({
                "tableId": tablesData[i].tableId,
                "state": tablesData[i].status
            })
        }
    }
    // Funcion para actualizar el estado de una mesa específica
    function updateTableState(table) {
        tableModel.setProperty(table.tableId - 1, "state", table.status)
    }

    // Función para marcar una mesa como disponible (desde C++)
    function setTableAvailable(tableId) {
        updateTableState(tableId, 0)
    }

    signal reserveTableA(int tableId)
    signal occupyTableA(int tableId)

    // ============================================================
    // POPUP EXTERNO - Instancia compartida
    // ============================================================
    TablePopup {
        id: tablePopup
        anchors.fill: parent 

        onReserveTable: (tableId) => {
            root.reserveTableA(tableId)
        }

        onOccupyTable: (tableId) => {
            root.occupyTableA(tableId)
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: theme.background
        radius: 8
        clip: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            Text {
                text: "Mesas del Restaurante"
                font { pixelSize: 24; family: "Rockwell"; weight: Font.Normal }
                color: theme.title
                Layout.alignment: Qt.AlignHCenter
            }

            GridLayout {
                Layout.alignment: Qt.AlignHCenter
                columns: 8
                rowSpacing: 45
                columnSpacing: 30

                Repeater {
                    model: tableModel

                    Rectangle {
                        id: tableItem
                        width: 60
                        height: 60
                        radius: 35
                        color: root.getColor(model.state)
                        border.width: 2
                        border.color: theme.white

                        opacity: 0
                        scale: 0.7

                        // Animación de entrada escalonada
                        Timer {
                            interval: index * root.stageDelay
                            running: true
                            repeat: false
                            onTriggered: entryAnim.start()
                        }

                        ParallelAnimation {
                            id: entryAnim
                            NumberAnimation {
                                target: tableItem
                                property: "opacity"
                                from: 0; to: 1
                                duration: root.stageDuration
                                easing.type: Easing.OutBack
                            }
                            NumberAnimation {
                                target: tableItem
                                property: "scale"
                                from: 0.7; to: 1.0
                                duration: root.stageDuration
                                easing.type: Easing.OutBack
                            }
                        }

                        // Sombra
                        Rectangle {
                            anchors.fill: parent
                            radius: 35
                            color: theme.shadow
                            opacity: 0.3
                            z: -1
                            anchors.margins: -4
                        }

                        // Número de mesa (usando tableId del modelo)
                        Text {
                            anchors.centerIn: parent
                            text: model.tableId
                            font.pixelSize: 20
                            font.bold: true
                            color: theme.white
                        }

                        // Tooltip de estado
                        Rectangle {
                            id: statusTooltip
                            anchors.top: parent.bottom
                            anchors.topMargin: 8
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: statusText.width + 16
                            height: 24
                            radius: 4
                            color: theme.tooltipBg
                            border.color: tableItem.color
                            border.width: 1
                            opacity: 0
                            visible: opacity > 0

                            Text {
                                id: statusText
                                anchors.centerIn: parent
                                text: root.getStatusText(model.state)
                                font.pixelSize: 11
                                color: theme.white
                            }

                            Behavior on opacity {
                                NumberAnimation { duration: 150 }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered: {
                                statusTooltip.opacity = 1
                                tableItem.scale = 1.1
                            }
                            onExited: {
                                statusTooltip.opacity = 0
                                tableItem.scale = 1.0
                            }
                            onClicked: {
                                tablePopup.tableId = model.tableId
                                tablePopup.currentState = model.state
                                tablePopup.open()
                            }
                        }

                        Behavior on scale {
                            NumberAnimation { duration: 150 }
                        }
                    }
                }
            }

            // Leyenda de estados
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 20

                Repeater {
                    model: [
                        { color: theme.available, text: "Disponible" },
                        { color: theme.reserved,  text: "Reservado"  },
                        { color: theme.occupied,  text: "Ocupado"    }
                    ]

                    RowLayout {
                        spacing: 6
                        Rectangle {
                            width: 16; height: 16; radius: 8
                            color: modelData.color
                        }
                        Text {
                            text: modelData.text
                            font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                            color: theme.white
                        }
                    }
                }
            }
        }
    }
}