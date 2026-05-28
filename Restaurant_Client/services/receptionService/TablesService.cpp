#include "TablesService.h"
#include <QDebug>
#include <QUrl>
#include <QJsonArray>

TablesService::TablesService(QObject* parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this))
{}

void TablesService::setBaseUrl(const QString& baseUrl){
    m_baseUrl = baseUrl;
    if (m_baseUrl.endsWith("/")) {
        m_baseUrl.chop(1);
    }
}

void TablesService::setToken(const QString& token){
    m_token = token;
}

// ============================================
// GET /api/tables
// ============================================
void TablesService::getAllTables(){
    QNetworkReply* reply = sendRequest("/tables");
    connect(reply, &QNetworkReply::finished, this, &TablesService::onGetAllTablesFinished);
}

void TablesService::onGetAllTablesFinished(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        handleNetworkError(reply);
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray tablesArray = doc.array();

    QVariantList tablesList;
    for (const QJsonValue& value : tablesArray) {
        tablesList.append(jsonObjectToVariantMap(value.toObject()));
    }

    emit tablesGetter(tablesList);
}

// ============================================
// GET /api/tables/:id
// ============================================
void TablesService::getTableById(int id){
    QNetworkReply* reply = sendRequest(QString("/tables/%1").arg(id));
    connect(reply, &QNetworkReply::finished, this, &TablesService::onGetTableByIdFinished);
}

void TablesService::onGetTableByIdFinished(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        handleNetworkError(reply);
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject tableObj = doc.object();

    emit tableGetter(jsonObjectToVariantMap(tableObj));
}

// ============================================
// PUT /api/tables/:id/release
// ============================================
void TablesService::releaseTable(int id){
    QNetworkReply* reply = sendRequest(QString("/tables/%1/release").arg(id), "PUT");
    connect(reply, &QNetworkReply::finished, this, &TablesService::onReleaseTableFinished);
}

void TablesService::onReleaseTableFinished(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        handleNetworkError(reply);
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject result = doc.object();

    if (result["success"].toBool()) {
        QVariantMap resultMap = result.toVariantMap();
        emit tableStatus(resultMap);
    }
    else {
        emit operationFailed(result["error"].toString());
    }
}

// ============================================
// PUT /api/tables/:id/reserve
// ============================================
void TablesService::reserveTable(int id){
    QNetworkReply* reply = sendRequest(QString("/tables/%1/reserve").arg(id), "PUT");
    connect(reply, &QNetworkReply::finished, this, &TablesService::onReserveTableFinished);
}

void TablesService::onReserveTableFinished(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        handleNetworkError(reply);
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject result = doc.object();

    if (result["success"].toBool()) {
        QVariantMap resultMap = result.toVariantMap();
        emit tableStatus(resultMap);
    }
    else {
        emit operationFailed(result["error"].toString());
    }
}

// ============================================
// PUT /api/tables/:id/occupy
// ============================================
void TablesService::occupyTable(int id){
    QNetworkReply* reply = sendRequest(QString("/tables/%1/occupy").arg(id), "PUT");
    connect(reply, &QNetworkReply::finished, this, &TablesService::onOccupyTableFinished);
}

void TablesService::onOccupyTableFinished(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        handleNetworkError(reply);
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject result = doc.object();

    if (result["success"].toBool()) {
        QVariantMap resultMap = result.toVariantMap();
        emit tableStatus(resultMap);
    }
    else {
        emit operationFailed(result["error"].toString());
    }
}

// ============================================
// Helpers privados
// ============================================
QNetworkReply* TablesService::sendRequest(const QString& endpoint,
    const QString& method,
    const QJsonObject& body){

    QUrl url(m_baseUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_token).toUtf8());
    }

    QNetworkReply* reply = nullptr;

    if (method == "GET") {
        reply = m_networkManager->get(request);
    }
    else if (method == "PUT") {
        QJsonDocument doc(body);
        reply = m_networkManager->put(request, doc.toJson());
    }
    else if (method == "POST") {
        QJsonDocument doc(body);
        reply = m_networkManager->post(request, doc.toJson());
    }
    else if (method == "DELETE") {
        reply = m_networkManager->deleteResource(request);
    }

    return reply;
}

void TablesService::handleNetworkError(QNetworkReply* reply){
    QString errorMsg = reply->errorString();
    qDebug() << "[TablesService] Network error:" << errorMsg;
    emit operationFailed(errorMsg);
}

QVariantMap TablesService::jsonObjectToVariantMap(const QJsonObject& obj){
    QVariantMap map;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        map.insert(it.key(), it.value().toVariant());
    }
    return map;
}