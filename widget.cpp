#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <utility>
#include "widget.h"
#include "./ui_widget.h"
#include "hmac.h"
#include "number.h"

QString username, password;

Widget::Widget(QWidget *parent)
        : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);
}


Widget::~Widget() {
    delete ui;
}


void Widget::on_set_database_clicked() {
    db.setHostName("localhost");
    db.setUserName(username);
    db.setPassword(password);
    if (db.open()) {
        auto data_list = db.exec("show databases");
        while (data_list.next()) {
            ui->database->addItem(data_list.value(0).toString());
        }
        ui->set_database->setEnabled(false);
    } else {
        qDebug() << QSqlDatabase::database().lastError();
    }
}

void Widget::on_database_currentTextChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    ui->table->clear();
    db.exec("use " + ui->database->currentText());
    auto table_list = db.exec("show tables");
    while (table_list.next()) {
        ui->table->addItem(table_list.value(0).toString());
    }
}

void Widget::on_select_key_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "选择密钥文件", "..", "Key(*.key *KEY)");
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "open failed";
    } else {
        QByteArray text = file.readAll();
        ui->lineEdit->setText(QString::fromStdString(text.toStdString()));
        ui->lineEdit->setReadOnly(true);
    }
}


void Widget::on_show_clicked() {
    QString sql = "SELECT COLUMN_NAME, data_type FROM information_schema.COLUMNS WHERE table_name = \'" + \
                ui->table->currentText() + "\' and table_schema = \'" + ui->database->currentText() + "\'";

    if (db.open()) {
        ui->can_add->clear(); // 先将框框清空
        auto col_list = db.exec(sql);
        while (col_list.next()) {
            if (col_list.value(1).toString() == "double" || col_list.value(1).toString() == "float") {
                ui->can_add->append(col_list.value(0).toString());
            }
        }
        // 这里不用急判断当前选中，后期再处理就行
    }
}

QVector <QString> Widget::getColName() {
    auto string = ui->can_add->toPlainText();
    auto list = string.split('\n');
    QVector <QString> qVector;
    for (const auto &each : list) {
        qVector.push_back(each);
    }
    return qVector;
}

void Widget::addWater() {
    clock_t start = clock();
    std::string key = ui->lineEdit->text().toStdString();
    int i = 0;
    if (db.open()) {
        auto result_list = db.exec("select * from " +
                                   ui->database->currentText() + "." + ui->table->currentText());
        ui->progressBar->setRange(0, result_list.size());
        while (result_list.next()) {
            auto hmac_str = Hmac::calc_hmac(key, result_list.value(0).toString().toStdString());
            auto F = Hmac::calc_hmac(key, hmac_str);
            if (!Hmac::hmac_mod(F, ui->scale->value())) {
                auto attr_index = Hmac::hmac_mod(F, getColName().size());
                auto attr_name = getColName()[attr_index];
                auto bit_index = Hmac::hmac_mod(F, ui->lsb->value());
                mark(result_list.value(0).toString(), attr_name, bit_index);
            }
            ui->progressBar->setValue(++i);
        }
    } else {
        qDebug() << "database not open.";
    }
    clock_t end = clock();
    auto time = -(start - end) * 1.0 / CLOCKS_PER_SEC * 1000;
    QMessageBox::information(nullptr, "信息", "添加完成。总共花费：" + QString("%1ms").arg(time));
}

QString Widget::getPkName() {
    QString sql = "SELECT COLUMN_NAME, COLUMN_KEY FROM information_schema.COLUMNS WHERE table_name = \'" + \
                ui->table->currentText() + "\' and table_schema = \'" + ui->database->currentText() + "\'";
    if (db.open()) {
        auto result_list = db.exec(sql);
        while (result_list.next()) {
            if (result_list.value(1).toString() == "PRI") {
                return result_list.value(0).toString();
            }
        }
    }
    return nullptr;
}

void Widget::mark(const QString &pk_value, const QString &attr_name, int bit_index) {
    auto pk_name = getPkName();
    QString sql = "select " + attr_name + " from " + ui->database->currentText() + "." + \
            ui->table->currentText() + " where " + pk_value + " = " + pk_name;

    if (db.open()) {
        auto result = db.exec(sql);
        if (result.first()) {
            auto number = result.value(0).toString().toStdString();
//            qDebug() << QString::fromStdString(number);
            auto number_string = Number::dec_2_bin(number);
            qDebug() << QString::fromStdString(number) << ' ' << QString::fromStdString(number_string) << ' ' << "bit: "
                     << bit_index;
            auto hmac_str = Hmac::calc_hmac(ui->lineEdit->text().toStdString(), pk_value.toStdString());
            if (Hmac::hmac_mod(hmac_str, 2) == 0) {
                number_string[64 - bit_index - 1] = '0';
            } else {
                number_string[64 - bit_index - 1] = '1';
            }
            auto new_number = Number::bin_2_dec(number_string);
//            qDebug() << QString::fromStdString(new_number) << ' ' << QString::fromStdString(number_string);
            QString sql2 = "update " + ui->database->currentText() + "." + \
                    ui->table->currentText() + " set " + attr_name + " = " + \
                        QString::fromStdString(new_number) + " where " + pk_name + " = " + pk_value;
            db.exec(sql2);
            qDebug() << sql2;
        }
    }
}

void Widget::on_add_clicked() {
    if (ui->lineEdit->text().toStdString().empty()) {
        QMessageBox::information(nullptr, "警告", "请输入密钥!");
    } else {
        addWater();
    }
}

void Widget::on_detect_clicked() {
    if (ui->lineEdit->text().toStdString().empty()) {
        QMessageBox::information(nullptr, "警告", "请输入密钥!");
    } else {
        detectWater();
    }
}

