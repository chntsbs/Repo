#ifndef CRYPTO_H
#define CRYPTO_H
#include <crypto++/aes.h>
#include <crypto++/base64.h>
#include <crypto++/filters.h>
#include <crypto++/hex.h>
#include <crypto++/modes.h>
#include <crypto++/sha.h>
#include <string>
#include <stdlib.h>
#include"QByteArray"
using namespace std;
class Crypto
{
public:
    Crypto();
    string encrypt(string password);
    string encryptByteArray(QByteArray password);
    string decrypt(string password);
};

#endif // CRYPTO_H
