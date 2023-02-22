#ifndef FILESYSTEM_APP_UTILS_H
#define FILESYSTEM_APP_UTILS_H

#include <vector>
#include <sstream>
#include <map>
#include <filesystem>
#include <dirent.h>
#include "AppConstants.h"
#include "Randomizer.h"

using namespace std;
using namespace AppConstants::CMDConstants;
using namespace AppConstants::DefaultDirectory;
using namespace filesystem;

struct stat info;

namespace AppUtils {

    class Utils {
        public:

        static Command findCommand(const map<Command, bool> &values) {

            Command result;
            int trueCount = 0;

            for (const auto &[command, value]: values) {

                if (value) {

                    trueCount++;
                    result = command;

                }
            }
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



    };
}



#endif //FILESYSTEM_APP_UTILS_H
