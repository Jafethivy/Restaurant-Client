#pragma once

#include <QObject>
#include "src/reception/Reception.h"


class ReceptionController  : public QObject
{
	Q_OBJECT

public:
	ReceptionController(Reception* w_reception = nullptr, QObject *parent = nullptr);
	~ReceptionController();

	void on_end_session();
	void createdReservationQml(QVariantMap n_data);
	void editedReservationQml(QVariantMap n_data);

signals:
	void updateSession();
	void endSession();

	void c_reservationInit();
	void c_getReservations(QVariantList reservations);
	void c_reservationCreated(QVariantMap m_data);
	void c_reservationEdited(QVariantMap m_data);
	void c_reservationRemoved(QVariant index);
	void signalReservationUpdate(QVariant id_resv, int status);

	void c_createdReservationQml(QVariantMap n_data);
	void c_editedReservationQml(QVariantMap n_data);
	void reservationUpdated(int id_resv);

	void create_qml();
	void c_advancedQuery(QVariantMap n_data);
	void s_reservationsAdvanced(QVariantList reservations);

	void signalTablesInit();
	void reserveTable(int tableId);
	void occupyTable(int tableId);

	void tablesGetter(QVariantList tables);
	void tableStatus(QVariantMap result);

	void reservationOverdue();

private:
	Reception* m_receptionWidget = nullptr;
};

