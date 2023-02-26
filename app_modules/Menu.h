#ifndef FILESYSTEM_APP_MENU_H
#define FILESYSTEM_APP_MENU_H

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
#include "ShareFile.h"
#include "KeyEncrypter.h"

using namespace std;
using namespace AppUtils;
using namespace AppConstants::DefaultDirectory;


class Menu {

private:

    string username = "";
    vector<string> breadCrumb;
    string current_dir_name = "";
    FileEncrypter enc;
    CreateUser mCreateUser;
    RSAEncryption mRSAEncryption;
    ShareFile mShareFile;
    KeyEncrypter mKeyEncrypter;
    bool is_admin;
    FileEncrypter mFileEncrypter;

public:



    string getRelativePWDPath(){
        string pwd_path = implPWD();
        string root_dir = Utils::getRootDirPath(Utils::getPwdPath(),FILE_SYSTEM);
        string translated_path = Randomizer::getTranslatedPathRev(root_dir, pwd_path);
        return showPWD(translated_path);
    }

    bool checkUsernameExist(string share_username = ""){
        string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
        string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
        DIR *dir;
        struct dirent *entry;
        bool is_username_present = false;
        dir = opendir(root_path.c_str());
        if (!dir) {
            cout << "Error: Could not open directory" << endl;
        }
        while ((entry = readdir(dir)) != NULL) {
            string random_d_name = string(entry->d_name);
            string value;
            bool is_dot_symbol = random_d_name == "." || random_d_name == "..";
            if(!is_dot_symbol)
                value = Randomizer::getMetaKey(root_path, random_d_name);
            else
                value = random_d_name;

            if (entry->d_type == DT_DIR && !value.empty() && share_username == value) {
                is_username_present = true;
            }
        }
        closedir(dir);
        if(!is_username_present && !is_admin) {
            cout << "User " << share_username << " doesn't exist" << endl;
        }
        return is_username_present;
    }

    void implCAT(string file_name, string actual_filename) {
        string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
        string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
        vector<string> paths = Utils::getPublicAndPrivateKeysPath(root_path);
        string private_key_path = paths[1] + "/" + username + PRIVATE_KEY_EXT;


        // for admin
        string target_username;
        string private_key_name_for_admin;
        if(is_admin) {
            string _username = Utils::getUsernameFromFilePath(Utils::getPwdPath());
            target_username = Randomizer::getMetaKey(root_path,_username);
            private_key_name_for_admin = paths[0] + "/" +
                                                Randomizer::getMetaValue(root_path, PRIVATE) + "/" +
                                                target_username + PRIVATE_KEY_EXT;
        }
        mRSAEncryption.printDecryptedContent(file_name,  private_key_path, is_admin, private_key_name_for_admin, paths);
    }

    void displayMenu() {
        cout << "\n";
        cout << "Welcome: " + username << endl;
        cout << "\n";
        cout << "Menu:" << endl;
        cout << "Please select the following options: " << endl;
        cout << "\n";
        cout << "1. cd <directory>" << endl;
        cout << "2. pwd" << endl;
        cout << "3. ls" << endl;
        cout << "4. cat <filename>" << endl;
        cout << "5. share <filename> <username>" << endl;
        cout << "6. mkdir <directory_name>" << endl;
        cout << "7. mkfile <filename> <contents>" << endl;
        cout << "8. help" << endl;
        cout << "9. exit" << endl;
        // for admin
        if (is_admin) {
            cout << "10. adduser <username>" << endl;
        }
        cout << "\n";
    }

    void implLS() {
        string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
        string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
        DIR *dir;
        struct dirent *entry;
        dir = opendir(".");
        if (!dir) {
            cout << "Error: Could not open directory" << endl;
        }
        while ((entry = readdir(dir)) != NULL) {
            string random_d_name = string(entry->d_name);
            string value;

            bool is_dot_symbol = random_d_name == "." || random_d_name == "..";
            if(!is_dot_symbol)
                value = Randomizer::getMetaKey(root_path, random_d_name);
            else
                value = random_d_name;

            if(value == USERS) continue;

            if (entry->d_type == DT_REG && !value.empty()) {
                cout << "f -> " << value << endl;
            } else if (entry->d_type == DT_DIR && !value.empty()) {
                cout << "d -> " << value << endl;
            }
        }
        closedir(dir);
    }


};

#endif //FILESYSTEM_APP_MENU_H
