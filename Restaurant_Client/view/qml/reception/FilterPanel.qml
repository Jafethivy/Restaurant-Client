// FilterPanel.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "filterComponents"
import "generalComponents"

Rectangle {
    id: root
    width: reception.panel_width + 40
    height: 650
    topLeftRadius: 10
    topRightRadius: 10
    color: "#37496D"
    border.color: clrBorder
    border.width: 2

    readonly property color clrBg:        "#3E527A"
    readonly property color clrSurface:   "#2A2F3C"
    readonly property color clrBorder:    "#A48265"
    readonly property color clrAccent:    "#847371"
    readonly property color clrText:      "#DDDBF1"
    readonly property color clrMuted:     "#F1F0F9"

    readonly property color clrPending:   "#F5A623"
    readonly property color clrComplete:  "#4CD964"
    readonly property color clrCancelled: "#FF3B30"
    readonly property color clrOverdue:   "#BF5AF2"

    function pad2(n) { return n < 10 ? "0" + n : "" + n }

    function formatDate(d) {
        if (!(d instanceof Date)) return ""
        return d.getFullYear() + "-" + pad2(d.getMonth() + 1) + "-" + pad2(d.getDate())
    }

    function collectFilters() {
        var s = []
        if (btnPending.active)   s.push(0)
        if (btnComplete.active)  s.push(1)
        if (btnCancelled.active) s.push(2)
        if (btnOverdue.active)   s.push(3)

        return {
            clientName: filters.nameEnabled   ? nameField.text.trim() : null,
            dateFrom:   filters.dateEnabled   ? formatDate(filters.dateFrom) : null,
            dateTo:     filters.dateEnabled   ? formatDate(filters.dateTo)   : null,
            timeFrom:   filters.timeEnabled   ? pad2(Math.floor(filters.minutesFrom / 60)) + ":" + pad2(filters.minutesFrom % 60) : null,
            timeTo:     filters.timeEnabled   ? pad2(Math.floor(filters.minutesTo   / 60)) + ":" + pad2(filters.minutesTo   % 60) : null,
            guestsMin:  filters.guestsEnabled ? filters.guestsMin : null,
            guestsMax:  filters.guestsEnabled ? filters.guestsMax : null,
            states:     filters.statesEnabled ? s : [],
            nameEnabled: filters.nameEnabled,
            dateEnabled: filters.dateEnabled,
            timeEnabled: filters.timeEnabled,
            guestsEnabled: filters.guestsEnabled,
            statesEnabled: filters.statesEnabled
        }
    }

    // ==========================================
    // MODELO DE DATOS
    // ==========================================
    QtObject {
        id: filters
        property bool nameEnabled:    true
        property bool dateEnabled:    true
        property bool timeEnabled:    true
        property bool guestsEnabled:  true
        property bool statesEnabled:  true

        property string clientName: ""
        property var    dateFrom:   new Date(2025, 0, 1)
        property var    dateTo:     new Date(2025, 0, 1)
        property int    minutesFrom: 0
        property int    minutesTo:   1439
        property int    guestsMin: 1
        property int    guestsMax: 20
        property var    states: []

        function syncFromUi() {
            clientName  = nameField.text.trim()
            dateFrom    = dateField.dateFrom
            dateTo      = dateField.dateTo
            minutesFrom = timeField.minutesFrom
            minutesTo   = timeField.minutesTo
            guestsMin   = comMin.value
            guestsMax   = comMax.value

            var s = []
            if (btnPending.active)   s.push("pending")
            if (btnComplete.active)  s.push("complete")
            if (btnCancelled.active) s.push("cancelled")
            if (btnOverdue.active)   s.push("overdue")
            states = s
        }

        function syncToUi() {
            nameField.text = clientName

            dateField.set(dateFrom.getDate(), dateFrom.getMonth() + 1, dateFrom.getFullYear(),
                          dateTo.getDate(),   dateTo.getMonth() + 1,   dateTo.getFullYear())

            timeField.hourFrom   = Math.floor(minutesFrom / 60)
            timeField.minuteFrom = minutesFrom % 60
            timeField.hourTo     = Math.floor(minutesTo / 60)
            timeField.minuteTo   = minutesTo % 60

            comMin.value = guestsMin
            comMax.value = guestsMax

            btnPending.active   = false
            btnComplete.active  = false
            btnCancelled.active = false
            btnOverdue.active   = false
        }

        function reset() {
            clientName  = ""
            dateFrom    = new Date(2025, 0, 1)
            dateTo      = new Date(2025, 0, 1)
            minutesFrom = 0
            minutesTo   = 1439
            guestsMin   = 1
            guestsMax   = 20
            states      = []
            syncToUi()
        }
    }

    // ==========================================
    // UI
    // ==========================================
    ColumnLayout {
        anchors { fill: parent; margins: 5 }
        spacing: 10

        // Header
        Item {
            Layout.fillWidth: true
            implicitHeight: 40

            Text {
                anchors.centerIn: parent
                text: "Filtros"
                font { pixelSize: 18; family: "Rockwell"; weight: Font.Normal }
                color: "#F1F0F9"
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: root.clrBorder }

        // Nombre
        ColumnLayout {
            spacing: 5
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 320
                Layout.leftMargin: 18
                Layout.rightMargin: 18

                SectionLabel {
                    text: "Nombre del cliente"
                    font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                    textColor: root.clrMuted
                }

                Item { Layout.fillWidth: true }

                MiniToggle {
                    id: nameToggle
                    checked: filters.nameEnabled
                    activeColor: root.clrAccent
                    inactiveColor: root.clrBorder
                    thumbColor: root.clrText
                    onToggled: {
                        filters.nameEnabled = checked
                        filters.syncFromUi()
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: 320
                Layout.fillWidth: true
                Layout.leftMargin: 18
                Layout.rightMargin: 18
                height: 38
                radius: 8
                color: root.clrSurface
                opacity: filters.nameEnabled ? 1.0 : 0.35
                enabled: filters.nameEnabled
                border.color: nameField.activeFocus ? root.clrAccent : root.clrBorder
                border.width: nameField.activeFocus ? 1.5 : 1
                Behavior on border.color { ColorAnimation { duration: 100 } }
                Behavior on opacity { NumberAnimation { duration: 120 } }

                TextField {
                    id: nameField
                    anchors { fill: parent; leftMargin: 12; rightMargin: 12 }
                    placeholderText: "Buscar por nombre..."
                    maximumLength: 40
                    font { pixelSize: 13; family: "Rockwell"; weight: Font.Normal }
                    color: root.clrText
                    placeholderTextColor: root.clrMuted
                    background: Item {}
                    leftPadding: 0; rightPadding: 0
                    verticalAlignment: TextInput.AlignVCenter
                    onTextChanged: filters.syncFromUi()
                }
            }
        }

        // Fecha
        DateRangeField {
            id: dateField
            Layout.fillWidth: true
            clrSurface: root.clrSurface
            clrBorder: root.clrBorder
            clrAccent: root.clrAccent
            clrText: root.clrText
            clrMuted: root.clrMuted
            onDatesChanged: filters.syncFromUi()
        }

        // Conexión EXPLÍCITA de la señal enabledChanged
        Connections {
            target: dateField
            function onToggledChanged(enabled) {
                filters.dateEnabled = enabled
                filters.syncFromUi()
            }
        }

        // Hora
        TimeRangeField {
            id: timeField
            Layout.fillWidth: true
            clrSurface: root.clrSurface
            clrBorder: root.clrBorder
            clrAccent: root.clrAccent
            clrText: root.clrText
            clrMuted: root.clrMuted
            onTimesChanged: filters.syncFromUi()
        }

        Connections {
            target: timeField
            function onToggledChanged(enabled) {
                filters.timeEnabled = enabled
                filters.syncFromUi()
            }
        }

        // Comensales
        ColumnLayout {
            spacing: 8
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 320
                Layout.leftMargin: 18
                Layout.rightMargin: 18

                SectionLabel {
                    text: "Comensales"
                    font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                    textColor: root.clrMuted
                }

                Item { Layout.fillWidth: true }

                MiniToggle {
                    id: guestsToggle
                    checked: filters.guestsEnabled
                    activeColor: root.clrAccent
                    inactiveColor: root.clrBorder
                    thumbColor: root.clrText
                    onToggled: {
                        filters.guestsEnabled = checked
                        filters.syncFromUi()
                    }
                }
            }

            RowLayout {
                spacing: 20
                Layout.alignment: Qt.AlignHCenter
                opacity: filters.guestsEnabled ? 1.0 : 0.35
                enabled: filters.guestsEnabled
                Behavior on opacity { NumberAnimation { duration: 120 } }

                ColumnLayout {
                    spacing: 5
                    Text {
                        text: "Minimo"
                        color: root.clrMuted
                        font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                        Layout.alignment: Qt.AlignHCenter
                    }
                    StyledSpinBox {
                        id: comMin
                        from: 1; to: 99; value: 1
                        implicitWidth: 100
                        clrSurface: root.clrSurface; clrBorder: root.clrBorder
                        clrAccent: root.clrAccent; clrText: root.clrText; clrMuted: root.clrMuted
                        onValueModified: {
                            if (value > comMax.value) comMax.value = value
                            filters.syncFromUi()
                        }
                    }
                }

                ColumnLayout {
                    spacing: 5
                    Text {
                        text: "Maximo"
                        color: root.clrMuted
                        font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                        Layout.alignment: Qt.AlignHCenter
                    }
                    StyledSpinBox {
                        id: comMax
                        from: 1; to: 99; value: 20
                        implicitWidth: 100
                        clrSurface: root.clrSurface; clrBorder: root.clrBorder
                        clrAccent: root.clrAccent; clrText: root.clrText; clrMuted: root.clrMuted
                        onValueModified: {
                            if (value < comMin.value) comMin.value = value
                            filters.syncFromUi()
                        }
                    }
                }
            }
        }

        // Estados
        ColumnLayout {
            spacing: 8
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 320
                Layout.leftMargin: 18
                Layout.rightMargin: 18

                SectionLabel {
                    text: "Estado de reserva"
                    font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
                    textColor: root.clrMuted
                }

                Item { Layout.fillWidth: true }

                MiniToggle {
                    id: statesToggle
                    checked: filters.statesEnabled
                    activeColor: root.clrAccent
                    inactiveColor: root.clrBorder
                    thumbColor: root.clrText
                    onToggled: {
                        filters.statesEnabled = checked
                        filters.syncFromUi()
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                opacity: filters.statesEnabled ? 1.0 : 0.35
                enabled: filters.statesEnabled
                Behavior on opacity { NumberAnimation { duration: 120 } }

                ToggleFilterButton {
                    id: btnPending
                    label: "PENDING"
                    activeColor: root.clrPending
                    surfaceColor: root.clrSurface
                    borderColor: root.clrBorder
                    mutedColor: root.clrMuted
                    onToggled: filters.syncFromUi()
                }
                ToggleFilterButton {
                    id: btnComplete
                    label: "COMPLETE"
                    activeColor: root.clrComplete
                    surfaceColor: root.clrSurface
                    borderColor: root.clrBorder
                    mutedColor: root.clrMuted
                    onToggled: filters.syncFromUi()
                }
                ToggleFilterButton {
                    id: btnCancelled
                    label: "CANCELLED"
                    activeColor: root.clrCancelled
                    surfaceColor: root.clrSurface
                    borderColor: root.clrBorder
                    mutedColor: root.clrMuted
                    onToggled: filters.syncFromUi()
                }
                ToggleFilterButton {
                    id: btnOverdue
                    label: "OVERDUE"
                    activeColor: root.clrOverdue
                    surfaceColor: root.clrSurface
                    borderColor: root.clrBorder
                    mutedColor: root.clrMuted
                    onToggled: filters.syncFromUi()
                }
            }
        }

        Item { Layout.fillHeight: true }

        Rectangle { Layout.fillWidth: true; height: 1; color: root.clrBorder }

        // Boton Aplicar
        Rectangle {
            Layout.fillWidth: true
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            height: 40
            Layout.alignment: Qt.AlignHCenter
            radius: 8
            color: applyMA.containsMouse ? Qt.lighter(root.clrAccent, 1.12) : root.clrAccent
            Behavior on color { ColorAnimation { duration: 120 } }

            Text {
                anchors.centerIn: parent
                text: "Aplicar filtros"
                font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
                color: "white"
            }

            MouseArea {
                id: applyMA
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                onClicked: {
                    filters.syncFromUi()
                    var data = root.collectFilters()
                    root.applied(data)
                    root.closeRequested()
                }
            }
        }

        CloseButton {
            onClicked: root.closeRequested()
        }
    }

    signal applied(var filterData)
    signal closeRequested()

    function clearAll() { filters.reset() }
    function getFilters() { return collectFilters() }
}