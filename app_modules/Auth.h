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
    // 
    int checkAndCreateAdmin(bool is_admin) {
        int status_code = checkAndCreateDirectory();
        if(status_code == 201) {
            createAdminUser();
        }
        if (status_code == 201 || status_code == 200) chdir(FILE_SYSTEM.c_str());
        return status_code;
    }
    // 
    int checkFileSystemExits(bool is_admin) {
        int p_status = stat(FILE_SYSTEM.c_str(), &info);
        if (p_status == 0 && S_ISDIR(info.st_mode)) return 200;
        else return 404;
    }
    // 
    int checkAndCreateDirectory() {
        int p_status = stat(FILE_SYSTEM.c_str(), &info);
        if (p_status == 0 && S_ISDIR(info.st_mode)) return 200;

        string personal_random_dir = Randomizer::generateRandomString();
        string shared_random_dir = Randomizer::generateRandomString();
        string users_key_random_dir = Randomizer::generateRandomString();
        string admin = Randomizer::generateRandomString();
        string private_k = Randomizer::generateRandomString();

        map<string, string> folder_key_values = {
                {PERSONAL, personal_random_dir},
                {SHARED, shared_random_dir},
                {USERS, users_key_random_dir},
                {ADMIN, admin},
                {PRIVATE ,private_k}
        };

        string child_folder_local = FILE_SYSTEM + "/" + personal_random_dir;
        string child_folder_shared = FILE_SYSTEM + "/" + shared_random_dir;
        string child_folder_users = FILE_SYSTEM + "/" + users_key_random_dir;
        string child_folder_users_private = child_folder_users + "/" + private_k;

        int s1 = mkdir(FILE_SYSTEM.c_str(), 0777);
        int s2 = mkdir(child_folder_local.c_str(), 0777);
        int s3 = mkdir(child_folder_shared.c_str(), 0777);
        int s4 = mkdir(child_folder_users.c_str(), 0777);
        int s5 = mkdir(child_folder_users_private.c_str(), 0777);

        if (s1 == 0 && s2 == 0 && s3 == 0 && s4 == 0 && s5 == 0) {
            string root_path = Utils::getRootDirPath(Utils::getPwdPath(),FILE_SYSTEM) + "/" + FILE_SYSTEM;
            mRandom.createMetaDataFile(root_path, folder_key_values);
            map<string, string> kp = mRandom.getDataFromMetaFile(root_path);
            return 201;
        }
        else return 500;
    }

    // 
    void createAdminUser() {
        string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
        string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
        vector<string> paths = Utils::getPublicAndPrivateKeysPath(root_path);
        mCreateUser.generateKeysForUser(ADMIN, paths);
        mKeyEncrypter.generateAndStoreAESKey(ADMIN, paths);
    }

    // 
    string getUsernameFromPrivateKey(string key) {
        vector<string> key_paths = Utils::split(key, '_');
        if(key_paths.empty()) {
            cout << "\nWrong key format" << endl;
            return "";
        }
        string username = key_paths[0];
        return username;
    }
    // 
    unsigned char* login(string username) {
        string pwd = Utils::getPwdPath();
        string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
        string users_path = root_path + "/" + Randomizer::getMetaValue(root_path, USERS);
        vector<string> all_files = Utils::getFilesInsideDir(users_path);
        string public_key_name = Utils::findPublicKey(all_files, username);

        vector<string> paths = Utils::getPublicAndPrivateKeysPath(root_path);
        unsigned char* key = mKeyEncrypter.decryptAESKey(paths, username);
        return key;
    }
    
    void changeDirToUsernameIfNotAdmin(bool is_admin, string username){
        if(!is_admin) {
            string root_path = Utils::getRootDirPath(Utils::getPwdPath(), FILE_SYSTEM);
            string randomised_username = Randomizer::getMetaValue(root_path, username);
            chdir(randomised_username.c_str());
        }
    }

};

#endif //FILESYSTEM_APP_AUTH_H
