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

    };
}

#endif //FILESYSTEM_APP_UTILS_H
