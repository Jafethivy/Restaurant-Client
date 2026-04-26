#include "Reception.h"


Reception::Reception(QWidget *parent)
	: QWidget(parent){
	ui.setupUi(this);

}

Reception::~Reception(){
	m_reservations->deleteLater();
	m_tables->deleteLater();
	delete m_Reception;
	delete m_config;
	delete m_advanced;
	m_overlay->deleteLater();
	m_overlayConfig->deleteLater();
	m_overlayAdvanced->deleteLater();
}

//Events
bool Reception::eventFilter(QObject* watched, QEvent* event) {
	if (event->type() != QEvent::MouseButtonPress) {
		return QWidget::eventFilter(watched, event);
	}

	QWidget* targetMenu = nullptr;
	bool isVisible = false;

	if (watched == m_overlay && m_recepVisible) {
		targetMenu = m_Reception;
		isVisible = true;
	}
	else if (watched == m_overlayConfig && m_configVisible) {
		targetMenu = m_config;
		isVisible = true;
	}
	else if (watched == m_overlayAdvanced && m_advancedVisible) {
		targetMenu = m_advanced;
		isVisible = true;
	}

	if (!isVisible) {
		return QWidget::eventFilter(watched, event);
	}

	QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
	QRect menuRect = targetMenu->geometry();
	QPoint globalClick = mouseEvent->globalPosition().toPoint();

	if (!menuRect.contains(globalClick)) {
		if (targetMenu == m_Reception) {
			closeReception();
		}
		else if (targetMenu == m_config) {
			closeConfig();
		}
		else if (targetMenu == m_advanced) {
			closeAdvanced();
		}
		return true;
	}

	return QWidget::eventFilter(watched, event);
}

void Reception::create_qml() {
	if (qml_exist) return;
	QTimer::singleShot(5, this, [this]() {
		create_qml_reservations();
	});
	QTimer::singleShot(20, this, [this]() {
		create_qml_tables();
	});
	QTimer::singleShot(0, this, [this]() {
		create_qml_Reception();
		create_qml_config();
		createQmlAdvanced();
	});
	QTimer::singleShot(50, this, [this]() {
		emit signalReservationInit();
	});
	qml_exist = true;
}

