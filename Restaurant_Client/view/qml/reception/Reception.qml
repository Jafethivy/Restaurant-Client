import QtQuick.Controls.Basic
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "generalComponents"

Rectangle {
    id: root
    width: 300
    height: 440
    color: clr.bgRoot
    topLeftRadius: 10
    topRightRadius: 10
    border.color: clr.borderDefault
    border.width: 2

    readonly property var clr: QtObject {
        readonly property color bgRoot:            "#3E527A"
        readonly property color bgDivider:         "#DDDBF1"
        readonly property color bgHeader:          "#293651"
        readonly property color bgField:           "#2A2F3C"
        readonly property color mnPlaceholder:     "#2A2F3C"
        readonly property color btnPrimaryNormal:  "#E3E2F3"
        readonly property color btnPrimaryHover:   "#B9B6E2"
        readonly property color btnPrimaryBorder:  "#C7C4E8"
        readonly property color btnPrimaryText:    "#383F51"
        readonly property color btnActionNormal:   "#6E605E"
        readonly property color btnActionHover:    "#847371"
        readonly property color btnSecondaryNormal:"#6E605E"
        readonly property color btnSecondaryHover: "#847371"
        readonly property color btnDisabled:       "#333333"
        readonly property color btnActionBorder:   "#555555"
        readonly property color borderDefault:     "#A48265"
        readonly property color borderFocus:       "#666666"
        readonly property color borderValid:       "#2ecc71"
        readonly property color borderError:       "#e74c3c"
        readonly property color textWhite:         "white"
        readonly property color textMuted:         "#DDDBF1"
        readonly property color textPlaceholder:   "#666666"
        readonly property color textError:         "#e74c3c"
    }

    property int currentView: 0

    signal reservationCreated(var data)
    signal reservationEdited(var data)
    signal closeRequested()

    StackLayout {
        anchors.fill: parent
        currentIndex: root.currentView

        //  Vista 0: Menu principal 
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12

            Text {
                text: "Reservaciones"
                color: clr.textWhite
                font { pixelSize: 20; family: "Rockwell"; weight: Font.Normal }
                Layout.topMargin: 20
                Layout.alignment: Qt.AlignHCenter
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                height: 1
                color: clr.bgDivider
            }

            Item { Layout.fillHeight: true }

            ItemDelegate {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.alignment: Qt.AlignHCenter
                height: 20
                contentItem: Text {
                    text: "Crear Reservaci\u00F3n"
                    color: clr.btnPrimaryText
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font { pixelSize: 16; family: "Rockwell"; weight: Font.Normal }
                }
                background: Rectangle {
                    color: parent.hovered ? clr.btnPrimaryHover : clr.btnPrimaryNormal
                    radius: 10
                    border.color: clr.btnPrimaryBorder
                    border.width: 1
                }
                onClicked: root.currentView = 1
            }

            ItemDelegate {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.alignment: Qt.AlignHCenter
                height: 20
                contentItem: Text {
                    text: "Editar Reservaci\u00F3n"
                    color: clr.btnPrimaryText
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font { pixelSize: 16; family: "Rockwell"; weight: Font.Normal }
                }
                background: Rectangle {
                    color: parent.hovered ? clr.btnPrimaryHover : clr.btnPrimaryNormal
                    radius: 10
                    border.color: clr.btnPrimaryBorder
                    border.width: 1
                }
                onClicked: root.currentView = 2
            }

            Item { Layout.fillHeight: true }
            CloseButton{
                Layout.bottomMargin: 4
                onClicked: root.closeRequested()
            }
        }

        //  Vista 1: Crear reservacion 
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            Rectangle {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                height: 40
                color: clr.bgHeader
                radius: 10
                Text {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    text: "Crear Reservaci\u00F3n"
                    color: clr.textMuted
                    font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.currentView = 0
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 12
                Layout.rightMargin: 12
                spacing: 6

                MenuField {
                    id: createName
                    label: "A nombre de"
                    placeholder: "Nombre"
                    maxLength: 40
                    errorMsg: "Requerido, max 40 caracteres"
                    validatorObj: RegularExpressionValidator {
                        regularExpression: /^.{1,40}$/
                    }
                }
                MenuField {
                    id: createDate
                    label: "Fecha"
                    placeholder: "YYYY-MM-DD"
                    errorMsg: "Formato requerido: YYYY-MM-DD"
                    formatMode: "date"
                    maskPattern: "^\\d{4}-\\d{2}-\\d{2}$"
                }
                MenuField {
                    id: createTime
                    label: "Hora"
                    placeholder: "HH:MM"
                    errorMsg: "Hora invalida: HH(0-23):MM(0-59)"
                    formatMode: "time"
                    maskPattern: "^(0[0-9]|1[0-9]|2[0-3]):(0[0-9]|[1-5][0-9])$"
                }
                MenuField {
                    id: createGuests
                    label: "Comensales"
                    placeholder: "1"
                    errorMsg: "Numero entre 1 y 999"
                    validatorObj: IntValidator { bottom: 1; top: 999 }
                }

                Rectangle { height: 2; color: "transparent" }

                MenuButton {
                    Layout.fillWidth: true
                    label: "Guardar"
                    enabled: createName.isValid && createDate.isValid && createTime.isValid && createGuests.isValid
                    onAction: {
                        var data = {
                            name_resv:  createName.value,
                            date_resv:  createDate.value,
                            time_resv:  createTime.value,
                            guest_resv: parseInt(createGuests.value) || 0,
                            status_resv: 0
                        }
                        reservationCreated(data)
                        createName.clear(); createDate.clear()
                        createTime.clear(); createGuests.clear()
                        root.currentView = 0
                    }
                }
                MenuButton {
                    Layout.fillWidth: true
                    label: "Volver"
                    secondary: true
                    onAction: {
                        createName.clear(); createDate.clear()
                        createTime.clear(); createGuests.clear()
                        root.currentView = 0
                    }
                }
            }

            Item { Layout.fillHeight: true }
            CloseButton{
                Layout.bottomMargin: 4
                onClicked: root.closeRequested()
            }
        }

        //  Vista 2: Editar reservacion 
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            Rectangle {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                height: 42
                color: clr.bgHeader
                radius: 10
                Text {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    text: "Editar Reservaci\u00F3n"
                    color: clr.textMuted
                    font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.currentView = 0
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 12
                Layout.rightMargin: 12
                spacing: 6

                MenuField {
                    id: editId
                    label: "ID"
                    placeholder: "1"
                    errorMsg: "ID invalido"
                    required: true
                    validatorObj: IntValidator { bottom: 1; top: 999999 }
                }
                MenuField {
                    id: editName
                    label: "A nombre de"
                    placeholder: "Nombre (opcional)"
                    maxLength: 40
                    errorMsg: "Max 40 caracteres"
                    required: false
                    validatorObj: RegularExpressionValidator {
                        regularExpression: /^.{1,40}$/
                    }
                }
                MenuField {
                    id: editDate
                    label: "Fecha"
                    placeholder: "YYYY-MM-DD (opcional)"
                    errorMsg: "Formato requerido: YYYY-MM-DD"
                    required: false
                    formatMode: "date"
                    maskPattern: "^\\d{4}-\\d{2}-\\d{2}$"
                }
                MenuField {
                    id: editTime
                    label: "Hora"
                    placeholder: "HH:MM (opcional)"
                    errorMsg: "Hora invalida: HH(0-23):MM(0-59)"
                    required: false
                    formatMode: "time"
                    maskPattern: "^(0[0-9]|1[0-9]|2[0-3]):(0[0-9]|[1-5][0-9])$"
                }
                MenuField {
                    id: editGuests
                    label: "Comensales"
                    placeholder: "1-999 (opcional)"
                    errorMsg: "Numero entre 1 y 999"
                    required: false
                    validatorObj: IntValidator { bottom: 1; top: 999 }
                }

                MenuButton {
                    Layout.fillWidth: true
                    label: "Guardar cambios"
                    enabled: editId.isValid && editName.isValid && editDate.isValid && editTime.isValid && editGuests.isValid
                    onAction: {
                        var data = {
                            id_resv:    parseInt(editId.value) || 0,
                            name_resv:  editName.value,
                            date_resv:  editDate.value,
                            time_resv:  editTime.value,
                            guest_resv: parseInt(editGuests.value) || 0,
                            status_resv: 0
                        }
                        reservationEdited(data)
                        editId.clear(); editName.clear(); editDate.clear()
                        editTime.clear(); editGuests.clear()
                        root.currentView = 0
                    }
                }
                MenuButton {
                    Layout.fillWidth: true
                    label: "Volver"
                    secondary: true
                    onAction: {
                        editId.clear(); editName.clear(); editDate.clear()
                        editTime.clear(); editGuests.clear()
                        root.currentView = 0
                    }
                }
            }

            Item { Layout.fillHeight: true }

            CloseButton{
                Layout.bottomMargin: 4
                onClicked: root.closeRequested()
            }
        }
    }

    //  Componentes internos 

    component MenuField: ColumnLayout {
        property string label:       ""
        property string placeholder: ""
        property string errorMsg:    ""
        property int    maxLength:   32767
        property alias  value:       tf.text
        property alias  validatorObj: tf.validator
        property bool   required:    true
        
        property string formatMode:  ""
        property string maskPattern: ""

        readonly property bool isValid: {
            if (formatMode !== "" && maskPattern !== "") {
                var re = new RegExp(maskPattern)
                return required ? re.test(tf.text)
                                : (tf.text.length === 0 || re.test(tf.text))
            }
            return required ? tf.acceptableInput
                            : (tf.text.length === 0 || tf.acceptableInput)
        }

        spacing: 2
        Layout.fillWidth: true

        property bool _busy:    false
        property int  _prevLen: 0
        property bool touched:  false

        function _applyFormat(raw) {
            var d = raw.replace(/\D/g, "")
            if (formatMode === "time") {
                d = d.substring(0, 4)
                return d.length > 2 ? d.substring(0, 2) + ":" + d.substring(2) : d
            }
            if (formatMode === "date") {
                d = d.substring(0, 8)
                if      (d.length > 6) return d.substring(0,4) + "-" + d.substring(4,6) + "-" + d.substring(6)
                else if (d.length > 4) return d.substring(0,4) + "-" + d.substring(4)
                else                   return d
            }
            return raw
        }

        function clear() {
            tf.text  = ""
            _prevLen = 0
            touched  = false
        }

        Text {
            text: label
            color: clr.textMuted
            font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
        }

        TextField {
            id: tf
            Layout.fillWidth: true
            height: 32
            placeholderText: placeholder
            placeholderTextColor: clr.textPlaceholder
            color: clr.textWhite
            font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
            leftPadding: 8
            // formatMode ya no colisiona con ninguna propiedad de TextField
            maximumLength: formatMode === "time" ? 5
                         : formatMode === "date" ? 10
                         : maxLength

            onActiveFocusChanged: {
                if (!activeFocus && tf.text.length > 0)
                    touched = true
            }

            onTextEdited: {
                if (formatMode === "" || _busy) return
                _busy = true

                var deleting = tf.text.length < _prevLen

                if (!deleting) {
                    var formatted = _applyFormat(tf.text)
                    if (tf.text !== formatted) {
                        tf.text = formatted
                        tf.cursorPosition = formatted.length
                    }
                } else {
                    // Borrado: eliminar guion colgante para no quedar atascado
                    var t = tf.text
                    if (t.length > 0 && t[t.length - 1] === ":") {
                        t = t.slice(0, -1)
                        tf.text = t
                        tf.cursorPosition = t.length
                    }
                }

                _prevLen = tf.text.length
                _busy    = false
            }

            background: Rectangle {
                color: clr.bgField
                radius: 4
                border.width: 1
                border.color: {
                    if (tf.activeFocus)          return clr.borderFocus
                    if (touched && !isValid)     return clr.borderError
                    if (touched &&  isValid)     return clr.borderValid
                    return clr.borderDefault
                }
            }
        }

        Text {
            visible: touched && !isValid
            text: errorMsg
            color: clr.textError
            font.pixelSize: 10
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }

    component MenuButton: Rectangle {
        property string label:     ""
        property bool   secondary: false
        signal action()

        height: 32
        radius: 4
        color: !enabled
               ? clr.btnDisabled
               : secondary
                 ? (btnArea.containsMouse ? clr.btnSecondaryHover  : clr.btnSecondaryNormal)
                 : (btnArea.containsMouse ? clr.btnActionHover     : clr.btnActionNormal)
        border.color: clr.btnActionBorder
        border.width: secondary ? 1 : 0
        opacity: enabled ? 1.0 : 0.4

        Text {
            anchors.centerIn: parent
            text: label
            color: clr.textWhite
            font { pixelSize: 14; family: "Rockwell"; weight: Font.Normal }
            font.bold: !secondary
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