void Widget::detectWater() {
    int total_count = 0, match_count = 0;
    clock_t start = clock();
    std::string key = ui->lineEdit->text().toStdString();
    int i = 0;
    if (db.open()) {
        auto result_list = db.exec("select * from " +
                                   ui->database->currentText() + "." + ui->table->currentText());
        ui->progressBar->setRange(0, result_list.size());
        while (result_list.next()) {
            auto hmac_str = Hmac::calc_hmac(key, result_list.value(0).toString().toStdString());
            auto F = Hmac::calc_hmac(key, hmac_str);
            if (!Hmac::hmac_mod(F, ui->scale->value())) {
                auto attr_index = Hmac::hmac_mod(F, getColName().size());
                auto attr_name = getColName()[attr_index];
                auto bit_index = Hmac::hmac_mod(F, ui->lsb->value());
                ++total_count;
                match_count += match(result_list.value(0).toString(), attr_name, bit_index);
            }
            ui->progressBar->setValue(++i);
        }
    } else {
        qDebug() << "database not open.";
    }
    clock_t end = clock();
    auto time = -(start - end) * 1.0 / CLOCKS_PER_SEC * 1000;
    if (ui->threshold->value() * 0.01 * total_count < match_count) {
        QMessageBox::information(nullptr, "信息",
                                 "检测完成.\n匹配率：" + QString("%1%\n").arg(match_count * 1.0 * 100 / total_count) + \
                                 "盗版数据\n cost: " + QString("%1ms").arg(time));
    } else {
        QMessageBox::information(nullptr, "信息",
                                 "检测完成.\n匹配率：" + QString("%1%\n").arg(match_count * 1.0 * 100 / total_count) + \
                                 "无法判断是否盗版数据\n cost: " + QString("%1ms").arg(time));
    }
}

int Widget::match(const QString &pk_value, const QString &attr_name, int bit_index) {
    auto pk_name = getPkName();
    QString sql = "select " + attr_name + " from " + ui->database->currentText() + "." + \
            ui->table->currentText() + " where " + pk_value + " = " + pk_name;
    if (db.open()) {
        auto result = db.exec(sql);
        if (result.first()) {
            auto number = result.value(0).toString().toStdString();
            std::string number_string = Number::dec_2_bin(number);
            auto hmac_str = Hmac::calc_hmac(ui->lineEdit->text().toStdString(), pk_value.toStdString());
            if (Hmac::hmac_mod(hmac_str, 2) == 0) {
                return number_string[64 - bit_index - 1] == '0' ? 1 : 0;
            } else {
                return number_string[64 - bit_index - 1] == '1' ? 1 : 0;
            }
        }
    } else {
        qDebug() << "database not open on match";
    }
    return 0;
}

void Widget::on_backup_clicked() {
    QString command = "mysqldump -u" + username + " -p" + password + ' ' + ui->database->currentText() + " > " +
                      ui->database->currentText() + ".sql";
    const char *string;
    auto temp = command.toLatin1();
    string = temp.data();
    ::system(string);
    QMessageBox::information(nullptr, "信息", "备份完成");
}

void Widget::on_restore_clicked() {
    QString command = "mysql -u" + username + " -p" + password + ' ' + ui->database->currentText() + " < " +
                      ui->database->currentText() + ".sql";
    const char *string;
    auto temp = command.toLatin1();
    string = temp.data();
    QString filePath = "./" + ui->database->currentText() + ".sql";
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        ::system(string);
        QMessageBox::information(nullptr, "信息", "恢复完成.");
    } else {
        QMessageBox::information(nullptr, "信息", "文件不存在,恢复失败.");
    }
}

void Widget::on_showTable_clicked() {
    tableModel = new QSqlTableModel;
    tableModel->setTable(ui->table->currentText());
    tableModel->select();
    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ui->tableView->setModel(tableModel);
}

void Widget::on_attack_clicked() {
    int i = 0;
    QString pk_name = getPkName();
    if (db.open()) {
        auto result_list = db.exec("select * from " +
                                   ui->database->currentText() + "." + ui->table->currentText());
        while (result_list.next()) {
            ui->progressBar->setRange(0, result_list.size());
            auto seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::seed_seq seed2(result_list.value(0).toString().toStdString().begin(), \
                                    result_list.value(0).toString().toStdString().end());
            std::mt19937 g1(seed2);
            std::mt19937 generator(seed);
            auto hmac_str = Hmac::calc_hmac(std::to_string(g1()), result_list.value(0).toString().toStdString());
            if (!Hmac::hmac_mod(hmac_str, ui->percent->value())) {
                auto attr_index = Hmac::hmac_mod(hmac_str, getColName().size());
                auto attr_name = getColName()[attr_index];
                auto result = db.exec("select " + attr_name + " from " + ui->database->currentText() + "." + \
                                      ui->table->currentText() + " where " + result_list.value(0).toString() + " = " +
                                      pk_name);
                if (result.first()) {
                    auto number = result.value(0).toString().toStdString();
                    auto new_number = Number::attack(number, ui->error_rate->value());
                    QString sql2 = "update " + ui->database->currentText() + "." + \
                            ui->table->currentText() + " set " + attr_name + " = " + \
                                QString::fromStdString(new_number) + " where " + pk_name + " = " +
                                   result_list.value(0).toString();
                    db.exec(sql2);
                }
            }
            ui->progressBar->setValue(++i);
        }
        QMessageBox::information(nullptr, "information", "attack done.");
    }
}

void Widget::on_to_right_clicked()
{
    auto text = ui->can_add->textCursor().selectedText();
    ui->can_not_add->append(text);
    ui->can_add->textCursor().select(QTextCursor::BlockUnderCursor);
    ui->can_add->textCursor().removeSelectedText();
    qDebug() << getColName().size();
}
