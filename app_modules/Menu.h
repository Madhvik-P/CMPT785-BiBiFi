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

};

#endif //FILESYSTEM_APP_MENU_H
