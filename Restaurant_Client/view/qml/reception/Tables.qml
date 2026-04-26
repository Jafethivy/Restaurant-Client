import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    anchors.fill: parent
    
    readonly property int stageDelay:    35
    readonly property int stageDuration: 200

    function getColor(state) {
        if (state === 0) return "#4CAF50"
        if (state === 1) return "#FFC107"
        if (state === 2) return "#F44336"
        return "#9E9E9E"
    }

    function getStatusText(state) {
        if (state === 0) return "Disponible"
        if (state === 1) return "Reservado"
        if (state === 2) return "Ocupado"
        return "Desconocido"
    }

    ListModel {
        id: tableModel
        ListElement { state: 0 }
        ListElement { state: 0 }
        ListElement { state: 1 }
        ListElement { state: 0 }
        ListElement { state: 2 }
        ListElement { state: 2 }
        ListElement { state: 1 }
        ListElement { state: 0 }
        ListElement { state: 0 }
        ListElement { state: 0 }
        ListElement { state: 0 }
        ListElement { state: 0 }
        ListElement { state: 1 }
        ListElement { state: 0 }
        ListElement { state: 2 }
        ListElement { state: 2 }
        ListElement { state: 1 }
        ListElement { state: 0 }
        ListElement { state: 0 }
        ListElement { state: 0 }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#293651"
        radius: 8
        clip: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            Text {
                text: "Mesas del Restaurante"
                font { pixelSize: 24; family: "Rockwell"; weight: Font.Normal }
                color: "#DDDBF1"
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
                        border.color: "#ffffff"
                        
                        opacity: 0
                        scale: 0.7

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

                        Rectangle {
                            anchors.fill: parent
                            radius: 35
                            color: "#000000"
                            opacity: 0.3
                            z: -1
                            anchors.margins: -4
                        }

                        Text {
                            anchors.centerIn: parent
                            text: index + 1
                            font.pixelSize: 20
                            font.bold: true
                            color: "#ffffff"
                        }

                        Rectangle {
                            id: statusTooltip
                            anchors.top: parent.bottom
                            anchors.topMargin: 8
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: statusText.width + 16
                            height: 24
                            radius: 4
                            color: "#2d2d44"
                            border.color: tableItem.color
                            border.width: 1
                            opacity: 0
                            visible: opacity > 0

                            Text {
                                id: statusText
                                anchors.centerIn: parent
                                text: root.getStatusText(model.state)
                                font.pixelSize: 11
                                color: "#ffffff"
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
                                tableModel.setProperty(index, "state", (model.state + 1) % 3)
                            }
                        }

                        Behavior on scale {
                            NumberAnimation { duration: 150 }
                        }
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 20

                Repeater {
                    model: [
                        { color: "#4CAF50", text: "Disponible" },
                        { color: "#FFC107", text: "Reservado"  },
                        { color: "#F44336", text: "Ocupado"    }
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
                            color: "#ffffff"
                        }
                    }
                }
            }
        }
    }
}
