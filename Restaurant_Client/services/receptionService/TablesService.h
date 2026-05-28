#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

class TablesService : public QObject
{
    Q_OBJECT
public:
    explicit TablesService(QObject* parent = nullptr);

    void setBaseUrl(const QString& baseUrl);
    void setToken(const QString& token);

    // Métodos públicos que llaman los controllers
    void getAllTables();
    void getTableById(int id);
    void releaseTable(int id);
    void reserveTable(int id);
    void occupyTable(int id);

signals:
    // Señales compatibles con controllers/vista existentes
    void tablesGetter(QVariantList tables);           // GET /api/tables
    void tableGetter(QVariantMap table);              // GET /api/tables/:id
    void tableStatus(QVariantMap result);
    void operationFailed(QString error);              // Error genérico

private slots:
    void onGetAllTablesFinished();
    void onGetTableByIdFinished();
    void onReleaseTableFinished();
    void onReserveTableFinished();
    void onOccupyTableFinished();

private:
    QNetworkAccessManager* m_networkManager;
    QString m_token;
    QString m_baseUrl = "http://localhost:8080/api";

    QNetworkReply* sendRequest(const QString& endpoint,
        const QString& method = "GET",
        const QJsonObject& body = QJsonObject());

    void handleNetworkError(QNetworkReply* reply);
    QVariantMap jsonObjectToVariantMap(const QJsonObject& obj);
};