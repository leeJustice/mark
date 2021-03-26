#include <openssl/hmac.h>
#include <openssl/bn.h>
#include <iomanip>
#include <sstream>

#include "hmac.h"

using namespace std;

Hmac::Hmac() = default;

int Hmac::hmac_mod(const string &str, int number) {
    BIGNUM *ret = BN_new();
    BN_set_word(ret, number);// 将num存在了ret中
    BIGNUM *a = BN_new(); // 接收被除数
    BN_hex2bn(&a, str.c_str());// 接收十六进制字符串转化为大整数a
    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *rem = BN_new();
    BN_mod(rem, a, ret, ctx);
    char *result = BN_bn2dec(rem);
    free(ret);
    free(a);
    free(rem);
    BN_CTX_free(ctx);
    return atoi(result);
}

string Hmac::calc_hmac(const std::string &key, const string &msg) {
    unsigned int hash_sz = EVP_MAX_MD_SIZE;
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_CTX_reset(ctx);
    auto digest = HMAC(EVP_md5(), key.c_str(), key.size(),
                       (unsigned char *) msg.c_str(), msg.size(),
                       nullptr, &hash_sz);
    std::stringstream ss;
    ss << std::setfill('0');
    for (size_t i = 0; i < hash_sz; ++i) {
        ss << std::hex << std::setw(2) << (unsigned int) digest[i];
    }
    std::string final_hash = ss.str();
    return final_hash;
}
