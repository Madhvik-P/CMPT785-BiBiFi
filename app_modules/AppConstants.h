#ifndef FILESYSTEM_APP_APPCONSTANTS_H
#define FILESYSTEM_APP_APPCONSTANTS_H

#include <string>

using namespace std;

namespace AppConstants {

    // constants of filesystem default directories
    namespace DefaultDirectory {
        const string PERSONAL = "personal";
        const string SHARED = "shared";
        const string FILE_SYSTEM = "filesystem";
        const string USERS = "users_key";
        const string ADMIN = "admin";
        const string META_FILE = "_meta_.txt";

    }

    // Declaring constants for all the commands
    namespace CMDConstants {
        
        const string EXIT = "exit";
        const string ADDUSER = "adduser";
        const string MKFILE = "mkfile";
        const string MKDIR = "mkdir";
        const string SHARE = "share";
        const string CAT = "cat";
        const string LS = "ls";
        const string CD = "cd";
        const string PWD = "pwd";
        const string CP = "cp";

        enum Command {
            CMD_EXIT,
            CMD_ADDUSER,
            CMD_MKFILE,
            CMD_MKDIR,
            CMD_SHARE,
            CMD_CAT,
            CMD_LS,
            CMD_CD,
            CMD_PWD,
            CMD_INVALID
        };

    }
}

#endif //FILESYSTEM_APP_APPCONSTANTS_H
