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



        

    };
}



#endif //FILESYSTEM_APP_UTILS_H
