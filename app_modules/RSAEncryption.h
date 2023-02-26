#ifndef FILESYSTEM_APP_RSAENCRYPTION_H
#define FILESYSTEM_APP_RSAENCRYPTION_H

#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <termios.h>
#include <unistd.h>
#include <curses.h>
#include<cstdlib>
#include<cstring>
#include <cstdlib>
#include <dirent.h>
#include <map>
#include <cstdlib>
#include <fstream>
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "Auth.h"
#include "Utils.h"
#include "AppConstants.h"
#include "FileEncrypter.h"
#include "CreateUser.h"
#include "Randomizer.h"

using namespace std;
using namespace AppUtils;
using namespace AppConstants::DefaultDirectory;

class RSAEncryption {

private:
    FileEncrypter mFileEncrypter;

public:


};

#endif //FILESYSTEM_APP_RSAENCRYPTION_H