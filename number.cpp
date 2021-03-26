#include <climits>
#include "number.h"
#include <bitset>
#include <QtCore/QString>
#include <QtCore/QProcess>
#include <QtDebug>

Number::Number() = default;

std::string Number::dec_2_bin(const std::string &number) {
    QString command = "python3 ./dec_2_bin.py " + QString::fromStdString(number);
    QProcess process;
    process.start(command);
    process.waitForFinished();
    auto output = process.readAllStandardOutput();
    auto result = output.toStdString();
    result.erase(result.size() - 1);
    return result;
}

std::string Number::bin_2_dec(const std::string &binary) {
    QString command = "python3 ./bin_2_dec.py " + QString::fromStdString(binary);
    QProcess process;
    process.start(command);
    process.waitForFinished();
    auto output = process.readAllStandardOutput();
    auto result = output.toStdString();
    result.erase(result.size() - 1);
    return result;
}

std::string Number::attack(const std::string &number, double error_rate)
{
    QString command = "python3 ./attack.py " + QString::fromStdString(number) + " " + QString::fromStdString(std::to_string(error_rate));
    QProcess process;
    process.start(command);
    process.waitForFinished();
    auto output = process.readAllStandardOutput();
    auto result = output.toStdString();
    result.erase(result.size() - 1);
    return result;
}

