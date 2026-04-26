#include "AppContext.h"
#include <QThread>

#include "src/MainWindow.h"

#include "src/reception/Reception.h"
#include "src/Reception/ReceptionController.h"
#include "src/login/LoginController.h"


AppContext::AppContext(QObject* parent) : QObject(parent) {
}

AppContext::~AppContext() {
    cleanup();
}

void AppContext::createObjects() {
	m_window = new MainWindow();

    m_loginWidget = m_window->loginWidget();

	m_controller = new LoginController(m_loginWidget);

}

void AppContext::setupThreads() {
}

void AppContext::setupConnections() {
	//Close Event Connections
    QObject::connect(m_window, &MainWindow::closingRequested,
        m_controller, &LoginController::on_endSession,
        Qt::AutoConnection);
    /*
    QObject::connect(m_controller, &LoginController::endSession,
        m_auth, &Auth::updateStatusDb,
        Qt::AutoConnection);

    QObject::connect(m_auth, &Auth::endSession_success,
        m_controller, &LoginController::on_endSession_success,
        Qt::AutoConnection);
    */
    QObject::connect(m_controller, &LoginController::endSession_success,
        m_window, &MainWindow::onCloseApproved,
        Qt::AutoConnection);

    //MainWindow -> this (Emit a signal telling to AppContext an area will be used)
    QObject::connect(m_window, &MainWindow::exist,
        this, &AppContext::Demand,
        Qt::AutoConnection);


    //Login Controller -> Auth
    /*
    QObject::connect(m_controller, &LoginController::LoginAttempt,
        m_auth, &Auth::on_login_attempt,
        Qt::AutoConnection);
    // Auth -> Login Controller
    QObject::connect(m_auth, &Auth::LoginStatus,
        m_controller, &LoginController::on_login_status,
        Qt::AutoConnection);
    */
    // Login Controller -> Login Window
    QObject::connect(m_controller, &LoginController::LoginStatus,
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
    /*
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
    */
}

void AppContext::demandReception() {
    if (!m_window->receptionWidget()) return;

    m_receptionWidget = m_window->receptionWidget();
    m_receptionController = new ReceptionController(m_receptionWidget);

    //Setup Reception connecetions
    
    // Reception Controller -> Auth 
    /*
    QObject::connect(m_ReceptionController, &ReceptionController::updateSession,
        m_auth, &Auth::updateStatusDb,
        Qt::AutoConnection);
    */
    // Reception Controller -> Main Window
    QObject::connect(m_receptionController, &ReceptionController::endSession,
        m_window, &MainWindow::logout,
        Qt::AutoConnection);
    //Main Window -> Reception Controller
    QObject::connect(m_window, &MainWindow::create_qml,
        m_receptionController, &ReceptionController::create_qml,
        Qt::AutoConnection);

    /*
    //Reception Controller -> DB [init reservations]
    QObject::connect(m_ReceptionController, &ReceptionController::c_reservationInit,
        m_db, &DB::initReservations,
        Qt::AutoConnection);
    //Reception Controller -> DB [create reservation]
    QObject::connect(m_ReceptionController, &ReceptionController::c_reservationCreated,
        m_db, &DB::createReservation,
        Qt::AutoConnection);
    //Reception Controller -> DB [edit reservation]
    QObject::connect(m_ReceptionController, &ReceptionController::c_reservationEdited,
        m_db, &DB::editReservation,
        Qt::AutoConnection);
    //Reception Controller -> DB [remove reservation]
    QObject::connect(m_ReceptionController, &ReceptionController::c_reservationRemoved,
        m_db, &DB::removeReservation,
        Qt::AutoConnection);
	//Reception Controller -> DB [advanced query]
    QObject::connect(m_ReceptionController, &ReceptionController::c_advancedQuery,
        m_db, &DB::advancedQuery,
		Qt::AutoConnection);

    //DB -> Reception Controller [get reservations]
    QObject::connect(m_db, &DB::reservationsGetter,
        m_ReceptionController, &ReceptionController::c_getReservations,
        Qt::AutoConnection);
    //DB -> Reception Controller [created reservation]
    QObject::connect(m_db, &DB::n_ReservationCreated,
        m_ReceptionController, &ReceptionController::createdReservationQml,
        Qt::AutoConnection);
    //DB -> Reception Controller [edited reservation]
    QObject::connect(m_db, &DB::n_ReservationEdited,
        m_ReceptionController, &ReceptionController::editedReservationQml,
        Qt::AutoConnection);
    //DB -> Reception Controller [advanced query]
    QObject::connect(m_db, &DB::reservationAdvanced,
        m_ReceptionController, &ReceptionController::s_reservationsAdvanced,
        Qt::AutoConnection);
    */
}