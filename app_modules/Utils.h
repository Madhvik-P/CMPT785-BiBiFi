#ifndef FILESYSTEM_APP_UTILS_H
#define FILESYSTEM_APP_UTILS_H

#include <vector>
#include <sstream>
#include <map>
#include <filesystem>
#include <dirent.h>
#include "AppConstants.h"
#include "Randomizer.h"
#include <sys/stat.h>

using namespace std;
using namespace AppConstants::CMDConstants;
using namespace AppConstants::DefaultDirectory;
using namespace filesystem;

struct stat info;

namespace AppUtils {

    class Utils {
        public:


        static Command getCommandName(vector<string> tokens, string input) {
            bool is_size_more_than_one = tokens.size() > 1;
            bool is_size_two = tokens.size() == 2;
            bool is_size_three = tokens.size() == 3;
            map<Command, bool> values = {
                    {CMD_EXIT,    !tokens.empty() && tokens[0] == EXIT},
                    {CMD_ADDUSER, is_size_two && tokens[0] == ADDUSER},
                    {CMD_MKFILE,  is_size_more_than_one && tokens[0] == MKFILE},
                    {CMD_MKDIR,   is_size_two && tokens[0] == MKDIR},
                    {CMD_SHARE,   is_size_three && tokens[0] == SHARE},
                    {CMD_CAT,     is_size_two && tokens[0] == CAT},
                    {CMD_LS,      (is_size_two && tokens[0] == LS) || input == LS},
                    {CMD_CD,      is_size_two && tokens[0] == CD},
                    {CMD_PWD,     input == PWD},
                    {CMD_HELP,     input == HELP}
            };
            return findCommand(values);
        };

        static vector<string> split(string s, char delimiter) {
            vector<string> words;
            istringstream iss(s);
            string word;

            while (getline(iss, word, delimiter)) {
                words.push_back(word);
            }

            return words;
        };
        // 
        static bool checkIfPathContainsRootDir(vector<string> inputArray, bool is_admin, string username) {
            bool containsFilesystem = false;

            for (int i = 0; i < inputArray.size(); i++) {
                if (is_admin && inputArray[i] == FILE_SYSTEM) {
                    containsFilesystem = true;
                    break;
                }else if (!is_admin && inputArray[i] == username) {
                    containsFilesystem = true;
                    break;
                }
            }
            return containsFilesystem;
        }

        static vector<string> getPublicAndPrivateKeyPaths() {
            string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
            string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
            vector<string> paths = Utils::getPublicAndPrivateKeysPath(root_path);
            return paths;
        }

        static string translateDirOrFileWhenCreated(string name){
            string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
            string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);

            string filename_value = Randomizer::getMetaValue(root_path,name);

            if(filename_value.empty()){
                map<string, string> folder_key_values = {{
                        name, Randomizer::generateRandomString()
                }};
                Randomizer::createMetaDataFile(root_path,folder_key_values);
                filename_value = Randomizer::getMetaValue(root_path, name);
            }
            return filename_value;
        }

        static bool isFileType(string input) {
            struct stat fileInfo;
            if (stat(input.c_str(), &fileInfo) < 0) {
                cout << "Error: Unable to stat file/directory." << endl;
                return false;
            }
            if (S_ISREG(fileInfo.st_mode)) {
                return true;
            }
            cout << "\nThe name entered is not a file.\n";
            return false;
        }


        static Command findCommand(const map<Command, bool> &values) {

            Command result;
            int trueCount = 0;
            for (const auto &[command, value]: values) {
                if (value) {
                    trueCount++;
                    result = command;
                }
            }
            return (trueCount == 1) ? result : CMD_INVALID;
        }

        static string joinVectorToString(vector<string> tokens, int fromIndex = 0, string separator = " ") {

            ostringstream result;
            for (vector<string>::iterator it = tokens.begin() + fromIndex; it != tokens.end(); it++) {
                if (it != tokens.begin() + fromIndex) {
                    result << separator;
                }
                result << *it;
            }
            return result.str();
        };

        static string trimAfterRootDir(string dir_name, string path) {
            vector<string> parts = Utils::split(path, '/');
            auto it = find(parts.begin(), parts.end(), dir_name);
            if (it != parts.end()) {
                parts.erase(it + 1, parts.end());
            }
            string final_path = Utils::joinVectorToString(parts, 0, "/");
            return final_path;
        }

        static string getRootDirPath(string pwd_path, string dir_name){

            string root_dir_path = Utils::trimAfterRootDir(dir_name,pwd_path);
            return  root_dir_path;

        }

        static vector<string> getPublicAndPrivateKeysPath(string root_path){

            vector<string> paths;

            vector<string> data = split(root_path, '/');
            data.pop_back();

            string private_key_path = joinVectorToString(data, 0, "/");
            string public_key_path = root_path + "/" + Randomizer::getMetaValue(root_path, USERS);

            paths.push_back(public_key_path);
            paths.push_back(private_key_path);
            return paths;

        }

        static string getPublicUserKeys() {
            string pwd = Utils::getPwdPath() + "/" + FILE_SYSTEM;
            string root_path = Utils::getRootDirPath(pwd, FILE_SYSTEM);
            vector<string> paths = Utils::getPublicAndPrivateKeysPath(root_path);
            return paths[0];
        }

        static string getUsernameFromFilePath(string path) {
            string username = "";
            size_t pos = path.find("/"+FILE_SYSTEM+"/");
            if (pos != string::npos) {
                username = path.substr(pos + 12, path.find("/", pos + 12) - pos - 12);
            }
            return username;
        }

        static bool checkDirectoryExists(string dir_name) {
            int p_status = stat(dir_name.c_str(), &info);
            if (p_status == 0 && S_ISDIR(info.st_mode)) return true;
            else return false;
        };

        static int findItemIndexFromVector(vector<string> path, string item_name) {
            auto it = find(path.begin(), path.end(), item_name);
            if (it != path.end()) return it - path.begin();
            else return -1;
        }

        static string getPwdPath() {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                cout << "Error: Could not get current directory" << endl;
            }
            return cwd;
        }

        static vector<string> getFilesInsideDir(string path) {
            vector<string> files;
            DIR *dir;
            struct dirent *entry;
            dir = opendir(path.c_str());
            if (!dir) {
                cout << "Error: Could not open directory" << endl;
            }
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_REG) {
                    files.push_back(entry->d_name);
                }
            }
            closedir(dir);
            return files;
        }

        static string findPublicKey(vector<string> all_files, string username) {
            string public_key = "";
            for (const string& k : all_files) {
                vector<string> kp = Utils::split(k, '_');
                if (kp[0] == username) {
                    public_key = k;
                    break;
                }
            }
            return public_key;
        }

        static bool startsWithPersonal(const string& str) {
            // Check if the string starts with "/personal"
            bool isPersonalFound = str.compare(0, 9, "/"+PERSONAL) == 0;
            if(!isPersonalFound) {
                cout << "\nForbidden\n" << endl;
            }
            return isPersonalFound;
        }

    };
}

#endif //FILESYSTEM_APP_UTILS_H
