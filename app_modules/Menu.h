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
    Menu(string u, bool a) {
        username = u;
        is_admin = a;
    };
    int showCMD() {
        // TODO: need to get the value from the main function
        displayMenu(is_admin);

    while (true) {
        string input;
        implPWD();

        if(current_dir_name == FILE_SYSTEM) cout << "/ $ ";
        else cout << current_dir_name + " $ ";

        getline(cin, input);

        vector<string> tokens = Utils::split(input, ' ');

        Command selectedCommand = Utils::getCommandName(tokens, input);

        if(selectedCommand == CMD_EXIT) {
            break;
        }

        switch (selectedCommand) {
            case CMD_ADDUSER: {
                if (is_admin) {
                    string username = tokens[1];
                    string root_path = Utils::getRootDirPath(implPWD(), FILE_SYSTEM);
                    vector<string> paths = Utils::getPublicAndPrivateKeysPath(root_path);
                    int status = createUser(root_path, username);
                    if(status == 201) mCreateUser.generateKeysForUser(username, paths);
                }else {
                    cout << "You are not admin user. Please contact admit to create new user." << endl;
                }
                break;
            }

            case CMD_MKFILE: {
                implMKFILE(tokens[1], Utils::joinVectorToString(tokens, 2, " "));
                break;
            }

            case CMD_MKDIR: {
                string filename_value = translateDirOrFileWhenCreated(tokens[1]);
                bool isPresent = Utils::checkDirectoryExists(filename_value);
                if (!isPresent) mkdir(filename_value.c_str(), 0777);
                else cout << "Directory already exists" << endl;
                break;
            }

            case CMD_SHARE: {

                bool isPresent = checkFilePresentInCurrentDir(tokens[1]) && checkUsernameExist(tokens[2]);
                if(isPresent) {
                    string root_path = Utils::getRootDirPath(implPWD(), FILE_SYSTEM);
                    string  filename = Randomizer::getMetaValue(root_path,tokens[1]);
                    string share_username = Randomizer::getMetaValue(root_path,tokens[2]);
                    implShare(filename, share_username);
                }
                break;
            }

            case CMD_CAT: {
                string root_path = Utils::getRootDirPath(implPWD(), FILE_SYSTEM);
                string translated_filename = Randomizer::getMetaValue(root_path,tokens[1]);
                implCAT(translated_filename, tokens[1]);
                break;
            }

            case CMD_LS: {
                implLS();
                break;
            }

            case CMD_CD: {
                implCD(tokens[1]);
                break;
            }

            case CMD_PWD: {
                string pwd_path = implPWD();
                string root_dir = Utils::getRootDirPath(Utils::getPwdPath(),FILE_SYSTEM);
                string translated_path = Randomizer::getTranslatedPathRev(root_dir, pwd_path);
                showPWD(translated_path);
                break;
            }

            default:
                break; 
            }
        }
        return 0;

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
