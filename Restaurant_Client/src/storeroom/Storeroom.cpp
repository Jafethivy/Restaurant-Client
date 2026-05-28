#include "Storeroom.h"

Storeroom::Storeroom(QWidget *parent)
	: QWidget(parent){
	ui.setupUi(this);

    connect(ui.stockTab, &QTabWidget::currentChanged,
        this, &Storeroom::onTabChanged);
}

Storeroom::~Storeroom()
{
    m_update->deleteLater();
    delete m_overlayUpdated;
}

void Storeroom::setUsername(QString username) {
	 ui.label_2->setText(username);
}

void Storeroom::initArea() {
    if (qml_exist) return;
	QTimer::singleShot(20, this, [this]() {
        emit createQml();
	});
	QTimer::singleShot(20, this, [this]() {
		emit getCategories();
	});
    QTimer::singleShot(20, this, [this]() {
        emit getExpandable();
    });
    qml_exist = true;
}

void Storeroom::categoriesReceived(const QVariantList& categories)
{
    ui.stockTab->clear();

    QList<QVariantMap> maps;
    for (const QVariant& var : categories) {
        maps.append(var.toMap());
    }

    std::sort(maps.begin(), maps.end(), [](const QVariantMap& a, const QVariantMap& b) {
        return a.value("id_category").toInt() < b.value("id_category").toInt();
        });

    for (const QVariantMap& map : maps) {
        int     id = map.value("id_category").toInt();
        QString name = map.value("name").toString();

        QWidget* tabContent = new QWidget(ui.stockTab);
        tabContent->setProperty("id_category", id);

        QVBoxLayout* layout = new QVBoxLayout(tabContent);
        layout->setContentsMargins(6, 6, 6, 6);

        QTableWidget* table = new QTableWidget(tabContent);
        table->setColumnCount(7);
        table->setHorizontalHeaderLabels({ "ID", "Producto", "Stock", "Unidad", "Estado", "Creado", "Actualizado" });
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setAlternatingRowColors(true);
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setHighlightSections(false);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        table->horizontalHeader()->setStretchLastSection(true);

        // === CORRECCIÓN: habilitar doble clic, pero controlar por celda ===
        table->setEditTriggers(QAbstractItemView::DoubleClicked);

        table->setStyleSheet(R"(
            QTableWidget {
                background-color: #F8F7FC;
                alternate-background-color: #eae9f6;
                color: #383F51;
                font-size: 18px;
                border: 2px solid #C3AB98;
                border-radius: 6px;
                gridline-color: #C3AB98;
                selection-background-color: #3E527A;
                selection-color: #FFFFFF;
            }
            QTableWidget::item {
                padding: 10px 24px;
                border-bottom: 1px solid #DDDBF1;
            }
            QTableWidget::item:selected {
                background-color: #3E527A;
                color: #FFFFFF;
            }
            QTableWidget::item:focus {
                background-color: #607191;
                color: #FFFFFF;
            }
            QHeaderView::section {
                background-color: #383F51;
                color: #DDDBF1;
                font-size: 18px;
                font-weight: bold;
                padding: 12px 24px;
                border: none;
                border-right: 1px solid #555b6b;
            }
            QHeaderView::section:last {
                border-right: none;
            }
            QHeaderView::section:pressed {
                background-color: #2f3544;
            }
            QTableCornerButton::section {
                background-color: #383F51;
                border: none;
            }
            QScrollBar:vertical {
                background: #eae9f6;
                width: 16px;
                border-radius: 8px;
            }
            QScrollBar::handle:vertical {
                background: #C3AB98;
                border-radius: 8px;
                min-height: 30px;
            }
            QScrollBar::handle:vertical:hover {
                background: #928072;
            }
        )");

        tabContent->setProperty("table", QVariant::fromValue(table));
        layout->addWidget(table);
        ui.stockTab->addTab(tabContent, name);

        connect(table, &QTableWidget::cellChanged, this, &Storeroom::onStockCellChanged);
    }

    connect(ui.stockTab, &QTabWidget::currentChanged,
        this, &Storeroom::onTabChanged);

    if (ui.stockTab->count() > 0) {
        onTabChanged(0);
    }
}

void Storeroom::onTabChanged(int index)
{
    if (index < 0) return;

    QWidget* tab = ui.stockTab->widget(index);
    int categoryId = tab->property("id_category").toInt();

    m_pendingCategoryId = categoryId;   // Guardamos para la respuesta
    qDebug() << "Pidiendo items para categoria ID:" << categoryId;

    // === LLAMA A TU SERVIDOR/RED AQUÍ ===
    emit getFirstArea(categoryId);
    // o: networkManager->getItemsByCategory(categoryId);
}

