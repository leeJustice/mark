#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSql>
#include "login.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    QSqlTableModel *tableModel;

    QVector<QString> getColName();

    QString getPkName();

    void addWater();

    void mark(const QString &pk_value, const QString &attr_name, int bit_index);

    void detectWater();

    int match(const QString &, const QString &attr_name, int bit_index);

    ~Widget() override;

private slots:

    void on_set_database_clicked();

    void on_database_currentTextChanged(const QString &arg1);

    void on_select_key_clicked();

    void on_show_clicked();

    void on_add_clicked();

    void on_detect_clicked();

    void on_backup_clicked();

    void on_restore_clicked();

    void on_showTable_clicked();

    void on_attack_clicked();

    void on_to_right_clicked();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
