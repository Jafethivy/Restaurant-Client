#pragma once
#include <QObject>
#include <QThread>
#include <memory>

class MainWindow;
class Login;
class Reception;

class LoginController;
class ReceptionController;

class AuthService;
class ReservationService;

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
private:
    QThread WorkerThread;

    MainWindow* m_window = nullptr;
	Login* m_loginWidget = nullptr;
	Reception* m_receptionWidget = nullptr;

    LoginController* m_loginController = nullptr;
	ReceptionController* m_receptionController = nullptr;

    AuthService* m_authService = nullptr;
    ReservationService* m_reservationService = nullptr;
};