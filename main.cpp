#include "widget.h"
#include "login.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Widget w;
    Login login;

    if (login.exec() == QDialog::Accepted) {
        w.show();
        return QApplication::exec();
    }
}
