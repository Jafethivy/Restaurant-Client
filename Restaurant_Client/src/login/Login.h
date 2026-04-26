#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QDialog>

#include "ui_Login.h"


class Login : public QWidget
{
	Q_OBJECT

public:
	Login(QWidget *parent = nullptr);
	~Login();

	void setMainWindow(QMainWindow* mainWindow);

	void Status(const int& area, const bool& status);
	void ShowLoginError();

public slots:
	void on_login_b_clicked();

signals:
	void LoginAttempt(const QString& username, const QString& password);
	void LoginSuccess(const int& area);


private:
	Ui::LoginClass ui;

	QMainWindow* m_mainWindow = nullptr;
};

