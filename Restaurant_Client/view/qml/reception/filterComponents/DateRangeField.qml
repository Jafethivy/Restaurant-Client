// DateRangeField.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "."

ColumnLayout {
    id: root
    spacing: 8
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignHCenter
    
    property alias label: sectionLabel.text
    property alias prefixFrom: fromRow.prefixText
    property alias prefixTo: toRow.prefixText

    property color clrSurface:   "#383F51"
    property color clrBorder:    "#2A2D36"
    property color clrAccent:    "#4F8EF7"
    property color clrText:      "#E8EAF0"
    property color clrMuted:     "#6B7080"

    property alias dayFrom: fromRow.dayValue
    property alias monthFrom: fromRow.monthValue
    property alias yearFrom: fromRow.yearValue
    property alias dayTo: toRow.dayValue
    property alias monthTo: toRow.monthValue
    property alias yearTo: toRow.yearValue

    property date dateFrom: new Date(2026, 0, 1)
    property date dateTo: new Date(2026, 0, 1)
    
    property bool filterEnabled: true

    signal datesChanged()
    signal toggledChanged(bool enabled)
    
    RowLayout {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: 320
        Layout.leftMargin: 18
        Layout.rightMargin: 18

        SectionLabel {
            id: sectionLabel
            text: "Rango de fechas"
            font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
            textColor: root.clrMuted
        }

        Item { Layout.fillWidth: true }

        MiniToggle {
            id: dateToggle
            checked: root.filterEnabled
            activeColor: root.clrAccent
            inactiveColor: root.clrBorder
            thumbColor: root.clrText
            onToggled: {
                root.filterEnabled = checked
                root.toggledChanged(checked)
                root.datesChanged()
            }
        }
    }
    
    ColumnLayout {
        id: contentContainer
        spacing: 8
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        opacity: root.filterEnabled ? 1.0 : 0.35
        enabled: root.filterEnabled

        Behavior on opacity { NumberAnimation { duration: 120 } }

        function daysInMonth(month, year) {
            return new Date(year, month, 0).getDate()
        }

        function clampDay(spinBox, month, year) {
            var maxDay = daysInMonth(month, year)
            if (spinBox.value > maxDay) spinBox.value = maxDay
            spinBox.to = maxDay
        }

        component DateRow : RowLayout {
            id: row
            property alias prefixText: prefixTextItem.text
            property alias dayValue: d.value
            property alias monthValue: m.value
            property alias yearValue: y.value
            property alias daySpinBox: d

            spacing: 8
            Layout.alignment: Qt.AlignHCenter

            Text {
                id: prefixTextItem
                color: root.clrMuted
                font.pixelSize: 12
            }

            StyledSpinBox {
                id: d
                from: 1
                to: contentContainer.daysInMonth(m.value, y.value)
                implicitWidth: 90
                clrSurface: root.clrSurface
                clrBorder: root.clrBorder
                clrAccent: root.clrAccent
                clrText: root.clrText
                clrMuted: root.clrMuted
                onValueModified: root.datesChanged()
            }

            Text { text: "/"; color: root.clrMuted; font.pixelSize: 14 }

            StyledSpinBox {
                id: m
                from: 1; to: 12
                implicitWidth: 90
                clrSurface: root.clrSurface
                clrBorder: root.clrBorder
                clrAccent: root.clrAccent
                clrText: root.clrText
                clrMuted: root.clrMuted
                onValueModified: root.datesChanged()
            }

            Text { text: "/"; color: root.clrMuted; font.pixelSize: 14 }

            StyledSpinBox {
                id: y
                from: 0; to: 2099
                implicitWidth: 100
                clrSurface: root.clrSurface
                clrBorder: root.clrBorder
                clrAccent: root.clrAccent
                clrText: root.clrText
                clrMuted: root.clrMuted
                onValueModified: root.datesChanged()
            }
        }

        DateRow {
            id: fromRow
            prefixText: "Desde"
            onMonthValueChanged: contentContainer.clampDay(fromRow.daySpinBox, monthValue, yearValue)
            onYearValueChanged:  contentContainer.clampDay(fromRow.daySpinBox, monthValue, yearValue)
        }

        DateRow {
            id: toRow
            prefixText: "Hasta"
            onMonthValueChanged: contentContainer.clampDay(toRow.daySpinBox, monthValue, yearValue)
            onYearValueChanged:  contentContainer.clampDay(toRow.daySpinBox, monthValue, yearValue)
        }
    }

    onDayFromChanged:     updateDateFrom()
    onMonthFromChanged:   updateDateFrom()
    onYearFromChanged:    updateDateFrom()
    onDayToChanged:       updateDateTo()
    onMonthToChanged:     updateDateTo()
    onYearToChanged:      updateDateTo()

    function updateDateFrom() {
        dateFrom = new Date(yearFrom, monthFrom - 1, dayFrom)
    }

    function updateDateTo() {
        dateTo = new Date(yearTo, monthTo - 1, dayTo)
        if (dateTo < dateFrom) {
            toRow.dayValue = fromRow.dayValue
            toRow.monthValue = fromRow.monthValue
            toRow.yearValue = fromRow.yearValue
        }
    }

    function clear() {
        fromRow.dayValue = 1; fromRow.monthValue = 1; fromRow.yearValue = 2025
        toRow.dayValue = 1; toRow.monthValue = 1; toRow.yearValue = 2025
    }

    function set(fromD, fromM, fromY, toD, toM, toY) {
        fromRow.dayValue = fromD; fromRow.monthValue = fromM; fromRow.yearValue = fromY
        toRow.dayValue = toD; toRow.monthValue = toM; toRow.yearValue = toY
    }
}