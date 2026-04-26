#include "MainWindow.h"

#include "src/reception/Reception.h"
#include "src/Login/Login.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent){
	ui.setupUi(this);


	loginWindow = new Login(this);
	loginWindow->setMainWindow(this);

	ui.stackedWidget->addWidget(loginWindow);

	set_login();
	setupAreas();
}

MainWindow::~MainWindow() {}

Login* MainWindow::loginWidget() const {
	return loginWindow;
}
Reception* MainWindow::receptionWidget() const {
	return receptionWindow;
}

void MainWindow::screen_area() {
	QScreen* screen = QGuiApplication::primaryScreen();
	QRect screenGeometry = screen->availableGeometry();
	width_screen = (screenGeometry.width() - this->width()) / 2;
	height_screen = (screenGeometry.height() - this->height()) / 2;
	move(screenGeometry.x() + width_screen, screenGeometry.y() + height_screen);
}

//Login and logout
void MainWindow::set_login() {
	showNormal();
	resize(421, 481);
	screen_area();
	ui.stackedWidget->setCurrentIndex(0);
}

void MainWindow::logout() {
	if (m_initialized[1] && receptionWindow) {
		set_login();
		ui.stackedWidget->removeWidget(receptionWindow);
		delete receptionWindow;
		receptionWindow = nullptr;
		m_initialized[1] = false;
	}
	// Volver a pagina 0 o login
	// ui.stackedWidget->setCurrentIndex(0);
}

void MainWindow::set_area(const int& areaUserID) {

	if (ensureAreaExists(areaUserID)) {
		switchToArea(areaUserID);
	}

	QTimer::singleShot(10, this, [this]() {
		showMaximized();
		});
	QTimer::singleShot(10, this, [this]() {
		emit create_qml();
		});
}

void MainWindow::closeEvent(QCloseEvent* event) {
	if (m_closingPending) {
		event->accept();
		return;
	}

	m_closingPending = true;
	m_pendingCloseEvent = event;

	emit closingRequested();

	event->ignore();
}

void MainWindow::onCloseApproved() {
	m_closingPending = false;
	close();
}

//QHash Functionality
void MainWindow::switchToArea(int areaId) {
	// Cambiar al widget especifico (necesitas este switch porque las variables son tipos distintos)
	switch (areaId) {
	case 1:
		ui.stackedWidget->setCurrentWidget(receptionWindow);
		break;
		// case 2: ui.stackedWidget->setCurrentWidget(kitchenWindow); break;
	}
}

void MainWindow::setupAreas() {
	m_areas[1] = {
		"Recepcion", [this]() {
			receptionWindow = new Reception(this);
			emit exist(1);
			ui.stackedWidget->addWidget(receptionWindow);
		}
	};

	// Inicializar estado en false
	m_initialized[1] = false;
}

bool MainWindow::ensureAreaExists(int areaId) {
	// Verificar si el ID existe en nuestro "diccionario"
	if (!m_areas.contains(areaId)) {
		qDebug() << "Error: Area" << areaId << "no esta registrada";
		return false;
	}

	// Ya existe? (Verificar la variable directamente o el flag)
	if (m_initialized[areaId]) {
		qDebug() << "Area" << m_areas[areaId].name << "ya inicializada";
		return true;
	}

	// Crear por primera vez - ejecuta la lambda que asigna a tu variable miembro
	qDebug() << "Creando Area:" << m_areas[areaId].name;
	m_areas[areaId].create();
	m_initialized[areaId] = true;

	return true;
}
