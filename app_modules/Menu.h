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
    // 
    Menu(string u, bool a) {
        username = u;
        is_admin = a;
    };
    // 
    int showCMD() {
        // TODO: need to get the value from the main function
        displayMenu();

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
                        string username_to_be_added = tokens[1];
                        string root_path = Utils::getRootDirPath(implPWD(), FILE_SYSTEM);
                        vector<string> paths = Utils::getPublicAndPrivateKeysPath(root_path);
                        int status = createUser(root_path, username_to_be_added);
                        if(status == 201) {
                            mCreateUser.generateKeysForUser(username_to_be_added, paths, username);
                            mKeyEncrypter.generateAndStoreAESKey(username_to_be_added, paths);
                        }
                    }else {
                        cout << "You are not admin user. Please contact admit to create new user." << endl;
                    }
                    break;
                }

                case CMD_MKFILE: {
                    if(Utils::startsWithPersonal(getRelativePWDPath())) {
                        string root_path = Utils::getRootDirPath(implPWD(), FILE_SYSTEM);
                        string _username = Utils::getUsernameFromFilePath(Utils::getPwdPath());
                        string target_username = Randomizer::getMetaKey(root_path,_username);
                        bool is_admin_inside_users_dir = Randomizer::getMetaValue(root_path, USER + target_username).empty() && is_admin;
//                    if(is_admin_inside_users_dir){
//                        cout << "\nAs an administrator, you are only authorized to access and read files on a system or server. You do not have the permission to modify or create files on the system.\n" << endl;
//                        break;
//                    }
                        implMKFILE(tokens[1], Utils::joinVectorToString(tokens, 2, " "));
                    }
                    break;
                }

                case CMD_MKDIR: {
                    if(Utils::startsWithPersonal(getRelativePWDPath())) {
                        string root_path = Utils::getRootDirPath(implPWD(), FILE_SYSTEM);
                        string _username = Utils::getUsernameFromFilePath(Utils::getPwdPath());

                        string filename_value = Utils::translateDirOrFileWhenCreated(tokens[1]);
                        bool isPresent = Utils::checkDirectoryExists(filename_value);
                        if (!isPresent) mkdir(filename_value.c_str(), 0777);
                        else cout << "Directory already exists" << endl;
                    }
                    break;
                }

                case CMD_SHARE: {
                    if (username == tokens[2]) {
                        cout << "The operation cannot be performed as you are attempting to share a file with your own account." << endl;
                        break;
                    }

                    bool isReceiverAdmin = ADMIN == tokens[2];
                    bool isFilePresent = checkFilePresentInCurrentDir(tokens[1]);
                    if(!isFilePresent) break;
                    bool isUser = isReceiverAdmin || checkUsernameExist(tokens[2]);
                    bool isPresent = isFilePresent && isUser;

                    if(isPresent) {
                        string root_path = Utils::getRootDirPath(implPWD(), FILE_SYSTEM);
                        string  filename = Randomizer::getMetaValue(root_path,tokens[1]);
                        string share_username = Randomizer::getMetaValue(root_path,tokens[2]);
                        mShareFile.implShare(filename, share_username, tokens[1], tokens[2], username);
                    }
                    break;
                }

                case CMD_CAT: {
                    string root_path = Utils::getRootDirPath(implPWD(), FILE_SYSTEM);
                    string translated_filename = Randomizer::getMetaValue(root_path,tokens[1]);
                    if(Utils::isFileType(translated_filename)){
                        implCAT(translated_filename, tokens[1]);
                    }
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
                    cout << getRelativePWDPath() << endl;
                    break;
                }

                case CMD_HELP: {
                    displayMenu();
                    break;
                }

                default:
                    cout << "\nInvalid Command\n" << endl;
                    break;
            }
        }
        return 0;

    }

    string getRelativePWDPath(){
        string pwd_path = implPWD();
        string root_dir = Utils::getRootDirPath(Utils::getPwdPath(),FILE_SYSTEM);
        string translated_path = Randomizer::getTranslatedPathRev(root_dir, pwd_path);
        return showPWD(translated_path);
    }
    // 
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
    // 
    bool checkFilePresentInCurrentDir(string filename = "", string share_username = "") {
        string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
        string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
        DIR *dir;
        struct dirent *entry;
        bool is_file_present = false;
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

            if (entry->d_type == DT_REG && !value.empty() && filename == value) {
                is_file_present = true;
            }
        }
        closedir(dir);

        if(!is_file_present) {
            cout << "File " << filename << " doesn't exist" << endl;
        }
        return is_file_present;
    }
    void implShare(string filename, string share_username){
        string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
        string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
        string share_destination_path = root_path + "/" + share_username + "/"
                                        + Randomizer::getMetaValue(root_path, SHARED);
        string cmd = CP + " " + filename + " " + share_destination_path;
        int exit_code = system(cmd.c_str());
        if (exit_code == 0) {
            cout << "File shared successfully." << endl;
        } else {
            cout << "Failed to share the file." << endl;
        }
    }
    string showPWD(string pwd_path){
        if(pwd_path.empty()) return"";
        vector<string> pwd_path_parts = Utils::split(pwd_path, '/');
        int index;
        if(is_admin) {
            index = Utils::findItemIndexFromVector(pwd_path_parts,FILE_SYSTEM);
        }else {
            index = Utils::findItemIndexFromVector(pwd_path_parts,username);
        }
        string abs_pwd_path = Utils::joinVectorToString(pwd_path_parts, index+1, "/");
        return "/" + abs_pwd_path;
    }
    // 
    int createUser(string root_dir_path, string username) {
        Utils::translateDirOrFileWhenCreated(USER + username);
        string randomized_name = Utils::translateDirOrFileWhenCreated(username);
        string user_folder_path = root_dir_path + "/" + randomized_name;
        int p_status = stat(user_folder_path.c_str(), &info);
        if (p_status == 0 && S_ISDIR(info.st_mode)) {
            cout << "User " << username << " already exists" << endl;
            return 200;
        }
        int _status = mkdir(user_folder_path.c_str(), 0777);
        if (_status == 0) {
            string child_folder_local = user_folder_path + "/" + Randomizer::getMetaValue(root_dir_path, PERSONAL);
            string child_folder_shared = user_folder_path + "/" + Randomizer::getMetaValue(root_dir_path, SHARED);
            mkdir(child_folder_local.c_str(), 0777);
            mkdir(child_folder_shared.c_str(), 0777);
            return 201;
        }
        else return 500;
    }
    // 
    void implMKFILE(string filename, string content) {
        string filename_value = Utils::translateDirOrFileWhenCreated(filename);
        string public_key_path = Utils::getPublicUserKeys() + "/" + username + PUBLIC_KEY_EXT;
        mRSAEncryption.encryptRSA(content, public_key_path, filename_value);
    }
    // 

    // 
    string implPWD() {
        string cwd = Utils::getPwdPath();
        vector<string> tokens = Utils::split(cwd, '/');

        string pwd = cwd + "/" + FILE_SYSTEM;
        string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);

        current_dir_name = Randomizer::getMetaKey(root_path, tokens[tokens.size() - 1]);
        return cwd;
    }
    // 
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
    // 
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
    // 
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

    // 
    void implCD(string token) {
        string translated_path;
        string root_dir = Utils::getRootDirPath(Utils::getPwdPath(),FILE_SYSTEM);
        translated_path = Randomizer::getTranslatedPath(root_dir, token);

        string f_path = getFuturePath(translated_path, token);
        vector<string> _f_path_parts = Utils::split(f_path, '/');
        string randomised_username = Randomizer::getMetaValue(root_dir, username);
        bool contains_root_dir = Utils::checkIfPathContainsRootDir(_f_path_parts, is_admin, randomised_username);
        if (f_path.empty() || !contains_root_dir) {
            return;
        }
        chdir(translated_path.c_str());
    }

    // 
    string getFuturePath(string file_path, string typed_file_path) {
        try {
            if(!filesystem::exists(file_path) || !filesystem::is_directory(file_path)) {
                cout << "Error: " << typed_file_path << " is not a valid directory." << endl;
                cout << "Please make sure that you have entered the correct directory path and that the directory exists." << endl;
                return "";
            }

            string command = "cd " + file_path + " && pwd";
            string futurePath = "";

            // execute command and capture output
            FILE* pipe = popen(command.c_str(), "r");
            if (!pipe) {
                cout << "Error executing command: " << typed_file_path << endl;
                return "";
            }

            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                futurePath += buffer;
            }

            pclose(pipe);
            // remove trailing newline
            futurePath.erase(futurePath.find_last_not_of("\n") + 1);
            return futurePath;
        } catch(const filesystem::filesystem_error& ex) {
            cout << "Error: " << ex.what() << endl;
            return "";
        }
    };
};

#endif //FILESYSTEM_APP_MENU_H
