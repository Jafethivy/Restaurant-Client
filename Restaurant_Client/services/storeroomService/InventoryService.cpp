#include "InventoryService.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

InventoryService::InventoryService(QObject* parent)
    : QObject(parent)
{
    m_net = new QNetworkAccessManager(this);
}

void InventoryService::setToken(const QString& token) {
    m_token = token;
}

void InventoryService::getInventory()
{
    QNetworkRequest req(QUrl(m_baseUrl + "/inventory"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());

    QNetworkReply* reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, &InventoryService::onInventoryReply);
}

void InventoryService::getCategories()
{
    QNetworkRequest req(QUrl(m_baseUrl + "/inventory/categories"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());

    QNetworkReply* reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, &InventoryService::onCategoriesReply);
}

void InventoryService::getByCategory(int id_category){
    QNetworkRequest req(QUrl(m_baseUrl + "/inventory/category/" + QString::number(id_category)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());

    QNetworkReply* reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, &InventoryService::onCategoryItemsReply);
}


void InventoryService::updateStock(int id_item, double newStock)
{
    QNetworkRequest req(QUrl(m_baseUrl + "/inventory/" + QString::number(id_item)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());

    QJsonObject body;
    body["current_stock"] = newStock;

    QNetworkReply* reply = m_net->put(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, &InventoryService::onUpdateReply);
}

void InventoryService::onInventoryReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray array = doc.array();
    QVariantList list = array.toVariantList();

    emit inventoryReceived(list);
    reply->deleteLater();
}

void InventoryService::onCategoriesReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray array = doc.array();
    QVariantList list = array.toVariantList();

    emit categoriesReceived(list);
    reply->deleteLater();
}

void InventoryService::onCategoryItemsReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray array = doc.array();
    QVariantList list = array.toVariantList();

    emit categoryItemsReceived(list);
    reply->deleteLater();
}

void InventoryService::onUpdateReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    bool ok = (reply->error() == QNetworkReply::NoError);
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray array = doc.array();
    QVariantList list = array.toVariantList();

    reply->deleteLater();
}

QVariantMap InventoryService::jsonObjectToVariantMap(const QJsonObject& obj) {
    QVariantMap map;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        map.insert(it.key(), it.value().toVariant());
    }
    return map;
}

void InventoryService::getMinimalInventory()
{
    QNetworkRequest req(QUrl(m_baseUrl + "/inventory/minimal"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());

    QNetworkReply* reply = m_net->get(req);
    connect(reply, &QNetworkReply::finished, this, &InventoryService::onMinimalInventoryReply);
}

void InventoryService::onMinimalInventoryReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray array = doc.array();
    QVariantList list = array.toVariantList();

    emit minimalInventoryReceived(list);
    reply->deleteLater();
}

void InventoryService::updateBatch(const QVariantMap stocks)
{
    QNetworkRequest req(QUrl(m_baseUrl + "/inventory/batch"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());

    QJsonObject body;
    for (auto it = stocks.begin(); it != stocks.end(); ++it) {
        body[it.key()] = it.value().toDouble();
    }

    QNetworkReply* reply = m_net->put(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, &InventoryService::onUpdateReply);
}