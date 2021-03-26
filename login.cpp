#include "login.h"
#include "ui_login.h"

extern QString username, password;

Login::Login(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Login) {
    ui->setupUi(this);
}

Login::~Login() {
    delete ui;
}

void Login::on_exit_clicked() {
    close();
}

void Login::on_enter_clicked() {
    username = ui->username->text();
    password = ui->password->text();
    accept();
}
