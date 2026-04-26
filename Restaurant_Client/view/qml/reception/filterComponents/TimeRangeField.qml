// TimeRangeField.qml
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

    property color clrSurface: "#1F2128"
    property color clrBorder:  "#2A2D36"
    property color clrAccent:  "#4F8EF7"
    property color clrText:    "#E8EAF0"
    property color clrMuted:   "#6B7080"

    property alias hourFrom: fromRow.hourValue
    property alias minuteFrom: fromRow.minuteValue
    property alias hourTo: toRow.hourValue
    property alias minuteTo: toRow.minuteValue

    property bool filterEnabled: true

    signal timesChanged()
    signal toggledChanged(bool enabled)

    RowLayout {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: 320
        Layout.leftMargin: 18
        Layout.rightMargin: 18

        SectionLabel {
            id: sectionLabel
            text: "Rango de hora"
            font { pixelSize: 12; family: "Rockwell"; weight: Font.Normal }
            textColor: root.clrMuted
        }

        Item { Layout.fillWidth: true }

        MiniToggle {
            id: timeToggle
            checked: root.filterEnabled
            activeColor: root.clrAccent
            inactiveColor: root.clrBorder
            thumbColor: root.clrText
            onToggled: {
                root.filterEnabled = checked
                root.toggledChanged(checked)
                root.timesChanged()
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

        component TimeRow : RowLayout {
            property alias prefixText: prefixTextItem.text
            property alias hourValue: h.value
            property alias minuteValue: m.value

            spacing: 8
            Layout.alignment: Qt.AlignHCenter

            Text {
                id: prefixTextItem
                color: root.clrMuted
                font.pixelSize: 12
            }

            StyledSpinBox {
                id: h
                from: 0; to: 23
                implicitWidth: 90
                clrSurface: root.clrSurface; clrBorder: root.clrBorder
                clrAccent: root.clrAccent; clrText: root.clrText; clrMuted: root.clrMuted
                onValueModified: root.timesChanged()
            }

            Text { text: ":"; color: root.clrMuted; font.pixelSize: 16; font.weight: Font.Bold }

            StyledSpinBox {
                id: m
                from: 0; to: 59
                implicitWidth: 90
                clrSurface: root.clrSurface; clrBorder: root.clrBorder
                clrAccent: root.clrAccent; clrText: root.clrText; clrMuted: root.clrMuted
                onValueModified: root.timesChanged()
            }
        }

        TimeRow {
            id: fromRow
            prefixText: "Desde"
            hourValue: 0
            minuteValue: 0
        }

        TimeRow {
            id: toRow
            prefixText: "Hasta"
            hourValue: 23
            minuteValue: 59
        }
    }

    property int minutesFrom: hourFrom * 60 + minuteFrom
    property int minutesTo: hourTo * 60 + minuteTo

    onMinutesToChanged: {
        if (minutesTo < minutesFrom) {
            toRow.hourValue = fromRow.hourValue
            toRow.minuteValue = fromRow.minuteValue
        }
    }

    function clear() {
        fromRow.hourValue = 0; fromRow.minuteValue = 0
        toRow.hourValue = 23; toRow.minuteValue = 59
    }

    function set(fromMinutes, toMinutes) {
        fromRow.hourValue = Math.floor(fromMinutes / 60)
        fromRow.minuteValue = fromMinutes % 60
        toRow.hourValue = Math.floor(toMinutes / 60)
        toRow.minuteValue = toMinutes % 60
    }
}