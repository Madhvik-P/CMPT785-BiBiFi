#ifndef FILESYSTEM_APP_SHAREFILE_H
#define FILESYSTEM_APP_SHAREFILE_H


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
#include "Auth.h"
#include "Utils.h"
#include "AppConstants.h"
#include "FileEncrypter.h"
#include "CreateUser.h"
#include "Randomizer.h"
#include "RSAEncryption.h"

using namespace std;
using namespace AppUtils;
using namespace AppConstants::DefaultDirectory;

class ShareFile{

    RSAEncryption mRSAEncryption;

public: 

}; 

#endif //FILESYSTEM_APP_SHAREFILE_H