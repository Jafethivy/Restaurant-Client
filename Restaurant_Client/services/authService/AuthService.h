#pragma once
#include <QObject>
#include <QNetworkAccessManager>

class AuthService : public QObject
{
    Q_OBJECT
public:
    explicit AuthService(QObject* parent = nullptr);

    void attemptLogin(const QString& username, const QString& password);
    void logout();
    void updateStatusDb_close(const int& status);

    QString token() const;
    int area() const;

signals:
    void LoginStatus(int area, bool status);
    void endSession_success();

private:
    QNetworkAccessManager* m_manager;
    QString m_token;
    QString m_username;
    int m_area = 0;
    QString m_baseUrl = "http://localhost:8080/api";
};
