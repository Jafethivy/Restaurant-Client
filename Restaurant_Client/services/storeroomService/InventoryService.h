#pragma once
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantMap>
#include <QNetworkAccessManager>

class InventoryService : public QObject
{
    Q_OBJECT
public:
    explicit InventoryService(QObject* parent = nullptr);
    void setToken(const QString& token);

    void getInventory();
    void getCategories();
    void updateStock(int id_item, double newStock);
    void getByCategory(int id_category);
    void getMinimalInventory();
    void updateBatch(const QVariantMap stocks);

signals:
    void inventoryReceived(const QVariantList& items);
    void categoriesReceived(const QVariantList& categories);
    void categoryItemsReceived(const QVariantList& items);
    void updateFinished(bool success, const QVariantList& response);
    void minimalInventoryReceived(const QVariantList& items);
    void errorOccurred(const QString& message); 

private slots:
    void onInventoryReply();
    void onCategoriesReply();
    void onCategoryItemsReply();
    void onMinimalInventoryReply();
    void onUpdateReply();

private:
    QNetworkAccessManager* m_net;
    QString m_baseUrl = "http://localhost:8080/api";
    QString m_token;

    QVariantMap jsonObjectToVariantMap(const QJsonObject& obj);
};