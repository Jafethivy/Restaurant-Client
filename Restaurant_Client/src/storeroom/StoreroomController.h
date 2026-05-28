#pragma once

#include <QObject>
#include "src/storeroom/Storeroom.h"

class StoreroomController  : public QObject
{
	Q_OBJECT

public:
	StoreroomController(Storeroom* w_storeroom = nullptr, QObject* parent = nullptr);
	~StoreroomController();

signals:
	void initArea();
	void getCategories();
	void getFirstArea(int id_category);

	void categoriesReceived(const QVariantList& categories);
	void categoryItemsReceived(const QVariantList& items);
	void minimalInventoryReceived(const QVariantList& items);

	void requestStockUpdate(QVariantMap updates);

	void getExpandable();

	void endSession();

private:
	Storeroom* m_storeroom = nullptr;
};

