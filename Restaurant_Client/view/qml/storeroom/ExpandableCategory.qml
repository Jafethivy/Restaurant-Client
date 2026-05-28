import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    id: root

    // === DIMENSIONES FIJAS ===
    width: 300
    height: 440

    // === PROPIEDADES ===
    property string categoryName: "Categoria"
    property var itemsModel: []

    signal stockUpdated(int id_item, real new_stock, string name, string unit)

    // === PALETA MANUAL ===
    readonly property color cDarkSlate:       "#383F51"
    readonly property color cLavender:        "#DDDBF1"
    readonly property color cSteelBlue:       "#3E527A"
    readonly property color cWarmTaupe:       "#C3AB98"
    readonly property color cDarkSlateLight:  "#555b6b"
    readonly property color cDarkSlateDark:   "#2f3544"
    readonly property color cLavenderDark:    "#c2c0d4"
    readonly property color cLavenderLight:   "#eae9f6"
    readonly property color cSteelBlueLight:  "#607191"
    readonly property color cSteelBlueDark:   "#314161"
    readonly property color cWarmTaupeLight:  "#cfbbac"
    readonly property color cWarmTaupeDark:   "#928072"
    readonly property color cCardBg:          "#F8F7FC"

    // === INTERNAS ===
    property bool isExpanded: false
    property int headerHeight: 55
    property int itemRowHeight: 50

    radius: 8
    color: isExpanded ? cLavenderLight : cCardBg
    border {
        width: 2
        color: cWarmTaupe
    }

    // === FUNCIONES ===
    function setItems(items) {
        itemsModel = items
    }

    function getInitialStock(id_item) {
        for (var i = 0; i < itemsModel.length; i++) {
            if (itemsModel[i].id_item === id_item) {
                return itemsModel[i].current_stock || 0
            }
        }
        return 0
    }

    Column {
        anchors.fill: parent
        spacing: 0

        // === HEADER ===
        Rectangle {
            width: parent.width
            height: headerHeight
            color: mouseAreaHeader.containsPress ? cSteelBlueLight
                   : (isExpanded ? cSteelBlue : cCardBg)
            radius: 8

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 18
                anchors.rightMargin: 18

                Text {
                    text: categoryName
                    font {
                        family: "Segoe UI"
                        pixelSize: 16
                        bold: true
                    }
                    color: isExpanded ? cLavender : cDarkSlate
                    Layout.fillWidth: true
                }

                Text {
                    text: isExpanded ? "?" : "?"
                    font {
                        family: "Segoe UI"
                        pixelSize: 16
                    }
                    color: isExpanded ? cLavender : cSteelBlue
                }
            }

            MouseArea {
                id: mouseAreaHeader
                anchors.fill: parent
                onClicked: {
                    isExpanded = !isExpanded
                }
            }
        }

        // === AREA DE ITEMS ===
        Flickable {
            id: itemsFlickable
            width: parent.width
            height: isExpanded ? parent.height - headerHeight : 0
            contentHeight: itemsColumn.height
            clip: true
            visible: height > 0

            Behavior on height {
                NumberAnimation { duration: 220; easing.type: Easing.InOutQuad }
            }

            Column {
                id: itemsColumn
                width: parent.width

                Repeater {
                    model: root.itemsModel || []

                    Rectangle {
                        width: itemsColumn.width
                        height: itemRowHeight
                        color: index % 2 === 0 ? cCardBg : cLavenderLight

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            anchors.rightMargin: 14
                            spacing: 8

                            // Nombre
                            Text {
                                text: modelData.name || ""
                                font {
                                    family: "Segoe UI"
                                    pixelSize: 13
                                }
                                color: (modelData.status !== undefined && modelData.status === 0)
                                       ? cWarmTaupeDark : cDarkSlate
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }

                            // Unidad (opcional, puede no venir en datos minimal)
                            Text {
                                text: modelData.unit || ""
                                font {
                                    family: "Segoe UI"
                                    pixelSize: 12
                                    italic: true
                                }
                                color: cSteelBlueLight
                                Layout.preferredWidth: 32
                                horizontalAlignment: Text.AlignRight
                                visible: text !== ""
                            }

                            // === INPUT DE STOCK ===
                            Rectangle {
                                Layout.preferredWidth: 80
                                Layout.preferredHeight: 32
                                radius: 6
                                color: fieldStock.activeFocus ? "#FFFFFF" : cLavenderLight
                                border {
                                    width: fieldStock.activeFocus ? 2 : 1
                                    color: fieldStock.activeFocus ? cSteelBlue : cWarmTaupe
                                }

                                TextField {
                                    id: fieldStock
                                    anchors.fill: parent
                                    anchors.margins: 2
                                    text: modelData.current_stock !== undefined
                                          ? String(modelData.current_stock) : "0"
                                    font {
                                        family: "Segoe UI"
                                        pixelSize: 14
                                        bold: true
                                    }
                                    color: cDarkSlate
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter

                                    validator: DoubleValidator {
                                        bottom: 0
                                        top: 999999
                                        decimals: 2
                                        notation: DoubleValidator.StandardNotation
                                    }

                                    selectByMouse: true

                                    onEditingFinished: {
                                        var newValue = parseFloat(text) || 0
                                        stockUpdated(
                                            modelData.id_item || 0,
                                            newValue,
                                            modelData.name || "",
                                            modelData.unit || ""
                                        )
                                    }

                                    Keys.onReturnPressed: focus = false
                                    Keys.onEnterPressed: focus = false
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}