#ifndef FILESYSTEM_APP_FILEENCRYPTOR_H
#define FILESYSTEM_APP_FILEENCRYPTOR_H
#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <vector>
#include "KeyEncrypter.h"

using namespace std;

class FileEncryptor {

private:
    static unsigned char key[16];
    KeyEncrypter mKeyEncrypter;

public:


};

unsigned char FileEncryptor::key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
#endif //FILESYSTEM_APP_FILEENCRYPTOR_H