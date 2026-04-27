#include "AppContext.h"
#include <QThread>

#include "src/MainWindow.h"
#include "src/reception/Reception.h"
#include "src/Reception/ReceptionController.h"
#include "src/login/LoginController.h"

#include "services/authService/AuthService.h"
#include "services/receptionService/ReservationService.h"


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
    // Login Controller -> Login Window
    QObject::connect(m_loginController, &LoginController::LoginStatus,
        m_loginWidget, &Login::Status);
    // Login Window -> Main Window
    QObject::connect(m_loginWidget, &Login::LoginSuccess,
        m_window, &MainWindow::set_area);

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
        break;
    case 4:
        break;
    }
}

void AppContext::demandReception() {
    if (!m_window->receptionWidget()) return;

    m_receptionWidget = m_window->receptionWidget();
    m_receptionController = new ReceptionController(m_receptionWidget);

    m_reservationService = new ReservationService();
    m_reservationService->setToken(m_authService->token());

    //Setup Reception connecetions
    
    // Reception Controller -> Auth 
    QObject::connect(m_receptionController, &ReceptionController::updateSession,
        m_authService, &AuthService::logout,
        Qt::AutoConnection);
    // Reception Controller -> Main Window
    QObject::connect(m_receptionController, &ReceptionController::endSession,
        m_window, &MainWindow::logout,
        Qt::AutoConnection);
    // Reception Controller -> AuthService [end session]
    QObject::connect(m_receptionController, &ReceptionController::updateSession,
        m_authService, &AuthService::logout,
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

    //ReservationService -> Reception Controller [get reservations]
    QObject::connect( m_reservationService, &ReservationService::reservationsGetter,
        m_receptionController, &ReceptionController::c_getReservations,
        Qt::AutoConnection);
    //ReservationService -> Reception Controller [created reservation]
    QObject::connect( m_reservationService, &ReservationService::n_ReservationCreated,
        m_receptionController, &ReceptionController::createdReservationQml,
        Qt::AutoConnection);
    //ReservationService -> Reception Controller [edited reservation]
    QObject::connect( m_reservationService, &ReservationService::n_ReservationEdited,
        m_receptionController, &ReceptionController::editedReservationQml,
        Qt::AutoConnection);
    //ReservationService -> Reception Controller [advanced query]
    QObject::connect( m_reservationService, &ReservationService::reservationAdvanced,
        m_receptionController, &ReceptionController::s_reservationsAdvanced,
        Qt::AutoConnection);
}