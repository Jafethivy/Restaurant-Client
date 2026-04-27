#include "ReservationService.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

ReservationService::ReservationService(QObject* parent) : QObject(parent){
    m_manager = new QNetworkAccessManager(this);
}

void ReservationService::setToken(const QString& token){
    m_token = token;
}

void ReservationService::setBaseUrl(const QString& url){
    m_baseUrl = url;
}

QNetworkRequest ReservationService::buildRequest(const QString& path) const{
    QUrl url(m_baseUrl + path);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_token).toUtf8());
    }
    return request;
}

QVariantMap ReservationService::jsonObjectToVariantMap(const QJsonObject& obj){
    QVariantMap map;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        map.insert(it.key(), it.value().toVariant());
    }
    return map;
}

QJsonObject ReservationService::variantMapToJsonObject(const QVariantMap& map){
    QJsonObject obj;
    for (auto it = map.begin(); it != map.end(); ++it) {
        obj.insert(it.key(), QJsonValue::fromVariant(it.value()));
    }
    return obj;
}

// ============================================
// GET /api/reservations
// ============================================
void ReservationService::initReservations(){
    QNetworkRequest request = buildRequest("/reservations");
    QNetworkReply* reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "[ReservationService] Error initReservations:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray array = doc.array();

        QVariantList reservations;
        for (const auto& val : array) {
            reservations.append(jsonObjectToVariantMap(val.toObject()));
        }

        emit reservationsGetter(reservations);
        reply->deleteLater();
        });
}

// ============================================
// POST /api/reservations
// ============================================
void ReservationService::createReservation(QVariantMap m_data){
    QNetworkRequest request = buildRequest("/reservations");

    // id_resv no se envía, el servidor lo genera
    m_data.remove("id_resv");

    QJsonObject body = variantMapToJsonObject(m_data);
    QNetworkReply* reply = m_manager->post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact)
    );

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "[ReservationService] Error createReservation:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QVariantMap result = jsonObjectToVariantMap(doc.object());

        emit n_ReservationCreated(result);
        reply->deleteLater();
        });
}

// ============================================
// PUT /api/reservations/:id
// ============================================
void ReservationService::editReservation(QVariantMap m_data){
    int id = m_data["id_resv"].toInt();
    m_data.remove("id_resv");

    QNetworkRequest request = buildRequest("/reservations/" + QString::number(id));
    QJsonObject body = variantMapToJsonObject(m_data);

    QNetworkReply* reply = m_manager->put(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact)
    );

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "[ReservationService] Error editReservation:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QVariantMap result = jsonObjectToVariantMap(doc.object());

        emit n_ReservationEdited(result);
        reply->deleteLater();
        });
}

// ============================================
// DELETE /api/reservations/:id
// ============================================
void ReservationService::removeReservation(QVariant index)
{
    int id = index.toInt();
    QNetworkRequest request = buildRequest("/reservations/" + QString::number(id));

    QNetworkReply* reply = m_manager->deleteResource(request);

    connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "[ReservationService] Error removeReservation:" << reply->errorString();
        }
        reply->deleteLater();
        });
}

// ============================================
// POST /api/reservations/search
// ============================================
void ReservationService::advancedQuery(QVariantMap n_data)
{
    QNetworkRequest request = buildRequest("/reservations/search");
    QJsonObject body = variantMapToJsonObject(n_data);

    QNetworkReply* reply = m_manager->post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact)
    );

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "[ReservationService] Error advancedQuery:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray array = doc.array();

        QVariantList reservations;
        for (const auto& val : array) {
            reservations.append(jsonObjectToVariantMap(val.toObject()));
        }

        emit reservationAdvanced(reservations);
        reply->deleteLater();
        });
}