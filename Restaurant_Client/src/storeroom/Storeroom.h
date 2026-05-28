#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QSplitter>
#include <QQuickWidget>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QParallelAnimationGroup>
#include <QQuickItem>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QQmlContext>
#include <QPainter>
#include <QTimer>

#include "ui_Storeroom.h"

class Storeroom : public QWidget
{
	Q_OBJECT

public:
	Storeroom(QWidget *parent = nullptr);
	~Storeroom();
	void setUsername(QString username);
	bool eventFilter(QObject* watched, QEvent* event);

	void onTabChanged(int index);
	void closeMenuQml(QWidget* overlay, QQuickWidget* widget, bool& flag);

	void createQml();
	void showUpdate();
	void closeUpdate();

public slots:
	void initArea();
	void categoriesReceived(const QVariantList& categories);
	void categoryItemsReceived(const QVariantList& items);
	void minimalInventoryReceived(const QVariantList& items);
	void onStockCellChanged(int row, int column);
	QVariantMap commitStockUpdates();

	void on_updateButton_clicked();
	void on_endSession_clicked();

signals:
	void getCategories();
	void getFirstArea(int id_category);
	void endSession();

	void requestStockUpdate(QVariantMap updates);

	void getExpandable();

private:
	Ui::StoreroomClass ui;
	QQuickWidget* m_update = nullptr;
	QWidget* m_overlayUpdated = nullptr;
	bool m_updateVisible;
	bool qml_exist = false;

	int m_pendingCategoryId = -1;
	QVariantMap m_pendingStockUpdates;
};

