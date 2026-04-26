import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    anchors.fill: parent
    color: "#dddbf1"
    radius: 8
    clip: true

    ListModel { id: reservationsModel }
    property int selectedIndex: -1

    property color headerColor: "#2c3e50"
    property color rowEvenColor: "#DDDBF1"
    property color rowOddColor: "#C7C4E8"
    property color textColor: "#2c3e50"
    property color borderColor: "#bdc3c7"

    readonly property int colId:         40
    readonly property int colFecha:      80
    readonly property int colHora:       40
    readonly property int colComensales: 60
    readonly property int sidePad:       15

    readonly property int headerDuration:   280
    readonly property int listFadeDelay:    180
    readonly property int listFadeDuration: 160
    readonly property int stageDelay:       40
    readonly property int stageDuration:    200
    readonly property int rowsStartOffset: headerDuration + listFadeDelay + listFadeDuration
    
    QtObject {
        id: statusColors
        readonly property color pending:   "#F1C40F"
        readonly property color completed: "#2ECC71"
        readonly property color cancelled: "#E74C3C"
        readonly property color expired:   "#95A5A6"

        function colorForStatus(s) {
            switch (s) {
                case 0:  return pending
                case 1:  return completed
                case 2:  return cancelled
                case 3:  return expired
                default: return expired
            }
        }
    }

    signal reservationRemoved(var index)

    function onDeleteRequested(index, data) {
        removeReservation(index)
        reservationRemoved(data)
    }

    function addReservation(reservation) {
        reservationsModel.append({
            id_resv:    reservation.id_resv    ?? "",
            name_resv:  reservation.name_resv  ?? "",
            date_resv:  reservation.date_resv  ?? "",
            time_resv:  reservation.time_resv  ?? "",
            guest_resv: reservation.guest_resv ?? 0,
            status:     reservation.status     ?? 0
        })
    }

    function removeReservation(index) {
        reservationsModel.remove(index)
        if (root.selectedIndex === index) {
            root.selectedIndex = -1
        } else if (root.selectedIndex > index) {
            root.selectedIndex--
        }
    }

    function update_resvReservations(new_reservation) {
        if (!new_reservation || Object.keys(new_reservation).length === 0) {
            console.warn("update_resvReservations: datos vacios")
            return
        }

        for (var i = 0; i < reservationsModel.count; i++) {
            if (reservationsModel.get(i).id_resv === new_reservation.id_resv) {
                var existing = reservationsModel.get(i)
                var updated = {
                    id_resv:    new_reservation.id_resv    ?? existing.id_resv,
                    name_resv:  new_reservation.name_resv  ?? existing.name_resv,
                    date_resv:  new_reservation.date_resv  ?? existing.date_resv,
                    time_resv:  new_reservation.time_resv  ?? existing.time_resv,
                    guest_resv: new_reservation.guest_resv ?? existing.guest_resv,
                    status:     new_reservation.status     ?? existing.status
                }
                reservationsModel.set(i, updated)
                return
            }
        }
        console.warn("update_resvReservations: id_resv no encontrado:", new_reservation.id_resv)
    }

    function loadReservations(list) {
        reservationsModel.clear()
        if (!list || list.length === 0) {
            console.warn("loadReservations: lista vacía")
            return
        }
        for (var i = 0; i < list.length; i++) {
            var item = list[i]
            reservationsModel.append({
                id_resv:    item.id_resv    ?? "",
                name_resv:  item.name_resv  ?? "",
                date_resv:  item.date_resv  ?? "",
                time_resv:  item.time_resv  ?? "",
                guest_resv: item.guest_resv ?? 0,
                status:     item.status     ?? 0
            })
        }
    }

    function loadAdvanced(list) {
        reservationsModel.clear()
        root.selectedIndex = -1
        if (!list || list.length === 0) {
            console.warn("loadAdvanced: lista vacía")
            return
        }
        for (var i = 0; i < list.length; i++) {
            var item = list[i]
            reservationsModel.append({
                id_resv:    item.id_resv    ?? "",
                name_resv:  item.name_resv  ?? "",
                date_resv:  item.date_resv  ?? "",
                time_resv:  item.time_resv  ?? "",
                guest_resv: item.guest_resv ?? 0,
                status:     item.status     ?? 0
            })
        }
    }

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            id: tableHeader
            Layout.fillWidth: true
            height: 50
            color: headerColor
            topLeftRadius: 8
            topRightRadius: 8

            opacity: 0
            y: -20

            Component.onCompleted: headerAnim.start()

            ParallelAnimation {
                id: headerAnim
                NumberAnimation {
                    target: tableHeader; property: "opacity"
                    from: 0; to: 1
                    duration: root.headerDuration; easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    target: tableHeader; property: "y"
                    from: -20; to: 0
                    duration: root.headerDuration; easing.type: Easing.OutCubic
                }
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin:  root.sidePad
                anchors.rightMargin: root.sidePad
                spacing: 10

                Text {
                    Layout.preferredWidth: root.colId
                    text: "ID"
                    color: "#DDDBF1"
                    font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
                    verticalAlignment:   Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    Layout.fillWidth: true
                    text: "A NOMBRE"
                    color: "#DDDBF1"
                    font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
                    verticalAlignment: Text.AlignVCenter
                }
                Text {
                    Layout.preferredWidth: root.colFecha
                    text: "FECHA"
                    color: "#DDDBF1"
                    font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
                    verticalAlignment:   Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    Layout.preferredWidth: root.colHora
                    text: "HORA"
                    color: "#DDDBF1"
                    font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
                    verticalAlignment:   Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    Layout.preferredWidth: root.colComensales
                    text: "USUARIOS"
                    color: "#DDDBF1"
                    font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
                    verticalAlignment:   Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
        
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: reservationsModel
            spacing: 1
            bottomMargin: 2

            opacity: 0

            Timer {
                interval: root.headerDuration + root.listFadeDelay
                running: true; repeat: false
                onTriggered: listFadeAnim.start()
            }

            NumberAnimation {
                id: listFadeAnim
                target: listView; property: "opacity"
                from: 0; to: 1
                duration: root.listFadeDuration; easing.type: Easing.OutCubic
            }

            delegate: Rectangle {
                id: delegateRoot
                width: ListView.view.width
                height: 60
                color: index % 2 === 0 ? rowEvenColor : rowOddColor
                clip: true
                bottomLeftRadius:  (index === ListView.view.count - 1) ? root.radius : 0
                bottomRightRadius: (index === ListView.view.count - 1) ? root.radius : 0

                opacity: 0
                transform: Translate { id: slideTransform; y: 12 }

                Timer {
                    id: entryTimer
                    interval: root.rowsStartOffset + (index * root.stageDelay)
                    running: true; repeat: false
                    onTriggered: entryAnim.start()
                }

                ParallelAnimation {
                    id: entryAnim
                    NumberAnimation {
                        target: delegateRoot; property: "opacity"
                        from: 0; to: 1
                        duration: root.stageDuration; easing.type: Easing.OutCubic
                    }
                    NumberAnimation {
                        target: slideTransform; property: "y"
                        to: 0
                        duration: root.stageDuration; easing.type: Easing.OutCubic
                    }
                }
                
                Rectangle {
                    anchors.fill: parent
                    color: mouseArea.containsMouse ? "#3498db" : "transparent"
                    opacity: mouseArea.containsMouse ? 0.1 : 0
                    Behavior on opacity { NumberAnimation { duration: 150 } }
                }
                
                Rectangle {
                    id: statusBar
                    anchors.left:   parent.left
                    anchors.top:    parent.top
                    anchors.leftMargin: 10
                    height: 10
                    width: 6
                    z: 2
                    color: statusColors.colorForStatus(model.status)

                    Behavior on color { ColorAnimation { duration: 200 } }
                }
                
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin:  statusBar.width + root.sidePad
                    anchors.rightMargin: root.sidePad
                    spacing: 8

                    Text {
                        Layout.preferredWidth: root.colId
                        text: model.id_resv !== undefined ? model.id_resv : "\x97"
                        color: mouseArea.containsMouse ? "#2980b9" : textColor
                        font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                        verticalAlignment:   Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Text {
                        Layout.fillWidth: true
                        text: model.name_resv || "\x97"
                        color: mouseArea.containsMouse ? "#2980b9" : textColor
                        font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    Text {
                        Layout.preferredWidth: root.colFecha
                        text: model.date_resv || "\x97"
                        color: mouseArea.containsMouse ? "#2980b9" : textColor
                        font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                        verticalAlignment:   Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Text {
                        Layout.preferredWidth: root.colHora
                        text: model.time_resv || "\x97"
                        color: mouseArea.containsMouse ? "#2980b9" : textColor
                        font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                        verticalAlignment:   Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Text {
                        Layout.preferredWidth: root.colComensales
                        text: model.guest_resv !== undefined ? model.guest_resv : "\x97"
                        color: mouseArea.containsMouse ? "#2980b9" : textColor
                        font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                        verticalAlignment:   Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
                
                Rectangle {
                    id: deletePanel
                    anchors.top:    parent.top
                    anchors.bottom: parent.bottom
                    anchors.right:  parent.right
                    width: root.selectedIndex === index ? 120 : 0
                    color: "#e74c3c"
                    clip: true
                    z: 1

                    Behavior on width { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }

                    Text {
                        anchors.centerIn: parent
                        text: "Eliminar"
                        color: "white"
                        font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
                        visible: root.selectedIndex === index
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.onDeleteRequested(index, model.id_resv)
                    }
                }

                // Bottom border
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: borderColor
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        root.selectedIndex = (root.selectedIndex === index) ? -1 : index
                    }
                }
            }

            Rectangle {
                visible: listView.count === 0
                anchors.fill: parent
                color: "#dddbf1"
                bottomLeftRadius: 18
                bottomRightRadius: 18

                Column {
                    anchors.centerIn: parent
                    spacing: 10

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Atenci\u00F3n"
                        font { pixelSize: 32; family: "Rockwell"; weight: Font.Normal }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "No hay reservaciones"
                        color: "#7f8c8d"
                        font { pixelSize: 20; family: "Rockwell"; weight: Font.Normal }
                    }
                }
            }
        }

        Rectangle{
            height:8
            Layout.fillWidth: true
            color: "transparent"
            z: 100

        }
    }
}
