#pragma once
#include <QObject>
#include <QThread>
#include <memory>

class MainWindow;
class Login;
class Reception;

class LoginController;
class ReceptionController;

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
    void setupConnections();
    void cleanup();
	void debugConnections();

    void demandReception();
private:
    // Hilo del modelo
    QThread WorkerThread;
    
    // Objetos (raw pointers porque manejamos lifetime manualmente con deleteLater)
	MainWindow* m_window = nullptr;
    
    //Login StackedWidget
	Login* m_loginWidget = nullptr;
	//Reception StackedWidget
	Reception* m_receptionWidget = nullptr;

	//Controllers
    LoginController* m_controller = nullptr;
	ReceptionController* m_receptionController = nullptr;
};