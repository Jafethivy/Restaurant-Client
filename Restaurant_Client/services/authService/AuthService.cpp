#include "AuthService.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

AuthService::AuthService(QObject* parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
}

void AuthService::attemptLogin(const QString& username, const QString& password){
    QUrl url(m_baseUrl + "/auth/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["username"] = username;
    body["password"] = password;

    QNetworkReply* reply = m_manager->post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact)
    );

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "[AuthService] Error de red:" << reply->errorString();
            emit LoginStatus(0, false);
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();
        
        bool success = obj["success"].toBool();
        int area = obj["area"].toInt(-1);
        QString token = obj["token"].toString();

        if (success && !token.isEmpty()) {
            m_token = token;
            m_username = obj["username"].toString();
            m_area = area;
            emit LoginStatus(area, true);
        }
        else {
            qDebug() << "[AuthService] Login fallido:" << obj["error"].toString();
            emit LoginStatus(0, false);
        }

        reply->deleteLater();
        });
}

void AuthService::logout(){
    if (m_token.isEmpty()) return;

    QUrl url(m_baseUrl + "/auth/logout");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["token"] = m_token;

    QNetworkReply* reply = m_manager->post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact)
    );

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        m_token.clear();
        m_username.clear();
        m_area = 0;
        reply->deleteLater();
        });
}

void AuthService::updateStatusDb_close(const int& status){
    Q_UNUSED(status)
    logout();
    emit endSession_success();
}

QString AuthService::token() const
{
    return m_token;
}

int AuthService::area() const
{
    return m_area;
}