#ifndef FILESYSTEM_APP_AUTH_H
#define FILESYSTEM_APP_AUTH_H
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem>
#include <termios.h>
#include <unistd.h>
#include<cstdlib>
#include<cstring>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include "Utils.h"
#include "AppConstants.h"
#include "CreateUser.h"
#include "Randomizer.h"
#include "KeyEncrypter.h"

using namespace AppUtils;
using namespace AppConstants::DefaultDirectory;
using namespace std;

class Auth {

private:
    CreateUser mCreateUser;
    Randomizer mRandom;
    KeyEncrypter mKeyEncrypter;

public:

};

#endif //FILESYSTEM_APP_AUTH_H
