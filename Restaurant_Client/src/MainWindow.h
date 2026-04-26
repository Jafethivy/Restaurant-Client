#pragma once

#include <QtWidgets/QMainWindow>
#include <QDialog>
#include <QCloseEvent>
#include <QTimer>
#include <QHash>
#include <QString>
#include <QStackedWidget>
#include "ui_MainWindow.h"

class Login;
class Reception;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    //Helpers
    Login* loginWidget() const;
    Reception* receptionWidget() const;

    void screen_area();

    //StackedWidget
    void set_login();
    void set_area(const int& area);

    void logout();

signals:
    void closingRequested();
    void closeApproved();

    void exist(int area);
    void change_QSWidget();

    void create_qml();


public slots:
    void onCloseApproved();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupAreas();
    bool ensureAreaExists(int areaId);
    void switchToArea(int areaId);


private:
    Ui::MainWindowClass ui;

    bool m_closingPending = false;
    QCloseEvent* m_pendingCloseEvent = nullptr;

    Login* loginWindow = nullptr;
    Reception* receptionWindow = nullptr;

    int width_screen;
    int height_screen;

    struct AreaConfig {
        QString name;
        std::function<void()> create;  // Lambda que asigna a la variable correspondiente
    };

    QHash<int, AreaConfig> m_areas;
    QHash<int, bool> m_initialized;
};

