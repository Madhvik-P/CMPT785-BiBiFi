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

using namespace std;
using namespace AppUtils;
using namespace AppConstants::DefaultDirectory;


class Menu {

private:
    string username = "";
    vector<string> breadCrumb;
    string current_dir_name = "";
    FileEncryptor enc;
    CreateUser mCreateUser;
    bool is_admin;

public:

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

        if(!is_username_present) {
            cout << "User " << share_username << " doesn't exist" << endl;
        }

        return is_username_present;
    }

    void implCAT(string file_name, string actual_filename) {
        
        FileEncryptor::show_decrypted_data(file_name, actual_filename);
    }

    void displayMenu(bool is_admin) {

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
        cout << "8. exit" << endl;
        // for admin
        if (is_admin) {
            cout << "9. adduser <username>" << endl;
        }

        cout << "\n";

    }



};

#endif //FILESYSTEM_APP_MENU_H
