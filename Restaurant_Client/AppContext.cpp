#include "AppContext.h"
#include <QThread>

#include "src/MainWindow.h"
#include "src/reception/Reception.h"
#include "src/Reception/ReceptionController.h"
#include "src/login/LoginController.h"
#include "src/storeroom/Storeroom.h"
#include "src/storeroom/StoreroomController.h"

#include "services/authService/AuthService.h"
#include "services/receptionService/ReservationService.h"
#include "services/receptionService/TablesService.h"
#include "services/storeroomService/InventoryService.h"

#include "network/WebSocketClient.h"


AppContext::AppContext(QObject* parent) : QObject(parent) {
}

AppContext::~AppContext() {
    cleanup();
}

void AppContext::createObjects() {
	m_window = new MainWindow();
    m_loginWidget = m_window->loginWidget();
	m_loginController = new LoginController(m_loginWidget);

    m_authService = new AuthService();
}

void AppContext::setupThreads() {
}

void AppContext::setupConnections() const {
	//Close Event Connections
    QObject::connect(m_window, &MainWindow::closingRequested,
        m_loginController, &LoginController::on_endSession,
        Qt::AutoConnection);
    /*
    QObject::connect(m_authService, &AuthService::endSession_success,
        m_loginController, &LoginController::on_endSession_success,
        Qt::AutoConnection);
    */
    QObject::connect(m_loginController, &LoginController::endSession_success,
        m_window, &MainWindow::onCloseApproved,
        Qt::AutoConnection);

    //MainWindow -> this (Emit a signal telling to AppContext an area will be used)
    QObject::connect(m_window, &MainWindow::exist,
        this, &AppContext::Demand,
        Qt::AutoConnection);


    //Login Controller -> AuthService
    QObject::connect(m_loginController, &LoginController::LoginAttempt,
        m_authService, &AuthService::attemptLogin,
        Qt::AutoConnection);
    // Auth -> Login Controller
    QObject::connect(m_authService, &AuthService::LoginStatus,
        m_loginController, &LoginController::on_login_status,
        Qt::AutoConnection);
    QObject::connect(m_authService, &AuthService::authUsername,
        m_window, &MainWindow::setUsername,
        Qt::AutoConnection);

    // Login Controller -> Login Window
    QObject::connect(m_loginController, &LoginController::LoginStatus,
        m_loginWidget, &Login::Status);
    // Login Window -> Main Window
    QObject::connect(m_loginWidget, &Login::LoginSuccess,
        m_window, &MainWindow::set_area);

    QObject::connect(this, &AppContext::NoArea,
        m_loginController, &LoginController::NoArea,
        Qt::AutoConnection);
}

void AppContext::cleanup() {
}

void AppContext::debugConnections() {
    qDebug() << "Debugging connections...";

}

MainWindow* AppContext::initialize() {
    createObjects();
    setupThreads();
    setupConnections();
    return m_window;
}

//on demand (this section will build the areas on demand)
void AppContext::Demand(int area) {
    switch (area) {
    case 1:
        demandReception();
        break;
    case 2:
        break;
    case 3:
        demandStoreroom();
        break;
    case 4:
        break;
    default:
        emit NoArea();
        break;
    }
}

