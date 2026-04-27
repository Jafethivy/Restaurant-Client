#pragma once

#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QNetworkAccessManager>

class ReservationService : public QObject
{
    Q_OBJECT
public:
    explicit ReservationService(QObject* parent = nullptr);

    void initReservations();
    void createReservation(QVariantMap m_data);
    void editReservation(QVariantMap m_data);
    void removeReservation(QVariant index);
    void advancedQuery(QVariantMap n_data);

    void setToken(const QString& token);
    void setBaseUrl(const QString& url);

signals:
    void reservationsGetter(QVariantList reservations);
    void n_ReservationCreated(QVariantMap m_data);
    void n_ReservationEdited(QVariantMap m_data);
    void reservationAdvanced(QVariantList reservations);

private:
    QNetworkAccessManager* m_manager;
    QString m_token;
    QString m_baseUrl = "http://localhost:8080/api";

    QNetworkRequest buildRequest(const QString& path) const;
    static QVariantMap jsonObjectToVariantMap(const QJsonObject& obj);
    static QJsonObject variantMapToJsonObject(const QVariantMap& map);
};