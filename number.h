#ifndef NUMBER_H
#define NUMBER_H


#include <string>

class Number {
public:
    Number();

    static std::string dec_2_bin(const std::string &number);

    static std::string bin_2_dec(const std::string &binary);

    static std::string attack(const std::string &number, double error_rate);
};

#endif // NUMBER_H
