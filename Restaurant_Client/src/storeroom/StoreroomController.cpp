#include "StoreroomController.h"

StoreroomController::StoreroomController(Storeroom* w_storeroom, QObject *parent)
	: QObject(parent), m_storeroom(w_storeroom) {

	connect(this, &StoreroomController::initArea,
		m_storeroom, &Storeroom::initArea);
	connect(m_storeroom, &Storeroom::endSession,
		this, &StoreroomController::endSession);

	connect(this, &StoreroomController::categoriesReceived,
		m_storeroom, &Storeroom::categoriesReceived);
	connect(this, &StoreroomController::categoryItemsReceived,
		m_storeroom, &Storeroom::categoryItemsReceived);
	connect(this, &StoreroomController::minimalInventoryReceived,
		m_storeroom, &Storeroom::minimalInventoryReceived);

	connect(m_storeroom, &Storeroom::getCategories,
		this, &StoreroomController::getCategories);
	connect(m_storeroom, &Storeroom::getFirstArea,
		this, &StoreroomController::getFirstArea);
	connect(m_storeroom, &Storeroom::getExpandable,
		this, &StoreroomController::getExpandable);
	connect(m_storeroom, &Storeroom::requestStockUpdate,
		this, &StoreroomController::requestStockUpdate);

}

StoreroomController::~StoreroomController()
{}
