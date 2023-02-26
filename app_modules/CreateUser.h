#ifndef FILESYSTEM_APP_CREATEUSER_H
#define FILESYSTEM_APP_CREATEUSER_H

#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "Utils.h"
#include "RSAEncryption.h"
#include "KeyEncrypter.h"
#include "FileEncrypter.h"

using namespace AppUtils;
using namespace AppConstants::DefaultDirectory;

class CreateUser {

private:
    RSAEncryption mRSAEncryption;
    KeyEncrypter mKeyEncrypter;
    FileEncrypter mFileEncrypter;

public:


};

#endif //FILESYSTEM_APP_CREATEUSER_H