void AppContext::demandReception() {
    if (!m_window->receptionWidget()) return;

    m_receptionWidget = m_window->receptionWidget();
    m_receptionController = new ReceptionController(m_receptionWidget);

    m_reservationService = new ReservationService();
    m_reservationService->setToken(m_authService->token());

    m_tablesService = new TablesService();
    m_tablesService->setToken(m_authService->token());

	m_webSocketClient = new WebSocketClient();
	m_webSocketClient->connectToServer(m_wsUrl, m_authService->token());

    wsReceptionConnections();
	connectionsReception();
}
void AppContext::connectionsReception() const {
    // Reception Controller -> Auth 
    QObject::connect(m_receptionController, &ReceptionController::updateSession,
        m_authService, &AuthService::logout,
        Qt::AutoConnection);
    // Reception Controller -> Main Window
    QObject::connect(m_receptionController, &ReceptionController::endSession,
        m_window, &MainWindow::logout,
        Qt::AutoConnection);

    //Main Window -> Reception Controller
    QObject::connect(m_window, &MainWindow::create_qml,
        m_receptionController, &ReceptionController::create_qml,
        Qt::AutoConnection);

    //Reception Controller -> ReservationService [init reservations]
    QObject::connect(m_receptionController, &ReceptionController::c_reservationInit,
        m_reservationService, &ReservationService::initReservations,
        Qt::AutoConnection);
    //Reception Controller -> ReservationService [create reservation]
    QObject::connect(m_receptionController, &ReceptionController::c_reservationCreated,
        m_reservationService, &ReservationService::createReservation,
        Qt::AutoConnection);
    //Reception Controller -> ReservationService [edit reservation]
    QObject::connect(m_receptionController, &ReceptionController::c_reservationEdited,
        m_reservationService, &ReservationService::editReservation,
        Qt::AutoConnection);
    //Reception Controller -> ReservationService [remove reservation]
    QObject::connect(m_receptionController, &ReceptionController::c_reservationRemoved,
        m_reservationService, &ReservationService::removeReservation,
        Qt::AutoConnection);
    //Reception Controller -> ReservationService [advanced query]
    QObject::connect(m_receptionController, &ReceptionController::c_advancedQuery,
        m_reservationService, &ReservationService::advancedQuery,
        Qt::AutoConnection);
    //Reception Controller -> ReservationService [update reservation]
    QObject::connect(m_receptionController, &ReceptionController::signalReservationUpdate,
        m_reservationService, &ReservationService::updateReservation,
        Qt::AutoConnection);

    //ReservationService -> Reception Controller [get reservations]
    QObject::connect(m_reservationService, &ReservationService::reservationsGetter,
        m_receptionController, &ReceptionController::c_getReservations,
        Qt::AutoConnection);
    //ReservationService -> Reception Controller [reservation created]
    QObject::connect(m_reservationService, &ReservationService::n_ReservationCreated,
        m_receptionController, &ReceptionController::createdReservationQml,
        Qt::AutoConnection);
    //ReservationService -> Reception Controller [reservation edited]
    QObject::connect(m_reservationService, &ReservationService::n_ReservationEdited,
        m_receptionController, &ReceptionController::editedReservationQml,
        Qt::AutoConnection);
    //ReservationService -> Reception Controller [advanced query]
    QObject::connect(m_reservationService, &ReservationService::reservationAdvanced,
        m_receptionController, &ReceptionController::s_reservationsAdvanced,
        Qt::AutoConnection);
    //ReservationService -> Reception Controller [reservation updated]
    QObject::connect(m_reservationService, &ReservationService::reservationUpdated,
        m_receptionController, &ReceptionController::reservationUpdated,
        Qt::AutoConnection);

    //ReservationController -> TablesService [init tables]
    QObject::connect(m_receptionController, &ReceptionController::signalTablesInit,
        m_tablesService, &TablesService::getAllTables,
        Qt::AutoConnection);
    //ReservationController -> TablesService [reserve table]
    QObject::connect(m_receptionController, &ReceptionController::reserveTable,
        m_tablesService, &TablesService::reserveTable,
        Qt::AutoConnection);
    //ReservationController -> TablesService [occupy table]
    QObject::connect(m_receptionController, &ReceptionController::occupyTable,
        m_tablesService, &TablesService::occupyTable,
        Qt::AutoConnection);

    //TablesService -> ReservationController [table status]
    QObject::connect(m_tablesService, &TablesService::tableStatus,
        m_receptionController, &ReceptionController::tableStatus,
        Qt::AutoConnection);
    //TablesService -> ReservationController [tables getter]
    QObject::connect(m_tablesService, &TablesService::tablesGetter,
        m_receptionController, &ReceptionController::tablesGetter,
        Qt::AutoConnection);
}
void AppContext::wsReceptionConnections() const {
    QObject::connect(m_webSocketClient, &WebSocketClient::tableStatusChange,
        m_receptionController, &ReceptionController::tableStatus,
		Qt::AutoConnection);
    QObject::connect(m_webSocketClient, &WebSocketClient::reservationOverdue,
        m_receptionController, &ReceptionController::reservationOverdue,
        Qt::AutoConnection);
}


void AppContext::demandStoreroom() {
    if (!m_window->storeroomWidget()) return;

    m_storeroomWidget = m_window->storeroomWidget();
    m_storeroomController = new StoreroomController(m_storeroomWidget);

    m_inventoryService = new InventoryService();
    m_inventoryService->setToken(m_authService->token());

    m_webSocketClient = new WebSocketClient();
    m_webSocketClient->connectToServer(m_wsUrl, m_authService->token());

    wsStoreroomConnections();
    connectionsStoreroom();
}
void AppContext::connectionsStoreroom() const {
    QObject::connect(m_window, &MainWindow::create_qml,
        m_storeroomController, &StoreroomController::initArea,
        Qt::AutoConnection);

    QObject::connect(m_storeroomController, &StoreroomController::endSession,
        m_authService, &AuthService::logout,
        Qt::AutoConnection);
    QObject::connect(m_storeroomController, &StoreroomController::endSession,
        m_window, &MainWindow::logout,
        Qt::AutoConnection);

    QObject::connect(m_storeroomController, &StoreroomController::getCategories,
        m_inventoryService, &InventoryService::getCategories,
        Qt::AutoConnection);
    QObject::connect(m_storeroomController, &StoreroomController::getFirstArea,
        m_inventoryService, &InventoryService::getByCategory,
        Qt::AutoConnection);
    QObject::connect(m_storeroomController, &StoreroomController::getExpandable,
        m_inventoryService, &InventoryService::getMinimalInventory,
        Qt::AutoConnection);
    QObject::connect(m_storeroomController, &StoreroomController::requestStockUpdate,
        m_inventoryService, &InventoryService::updateBatch,
        Qt::AutoConnection);

    QObject::connect(m_inventoryService, &InventoryService::categoriesReceived,
        m_storeroomController, &StoreroomController::categoriesReceived,
        Qt::AutoConnection);
    QObject::connect(m_inventoryService, &InventoryService::categoryItemsReceived,
        m_storeroomController, &StoreroomController::categoryItemsReceived);
    QObject::connect(m_inventoryService, &InventoryService::minimalInventoryReceived,
        m_storeroomController, &StoreroomController::minimalInventoryReceived,
        Qt::AutoConnection);


}
void AppContext::wsStoreroomConnections() const {

}