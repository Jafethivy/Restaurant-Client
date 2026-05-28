#pragma once
#include <QObject>
#include <QJsonObject>
#include <QTimer>

QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebSocketClient : public QObject
{
    Q_OBJECT
        Q_PROPERTY(ConnectionState connectionState READ connectionState NOTIFY connectionStateChanged)

public:
    explicit WebSocketClient(QObject* parent = nullptr);
    ~WebSocketClient();

    enum ConnectionState {
        Disconnected,
        Connecting,
        Connected,
        Identified,
        Error
    };
    Q_ENUM(ConnectionState)

        ConnectionState connectionState() const;

    Q_INVOKABLE void connectToServer(const QString& url, const QString& token);
    Q_INVOKABLE void disconnectFromServer();

signals:
    void connectionStateChanged(ConnectionState state);
    void connected();
    void identified();
    void disconnected();
    void error(const QString& message);

    // --- Notificaciones de negocio (parseadas del servidor) ---
    void tableStatusChange(QVariantMap result);

    void orderCreated(int orderId, int tableId);
    void orderUpdated(int orderId, int tableId);
    void orderCompleted(int orderId, int tableId);
    void orderCancelled(int orderId, int tableId);

    void reservationOverdue();

    // Señal genérica por si necesitas manejar algo extra en QML/C++
    void notificationReceived(const QString& type, const QJsonObject& payload);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessage(const QString& message);
    void onSocketError();
    void tryReconnect();

private:
    void setState(ConnectionState state);
    void sendJson(const QJsonObject& obj);
    void performHandshake();
    void handleNotification(const QString& type, const QJsonObject& payload);

    QWebSocket* m_socket;
    ConnectionState m_state;
    QString m_url;
    QString m_token;
    QTimer* m_reconnectTimer;
    int m_reconnectAttempts;
};