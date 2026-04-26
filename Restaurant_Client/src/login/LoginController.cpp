#include "LoginController.h"


LoginController::LoginController(Login* window,QObject* parent) 
	: QObject(parent), m_window(window){

	connect(window, &Login::LoginAttempt, this, &LoginController::on_login_attempt);

}

void LoginController::on_login_attempt(const QString& username, const QString& password) {
	emit LoginAttempt(username, password);
}

void LoginController::on_login_status(const int& area, const bool& status) {
	emit LoginStatus(area, status);
}

void LoginController::on_endSession() {
	emit endSession(0);
}
void LoginController::on_endSession_success() {
	emit endSession_success();
}