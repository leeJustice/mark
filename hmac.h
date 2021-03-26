#ifndef HMAC_H
#define HMAC_H


#include <string>

class Hmac {
public:
    Hmac();

    static int hmac_mod(const std::string &str, int number);

    static std::string calc_hmac(const std::string &key, const std::string &msg);
};

#endif // HMAC_H
