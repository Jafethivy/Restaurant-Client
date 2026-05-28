#pragma once
#include <QObject>
#include <QThread>
#include <memory>

class MainWindow;
class Login;
class Reception;
class Storeroom;

class LoginController;
class ReceptionController;
class StoreroomController;

class AuthService;
class ReservationService;
class TablesService;
class InventoryService;

class WebSocketClient;

class AppContext : public QObject {
    Q_OBJECT
public:
    explicit AppContext(QObject* parent = nullptr);
    ~AppContext();

    // No copiable
    AppContext(const AppContext&) = delete;
    AppContext& operator=(const AppContext&) = delete;

    // Inicializa todo y retorna la ventana principal
	MainWindow* initialize();

    void Demand(int area);

private:
    void createObjects();
    void setupThreads();
    void setupConnections() const;
    void cleanup();
	void debugConnections();

    void demandReception();
	void connectionsReception() const;
	void wsReceptionConnections() const;

    void demandStoreroom();
    void connectionsStoreroom() const;
    void wsStoreroomConnections() const;

signals:
    void NoArea();

private:
    QString m_wsUrl = "ws://localhost:8081";
    QThread WorkerThread;

    MainWindow* m_window = nullptr;
	Login* m_loginWidget = nullptr;
	Reception* m_receptionWidget = nullptr;
    Storeroom* m_storeroomWidget = nullptr;

    LoginController* m_loginController = nullptr;
	ReceptionController* m_receptionController = nullptr;
    StoreroomController* m_storeroomController = nullptr;

    AuthService* m_authService = nullptr;
    ReservationService* m_reservationService = nullptr;
    TablesService* m_tablesService = nullptr;
    InventoryService* m_inventoryService = nullptr;

	WebSocketClient* m_webSocketClient = nullptr;
};