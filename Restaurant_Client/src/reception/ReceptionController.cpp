#include "ReceptionController.h"

ReceptionController::ReceptionController(Reception* w_reception, QObject *parent)
	: QObject(parent), m_receptionWidget(w_reception){

	connect(m_receptionWidget, &Reception::endSession,
		this, &ReceptionController::on_end_session);

	connect(m_receptionWidget, &Reception::signalReservationInit,
		this, &ReceptionController::c_reservationInit);
	connect(m_receptionWidget, &Reception::signalReservationCreated,
		this, &ReceptionController::c_reservationCreated);
	connect(m_receptionWidget, &Reception::signalReservationEdited,
		this, &ReceptionController::c_reservationEdited);
	connect(m_receptionWidget, &Reception::signalReservationRemoved,
		this, &ReceptionController::c_reservationRemoved);
	connect(m_receptionWidget, &Reception::signalAdvancedQuery,
		this, &ReceptionController::c_advancedQuery);
	connect(m_receptionWidget, &Reception::signalReservationUpdate,
		this, &ReceptionController::signalReservationUpdate);

	connect(this, &ReceptionController::c_getReservations,
		m_receptionWidget, &Reception::init_reservations);
	connect(this, &ReceptionController::c_createdReservationQml,
		m_receptionWidget, &Reception::reservationCreatedQml);
	connect(this, &ReceptionController::c_editedReservationQml,
		m_receptionWidget, &Reception::reservationEditedQml);
	connect(this, &ReceptionController::s_reservationsAdvanced,
		m_receptionWidget, &Reception::reservationAdvanced);
	connect(this, &ReceptionController::reservationUpdated,
		m_receptionWidget,&Reception::reservationUpdated);

	connect(this, &ReceptionController::create_qml,
		m_receptionWidget, &Reception::create_qml);

	connect(m_receptionWidget, &Reception::signalTablesInit,
		this, &ReceptionController::signalTablesInit);
	connect(m_receptionWidget, &Reception::reserveTable,
		this, &ReceptionController::reserveTable);
	connect(m_receptionWidget, &Reception::occupyTable,
		this, &ReceptionController::occupyTable);

	connect(this, &ReceptionController::tablesGetter,
		m_receptionWidget, &Reception::tablesGetter);
	connect(this, &ReceptionController::tableStatus,
		m_receptionWidget, &Reception::tableStatus);

	connect(this, &ReceptionController::reservationOverdue,
		m_receptionWidget, &Reception::signalReservationInit);
}

ReceptionController::~ReceptionController()
{}

void ReceptionController::on_end_session() {
	emit updateSession();
	emit endSession();
}

void ReceptionController::createdReservationQml(QVariantMap n_data) {
	emit c_createdReservationQml(n_data);
}
void ReceptionController::editedReservationQml(QVariantMap n_data) {
	emit c_editedReservationQml(n_data);
}