void Reception::closeMenuQml(QWidget* overlay, QQuickWidget* widget, bool& flag) {
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

//Slots
void Reception::on_end_session_clicked() {
	emit endSession();
}

void Reception::on_reception_button_clicked() {
	if (!m_recepVisible) {
		showMenu();
	}
	else {
		closeReception();
	}
}

void Reception::on_config_button_clicked() {
	if (!m_configVisible) {
		showConfig();
	}
	else {
		closeConfig();
	}
}

void Reception::on_help_button_clicked() {

}

void Reception::on_advanced_button_clicked() {
	if (!m_advancedVisible) {
		showAdvanced();
	}
	else {
		closeAdvanced();
	}
}

// Widget Helpers
void Reception::resizeEvent(QResizeEvent* event) {
	w_width = static_cast<int>(width() * 0.3);
	ui.reception_island->setFixedWidth(w_width);
	QWidget::resizeEvent(event);
}

// tables
void Reception::create_qml_tables() {
	m_tables = new QQuickWidget(this);
	m_tables->setResizeMode(QQuickWidget::SizeRootObjectToView);
	m_tables->setAttribute(Qt::WA_TranslucentBackground);
	m_tables->setClearColor(QColor("#dddbf1"));
	m_tables->setSource(QUrl("qrc:/qt/qml/Restaurant_Client/view/qml/reception/tables.qml"));

	QLayout* layout = ui.tables_widget->layout();
	if (!layout) {
		layout = new QVBoxLayout(ui.tables_widget);
		layout->setContentsMargins(0, 0, 0, 0);
	}
	layout->addWidget(m_tables);
}

//reservations
void Reception::create_qml_reservations() {
	m_reservations = new QQuickWidget(this);
	m_reservations->setResizeMode(QQuickWidget::SizeRootObjectToView);
	m_reservations->setClearColor(QColor("#C3AB98"));

	m_reservations->rootContext()->setContextProperty("r_widget", this);

	connect(m_reservations, &QQuickWidget::statusChanged,
		this, [this](QQuickWidget::Status status) {
			if (status == QQuickWidget::Ready)
				resv_create_connections();
		});

	m_reservations->setSource(QUrl("qrc:/qt/qml/Restaurant_Client/view/qml/reception/reservations.qml"));

	m_reservations->setMinimumSize(100, 100);
	m_reservations->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QLayout* layout = ui.reception_widget->layout();
	if (!layout) {
		layout = new QVBoxLayout(ui.reception_widget);
		layout->setContentsMargins(0, 0, 0, 0);
	}

	layout->addWidget(m_reservations);

	ui.reception_widget->update();
	ui.reception_widget->repaint();
}

void Reception::resv_create_connections() {
	QObject* o_reservation = m_reservations->rootObject();
	if (!o_reservation) return;
	connect(o_reservation, SIGNAL(reservationRemoved(QVariant)), this, SLOT(onReservationRemoved(QVariant)));
}

void Reception::onReservationRemoved(QVariant index) {
	emit signalReservationRemoved(index);
}

void Reception::reservationCreatedQml(QVariantMap n_data) {
	QObject* root = m_reservations->rootObject();
	QMetaObject::invokeMethod(root, "addReservation",
		Q_ARG(QVariant, QVariant::fromValue(n_data)));
}

void Reception::reservationEditedQml(QVariantMap n_data) {
	QObject* root = m_reservations->rootObject();
	QMetaObject::invokeMethod(root, "update_resvReservations",
		Q_ARG(QVariant, QVariant::fromValue(n_data)));
}

void Reception::init_reservations(QVariantList reservations) {
	QObject* root = m_reservations->rootObject();
	QMetaObject::invokeMethod(root, "loadReservations",
		Q_ARG(QVariant, QVariant::fromValue(reservations)));
}

void Reception::reservationAdvanced(QVariantList reservations) {
	QObject* root = m_reservations->rootObject();
	QMetaObject::invokeMethod(root, "loadAdvanced",
		Q_ARG(QVariant, QVariant::fromValue(reservations)));
}

//advanced search
void Reception::createQmlAdvanced() {
	m_overlayAdvanced  = new QWidget(parentWidget());
	m_overlayAdvanced->setStyleSheet("background-color: transparent;");
	m_overlayAdvanced->hide();

	m_advanced = new QQuickWidget(nullptr);
	m_advanced->setResizeMode(QQuickWidget::SizeRootObjectToView);

	//Properties
	m_advanced->rootContext()->setContextProperty("Reception", this);

	connect(m_advanced, &QQuickWidget::statusChanged,
		this, [this](QQuickWidget::Status status) {
			if (status == QQuickWidget::Ready)
				a_create_connections();
		});

	m_advanced->setSource(QUrl("qrc:/qt/qml/Restaurant_Client/view/qml/reception/filterPanel.qml"));
	m_advanced->setFixedSize(w_width + 40, 650);

	m_advanced->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

	m_advanced->setAttribute(Qt::WA_TranslucentBackground);
	m_advanced->setAttribute(Qt::WA_NoSystemBackground);
	m_advanced->setClearColor(Qt::transparent);

	m_overlayAdvanced->installEventFilter(this);
}

void Reception::a_create_connections() {
	QObject* advanced = m_advanced->rootObject();
	connect(advanced, SIGNAL(closeRequested()),
		this, SLOT(closeAdvanced()));
	connect(advanced, SIGNAL(applied(QVariant)),
		this, SLOT(onAdvancedQuery(QVariant)));
}

void Reception::onAdvancedQuery(QVariant r_data) {
	QVariantMap data = r_data.toMap();
	emit signalAdvancedQuery(data);
}

void Reception::showAdvanced() {
	QWidget* mainWindow = window();
	if (!mainWindow) return;
	m_overlayAdvanced->setGeometry(mainWindow->rect());
	m_overlayAdvanced->show();
	m_overlayAdvanced->raise();
	m_overlayAdvanced->adjustSize();

	int x1 = ui.reception_island->mapToGlobal(
		QPoint(0, ui.reception_island->height())
	).x();
	int y1 = ui.reception_island->mapToGlobal(
		QPoint(ui.reception_island->width(), ui.reception_island->height())
	).y();
	int x2 = 220;
	int y2 = 650;

	QPoint finalPos(x1, y1 - y2);
	QPoint startPos = finalPos + QPoint(0, 20);

	m_advanced->move(startPos);
	m_advanced->show();
	m_advanced->raise();
	m_advanced->setWindowOpacity(0);

	auto* group = new QParallelAnimationGroup(this);

	auto* slide = new QPropertyAnimation(m_advanced, "pos");
	slide->setDuration(600);
	slide->setStartValue(startPos);
	slide->setEndValue(finalPos);
	slide->setEasingCurve(QEasingCurve::OutCubic);

	auto* fade = new QPropertyAnimation(m_advanced, "windowOpacity");
	fade->setDuration(400);
	fade->setStartValue(0.0);
	fade->setEndValue(1.0);
	fade->setEasingCurve(QEasingCurve::OutCubic);

	group->addAnimation(slide);
	group->addAnimation(fade);
	group->start(QAbstractAnimation::DeleteWhenStopped);

	m_advancedVisible = true;
}

void Reception::closeAdvanced() {
	closeMenuQml(m_overlayAdvanced, m_advanced, m_advancedVisible);
}

//Reception
void Reception::create_qml_Reception() {
	//Build
	m_overlay = new QWidget(parentWidget());
	m_overlay->setStyleSheet("background-color: transparent;");
	m_overlay->hide();

	m_Reception = new QQuickWidget(nullptr);
	m_Reception->setResizeMode(QQuickWidget::SizeRootObjectToView);

	//Properties
	m_Reception->rootContext()->setContextProperty("r_widget", this);

	connect(m_Reception, &QQuickWidget::statusChanged,
		this, [this](QQuickWidget::Status status) {
			if (status == QQuickWidget::Ready)
				r_create_connections();
		});

	m_Reception->setSource(QUrl("qrc:/qt/qml/Restaurant_Client/view/qml/reception/Reception.qml"));
	m_Reception->setFixedSize(300, 440);

	m_Reception->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

	m_Reception->setAttribute(Qt::WA_TranslucentBackground);
	m_Reception->setAttribute(Qt::WA_NoSystemBackground);
	m_Reception->setClearColor(Qt::transparent);

	m_overlay->installEventFilter(this);
}

void Reception::r_create_connections() {
	QObject* o_Reception = m_Reception->rootObject();
	if (!o_Reception) return;
	connect(o_Reception, SIGNAL(reservationCreated(QVariant)),
		this, SLOT(onReservationCreated(QVariant)));
	connect(o_Reception, SIGNAL(reservationEdited(QVariant)),
		this, SLOT(onReservationEdited(QVariant)));
	connect(o_Reception, SIGNAL(closeRequested()),
		this, SLOT(closeReception()));
}

void Reception::onReservationCreated(QVariant data) {
	QVariantMap m_data = data.toMap();
	emit signalReservationCreated(m_data);
}

void Reception::onReservationEdited(QVariant data) {
	QVariantMap m_data = data.toMap();
	emit signalReservationEdited(m_data);
}

void Reception::showMenu() {
	QWidget* mainWindow = window();
	if (!mainWindow) return;

	m_overlay->setGeometry(mainWindow->rect());
	m_overlay->show();
	m_overlay->raise();
	m_Reception->adjustSize();

	int x1 = ui.reception_button->mapToGlobal(
		QPoint(ui.reception_button->width(), ui.reception_button->height())
	).x();
	int y1 = ui.reception_button->mapToGlobal(
		QPoint(ui.reception_button->width(), ui.reception_button->height())
	).y();
	
	int x2 = 260;//m_Reception width - spacer x
	int y2 = 420;//m_Reception height - spacer y

	QPoint finalPos(x1-x2, y1 - y2);
	QPoint startPos = finalPos + QPoint(0, 20);

	m_Reception->move(startPos);
	m_Reception->show();
	m_Reception->raise();

	m_Reception->setWindowOpacity(0);
	auto* group = new QParallelAnimationGroup(this);

	auto* slide = new QPropertyAnimation(m_Reception, "pos");
	slide->setDuration(600);
	slide->setStartValue(startPos);
	slide->setEndValue(finalPos);
	slide->setEasingCurve(QEasingCurve::OutCubic);

	auto* fade = new QPropertyAnimation(m_Reception, "windowOpacity");
	fade->setDuration(400);
	fade->setStartValue(0.0);
	fade->setEndValue(1.0);
	fade->setEasingCurve(QEasingCurve::OutCubic);

	group->addAnimation(slide);
	group->addAnimation(fade);
	group->start(QAbstractAnimation::DeleteWhenStopped);

	m_recepVisible = true;
}

void Reception::closeReception() {
	closeMenuQml(m_overlay, m_Reception, m_recepVisible);
}

//config
void Reception::create_qml_config() {
	m_overlayConfig = new QWidget(parentWidget());
	m_overlayConfig->setStyleSheet("background-color: transparent;");
	m_overlayConfig->hide();

	m_config = new QQuickWidget(nullptr);
	m_config->setResizeMode(QQuickWidget::SizeRootObjectToView);
	m_config->setSource(QUrl("qrc:/qt/qml/Restaurant_Client/view/qml/reception/config.qml"));
	m_config->setFixedSize(220, 160);

	m_config->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

	m_config->setAttribute(Qt::WA_TranslucentBackground);
	m_config->setAttribute(Qt::WA_NoSystemBackground);
	m_config->setClearColor(Qt::transparent);

	m_overlayConfig->installEventFilter(this);
}

void Reception::showConfig() {
	QWidget* mainWindow = window();
	if (!mainWindow) return;

	m_overlayConfig->setGeometry(mainWindow->rect());
	m_overlayConfig->show();
	m_overlayConfig->raise();

	m_config->adjustSize();
	int menuHeight = m_config->height();

	QPoint globalPos = ui.config_button->mapToGlobal(
		QPoint(1, -menuHeight)
	);
	m_config->move(globalPos);
	m_config->show();
	m_config->raise();

	m_config->setWindowOpacity(0);
	auto* anim = new QPropertyAnimation(m_config, "windowOpacity");
	anim->setDuration(150);
	anim->setStartValue(0.0);
	anim->setEndValue(1.0);
	anim->start(QAbstractAnimation::DeleteWhenStopped);

	m_configVisible = true;
}

void Reception::closeConfig() {
	closeMenuQml(m_overlayConfig, m_config, m_configVisible);
}