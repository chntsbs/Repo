#include "crypto.h"
#include "config.h"
#include <string>
#include <stdlib.h>
#include"syslog.h"
#include <global.h>
using namespace CryptoPP;
using namespace std;
Crypto::Crypto()
{
}


string Crypto::encrypt(string password)
{
    string plain = password;
    string ciphertext;
    try{
        byte iv[AES::BLOCKSIZE];
        byte key[AES::DEFAULT_KEYLENGTH] = { 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1 };
        for (int i = 0; i < AES::BLOCKSIZE; i++) {
            iv[i] = 0;
        }
        CBC_Mode<AES>::Encryption Encryptor(key, sizeof(key), iv);
        StringSource(plain, true, new StreamTransformationFilter(Encryptor,
                                                                 new Base64Encoder(new StringSink(ciphertext))));
    }
    catch( const CryptoPP::Exception& e )
    {

        if(test) qDebug()<<"encrypt"<<QString::fromStdString(e.what());
        settings.setlog("CRYPTO_DEBUG "+QString::fromStdString(e.what()));
    }
    return ciphertext;
}

string Crypto::encryptByteArray(QByteArray password)
{
    string plain = QString::fromLatin1(password).toStdString();
    string ciphertext;
    try{
        byte iv[AES::BLOCKSIZE];
        byte key[AES::DEFAULT_KEYLENGTH] = { 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1 };
        for (int i = 0; i < AES::BLOCKSIZE; i++) {
            iv[i] = 0;
        }
        CBC_Mode<AES>::Encryption Encryptor(key, sizeof(key), iv);
        StringSource(plain, true, new StreamTransformationFilter(Encryptor,
                                                                 new Base64Encoder(new StringSink(ciphertext))));
    }
    catch( const CryptoPP::Exception& e )
    {

        if(test) qDebug()<<"encryptByteArray"<<QString::fromStdString(e.what());
        settings.setlog("CRYPTO_DEBUG "+QString::fromStdString(e.what()));
    }
    return ciphertext;
}

string Crypto::decrypt(string password)
{
    std::string decryptedtext;
    std::string ciphertext = password;
    try
    {

        byte key[AES::DEFAULT_KEYLENGTH] = { 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1 };
        byte iv[AES::BLOCKSIZE];
        for (int i = 0; i < AES::BLOCKSIZE; i++) {
            iv[i] = 0;
        }
        CBC_Mode<AES>::Decryption Decryptor(key, sizeof(key), iv);
        CryptoPP::StringSource(ciphertext, true,
                               new CryptoPP::Base64Decoder(
                                   new CryptoPP::StreamTransformationFilter(Decryptor,
                                                                            new CryptoPP::StringSink(decryptedtext))));
    }
    catch( const CryptoPP::Exception& e )
    {

        if(test)qDebug()<<"decrypt"<<QString::fromStdString(e.what());
        settings.setlog("CRYPTO_DEBUG "+QString::fromStdString(e.what()));
    }
    return decryptedtext;
}