// === SLOT: Cuando el usuario edita una celda de Stock ===
void Storeroom::onStockCellChanged(int row, int column)
{
    if (column != 2) return;

    QTableWidget* table = qobject_cast<QTableWidget*>(sender());
    if (!table) return;

    QTableWidgetItem* idItem = table->item(row, 0);
    QTableWidgetItem* stockItem = table->item(row, 2);
    if (!idItem || !stockItem) return;

    int id = idItem->text().toInt();
    QString stockText = stockItem->text();

    bool ok;
    double newStock = stockText.toDouble(&ok);
    if (!ok || newStock < 0) {
        qWarning() << "Valor invalido en stock, ignorando:" << stockText;
        return;
    }

    // Guardar como QString(clave) -> QVariant(double)
    m_pendingStockUpdates[QString::number(id)] = newStock;

    qDebug() << "Stock modificado — ID:" << id << "?" << newStock;
    qDebug() << "Pendientes:" << m_pendingStockUpdates;
}

// === PREPARA EL MAPA LIMPIO PARA EL SERVIDOR ===
QVariantMap Storeroom::commitStockUpdates()
{
    QVariantMap payload;

    for (auto it = m_pendingStockUpdates.begin(); it != m_pendingStockUpdates.end(); ++it) {
        QString idStr = it.key();           // ej: "5"
        QVariant stock = it.value();         // ej: 15.5

        payload[idStr] = stock;              // { "5": 15.5, "2": 18.0 }
    }

    return payload;  // QVariantMap plano listo para emitir
}

// === BOTÓN: Emite y limpia ===
void Storeroom::on_updateButton_clicked()
{
    if (m_pendingStockUpdates.isEmpty()) {
        qDebug() << "No hay cambios de stock pendientes";
        return;
    }

    QVariantMap updates = commitStockUpdates();

    qDebug() << "Emitiendo al servidor:" << updates;

    // === EMITIR AQUÍ ===
    emit requestStockUpdate(updates);

    // Limpiar después de enviar
    m_pendingStockUpdates.clear();
}

void Storeroom::categoryItemsReceived(const QVariantList& items)
{
    qDebug() << ">>> Recibidos" << items.count() << "items";
    if (items.isEmpty()) return;

    QTableWidget* targetTable = nullptr;
    for (int i = 0; i < ui.stockTab->count(); ++i) {
        QWidget* tab = ui.stockTab->widget(i);
        if (tab->property("id_category").toInt() == m_pendingCategoryId) {
            targetTable = tab->property("table").value<QTableWidget*>();
            break;
        }
    }

    if (!targetTable) {
        qWarning() << ">>> ERROR: No se encontro tabla para categoryId:" << m_pendingCategoryId;
        return;
    }

    targetTable->clearContents();
    targetTable->setRowCount(0);

    QList<QVariantMap> maps;
    for (const QVariant& var : items) maps.append(var.toMap());
    std::sort(maps.begin(), maps.end(), [](const QVariantMap& a, const QVariantMap& b) {
        return a.value("id_item").toLongLong() < b.value("id_item").toLongLong();
        });

    for (const QVariantMap& map : maps) {
        if (map.isEmpty()) continue;

        qlonglong idItemLL = map.value("id_item").toLongLong();
        QString   name = map.value("name").toString();
        QString   unit = map.value("unit").toString();
        qlonglong statusLL = map.value("status").toLongLong();
        QString   created = map.value("created_at").toString();
        QString   updated = map.value("updated_at").toString();

        QString stock;
        QVariant stockVar = map.value("current_stock");
        if (stockVar.userType() == QMetaType::Double || stockVar.canConvert<double>()) {
            stock = QString::number(stockVar.toDouble(), 'f', 2);
        }
        else {
            stock = QString::number(stockVar.toLongLong());
        }

        QString statusText;
        switch (static_cast<int>(statusLL)) {
        case 0:  statusText = "Inactivo"; break;
        case 1:  statusText = "Activo"; break;
        case 2:  statusText = "Bajo stock"; break;
        default: statusText = "Desconocido"; break;
        }

        int newRow = targetTable->rowCount();
        targetTable->insertRow(newRow);

        // === COL 0: ID — NO editable ===
        QTableWidgetItem* item0 = new QTableWidgetItem(QString::number(idItemLL));
        item0->setTextAlignment(Qt::AlignCenter);
        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
        targetTable->setItem(newRow, 0, item0);

        // === COL 1: Nombre — NO editable ===
        QTableWidgetItem* item1 = new QTableWidgetItem(name);
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
        targetTable->setItem(newRow, 1, item1);

        // === COL 2: Stock — SÍ editable (dejamos el flag por defecto) ===
        QTableWidgetItem* item2 = new QTableWidgetItem(stock);
        item2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        // ItemIsEditable está activo por defecto, no lo tocamos
        targetTable->setItem(newRow, 2, item2);

        // === COL 3: Unidad — NO editable ===
        QTableWidgetItem* item3 = new QTableWidgetItem(unit);
        item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
        targetTable->setItem(newRow, 3, item3);

        // === COL 4: Estado — NO editable ===
        QTableWidgetItem* item4 = new QTableWidgetItem(statusText);
        item4->setFlags(item4->flags() & ~Qt::ItemIsEditable);
        targetTable->setItem(newRow, 4, item4);

        // === COL 5: Creado — NO editable ===
        QTableWidgetItem* item5 = new QTableWidgetItem(created);
        item5->setFlags(item5->flags() & ~Qt::ItemIsEditable);
        targetTable->setItem(newRow, 5, item5);

        // === COL 6: Actualizado — NO editable ===
        QTableWidgetItem* item6 = new QTableWidgetItem(updated);
        item6->setFlags(item6->flags() & ~Qt::ItemIsEditable);
        targetTable->setItem(newRow, 6, item6);
    }

    targetTable->resizeColumnsToContents();
    targetTable->viewport()->update();

    qDebug() << ">>> Carga completa. Total filas:" << targetTable->rowCount();
}

