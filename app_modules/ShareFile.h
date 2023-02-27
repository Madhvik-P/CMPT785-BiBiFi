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

    void implShare(string filename, string share_username, string actual_filename, string actual_share_username, string username) {
        string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
        string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
        string share_destination_path;
        if(actual_share_username == ADMIN) {
            share_destination_path = root_path
                                     + "/"
                                     + Randomizer::getMetaValue(root_path, SHARED);
        }else {
            share_destination_path = root_path
                                     + "/" + share_username + "/"
                                     + Randomizer::getMetaValue(root_path, SHARED);
        }

        string dest_file_name = Utils::translateDirOrFileWhenCreated(username + "-" + actual_filename);
        share_destination_path = share_destination_path + "/" + dest_file_name;

        vector<string> paths = Utils::getPublicAndPrivateKeyPaths();

        string pvt_key_path = paths[1] + "/" + username + PRIVATE_KEY_EXT;
        string pub_key_path = paths[0] + "/" + actual_share_username + PUBLIC_KEY_EXT;

        string contents = mRSAEncryption.decryptRSA(filename, pvt_key_path,false,"",paths);
        mRSAEncryption.encryptRSA(contents, pub_key_path, share_destination_path);
    }

}; 

#endif //FILESYSTEM_APP_SHAREFILE_H