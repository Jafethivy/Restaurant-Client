#include "WebSocketClient.h"
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QDebug>

WebSocketClient::WebSocketClient(QObject* parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_state(Disconnected)
    , m_reconnectTimer(new QTimer(this))
    , m_reconnectAttempts(0)
{
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &WebSocketClient::tryReconnect);
}

WebSocketClient::~WebSocketClient(){
    disconnectFromServer();
}

WebSocketClient::ConnectionState WebSocketClient::connectionState() const {
    return m_state;
}

void WebSocketClient::setState(ConnectionState state) {
    if (m_state == state)
        return;

    m_state = state;
    emit connectionStateChanged(m_state);

    if (state == Connected)
        emit connected();
    else if (state == Identified)
        emit identified();
    else if (state == Disconnected)
        emit disconnected();
    else if (state == Error)
        emit error(QStringLiteral("WebSocket error state"));
}

void WebSocketClient::connectToServer(const QString& url, const QString& token) {
    m_url = url;
    m_token = token;

    if (m_socket) {
        disconnectFromServer();
    }
    if (sender() != m_reconnectTimer) {
        m_reconnectAttempts = 0;
    }

    m_socket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);

    connect(m_socket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessage);
    connect(m_socket, &QWebSocket::errorOccurred, this, &WebSocketClient::onSocketError);

    setState(Connecting);
    m_socket->open(QUrl(url));
}

void WebSocketClient::disconnectFromServer()
{
    m_reconnectTimer->stop();

    if (m_socket) {
        m_socket->close();
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    setState(Disconnected);
}

void WebSocketClient::sendJson(const QJsonObject& obj)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    QJsonDocument doc(obj);
    m_socket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void WebSocketClient::performHandshake()
{
    QJsonObject handshake;
    handshake["action"] = QStringLiteral("identify");
    handshake["token"] = m_token;

    qDebug() << "[WS Client] Sending handshake";
    sendJson(handshake);
}

void WebSocketClient::onConnected()
{
    qDebug() << "[WS Client] Socket connected";
    setState(Connected);
    performHandshake();
}

void WebSocketClient::onDisconnected()
{
    qDebug() << "[WS Client] Socket disconnected";
    m_socket->deleteLater();
    m_socket = nullptr;

    setState(Disconnected);

    // Auto-reconexión con backoff
    int delay = qMin(3000 * (m_reconnectAttempts + 1), 30000); // max 30s
    m_reconnectAttempts++;
    qDebug() << "[WS Client] Reconnecting in" << delay << "ms (attempt" << m_reconnectAttempts << ")";
    m_reconnectTimer->start(delay);
}

void WebSocketClient::onSocketError()
{
    if (m_socket) {
        qWarning() << "[WS Client] Error:" << m_socket->errorString();
        emit error(m_socket->errorString());
    }
}

void WebSocketClient::tryReconnect()
{
    if (m_url.isEmpty() || m_token.isEmpty())
        return;

    qDebug() << "[WS Client] Attempting reconnect...";
    connectToServer(m_url, m_token);
}

void WebSocketClient::onTextMessage(const QString& message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject())
        return;

    QJsonObject obj = doc.object();
    QString type = obj.value("type").toString();
    QJsonObject payload = obj.value("payload").toObject();

    if (type == QLatin1String("identified")) {
        qDebug() << "[WS Client] Identified successfully";
        setState(Identified);
        return;
    }

    if (type == QLatin1String("error")) {
        QString msg = payload.value("message").toString();
        if (msg.isEmpty()) msg = obj.value("message").toString();
        qWarning() << "[WS Client] Server error:" << msg;
        emit error(msg);
        return;
    }

    // Notificación de negocio
    handleNotification(type, payload);
    emit notificationReceived(type, payload);
}

void WebSocketClient::handleNotification(const QString& type, const QJsonObject& payload){
    qDebug() << type;
    qDebug() << payload.toVariantMap();
    if (type == QLatin1String("table_released")) {
        QVariantMap result = payload.toVariantMap();
        emit tableStatusChange(result);

    }
    else if (type == QLatin1String("table_occupied")) {
        QVariantMap result = payload.toVariantMap();
        emit tableStatusChange(result);

    }
    else if (type == QLatin1String("table_reserved")) {
        QVariantMap result = payload.toVariantMap();
        emit tableStatusChange(result);

    }
    else if (type == QLatin1String("order_created")) {
        qDebug() << type;
        int orderId = payload.value("order_id").toInt();
        int tableId = payload.value("table_id").toInt();
        emit orderCreated(orderId, tableId);

    }
    else if (type == QLatin1String("order_updated")) {
        qDebug() << type;
        int orderId = payload.value("order_id").toInt();
        int tableId = payload.value("table_id").toInt();
        emit orderUpdated(orderId, tableId);

    }
    else if (type == QLatin1String("order_completed")) {
        qDebug() << type;
        int orderId = payload.value("order_id").toInt();
        int tableId = payload.value("table_id").toInt();
        emit orderCompleted(orderId, tableId);

    }
    else if (type == QLatin1String("order_cancelled")) {
        qDebug() << type;
        int orderId = payload.value("order_id").toInt();
        int tableId = payload.value("table_id").toInt();
        emit orderCancelled(orderId, tableId);
    }
    else if (type == QLatin1String("reservation_overdue")) {
        emit reservationOverdue();
    }
}