void Storeroom::on_endSession_clicked() {
    emit endSession();
}

// NO LAS VOY A USAR
void Storeroom::createQml() {
    m_overlayUpdated = new QWidget(parentWidget());
    m_overlayUpdated->setStyleSheet("background-color: transparent;");
    m_overlayUpdated->hide();

    m_update = new QQuickWidget(nullptr);
    m_update->setResizeMode(QQuickWidget::SizeRootObjectToView);

    //Properties
    m_update->rootContext()->setContextProperty("Reception", this);

    connect(m_update, &QQuickWidget::statusChanged,
        this, [this](QQuickWidget::Status status) {
            //if (status == QQuickWidget::Ready)
                //a_create_connections();
        });

    m_update->setSource(QUrl("qrc:/qt/qml/Restaurant_Client/view/qml/storeroom/ExpandableCategory.qml"));
    m_update->setFixedSize(300, 440);

    m_update->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    m_update->setAttribute(Qt::WA_TranslucentBackground);
    m_update->setAttribute(Qt::WA_NoSystemBackground);
    m_update->setClearColor(Qt::transparent);

    m_overlayUpdated->installEventFilter(this);
}

void Storeroom::showUpdate() {
    QWidget* mainWindow = window();
    if (!mainWindow) return;

    m_overlayUpdated->setGeometry(mainWindow->rect());
    m_overlayUpdated->show();
    m_overlayUpdated->raise();
    m_update->adjustSize();

    int x1 = ui.updateButton->mapToGlobal(
        QPoint(ui.updateButton->width(), ui.updateButton->height())
    ).x();
    int y1 = ui.updateButton->mapToGlobal(
        QPoint(ui.updateButton->width(), ui.updateButton->height())
    ).y();

    int x2 = 260;//m_Reception width - spacer x
    int y2 = 420;//m_Reception height - spacer y

    QPoint finalPos(x1 - x2, y1 - y2);
    QPoint startPos = finalPos + QPoint(0, 20);

    m_update->move(startPos);
    m_update->show();
    m_update->raise();

    m_update->setWindowOpacity(0);
    auto* group = new QParallelAnimationGroup(this);

    auto* slide = new QPropertyAnimation(m_update, "pos");
    slide->setDuration(600);
    slide->setStartValue(startPos);
    slide->setEndValue(finalPos);
    slide->setEasingCurve(QEasingCurve::OutCubic);

    auto* fade = new QPropertyAnimation(m_update, "windowOpacity");
    fade->setDuration(400);
    fade->setStartValue(0.0);
    fade->setEndValue(1.0);
    fade->setEasingCurve(QEasingCurve::OutCubic);

    group->addAnimation(slide);
    group->addAnimation(fade);
    group->start(QAbstractAnimation::DeleteWhenStopped);

    m_updateVisible = true;
}

void Storeroom::closeUpdate() {
    closeMenuQml(m_overlayUpdated, m_update, m_updateVisible);
}

void Storeroom::minimalInventoryReceived(const QVariantList& items) {
    QObject* root = m_update->rootObject();
    QMetaObject::invokeMethod(root, "setItems",
        Q_ARG(QVariant, QVariant::fromValue(items)));
}

void Storeroom::closeMenuQml(QWidget* overlay, QQuickWidget* widget, bool& flag) {
    if (!flag) return;

    auto* anim = new QPropertyAnimation(widget, "windowOpacity");
    anim->setDuration(150);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);

    connect(anim, &QPropertyAnimation::finished, [=, &flag]() {
        widget->hide();
        overlay->hide();
        flag = false;
        });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

bool Storeroom::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() != QEvent::MouseButtonPress) {
        return QWidget::eventFilter(watched, event);
    }

    QWidget* targetMenu = nullptr;
    bool isVisible = false;

    if (watched == m_overlayUpdated && m_updateVisible) {
        targetMenu = m_update;
        isVisible = true;
    }

    if (!isVisible) {
        return QWidget::eventFilter(watched, event);
    }

    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    QRect menuRect = targetMenu->geometry();
    QPoint globalClick = mouseEvent->globalPosition().toPoint();

    if (!menuRect.contains(globalClick)) {
        if (targetMenu == m_update) {
            closeUpdate();
        }
        return true;
    }

    return QWidget::eventFilter(watched, event);
}
