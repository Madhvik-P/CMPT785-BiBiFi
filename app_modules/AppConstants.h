#ifndef FILESYSTEM_APP_APPCONSTANTS_H
#define FILESYSTEM_APP_APPCONSTANTS_H

#include <string>

using namespace std;

namespace AppConstants {

   
    namespace DefaultDirectory {
        const string PERSONAL = "personal";
        const string SHARED = "shared";
        const string FILE_SYSTEM = "filesystem";
        const string USERS = "users_key";
        const string ADMIN = "admin";
        const string PRIVATE = "private";
        const string META_FILE = "_meta_.txt";
        const string PUBLIC_KEY_EXT = "_public.pem";
        const string PRIVATE_KEY_EXT = "_private.pem";
        const string AES_KEY_EXT = "-encrypted_key.bin";
        const string USER = "____user____";
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
        const string HELP = "help";

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
            CMD_INVALID,
            CMD_HELP
        };
    }
}

#endif //FILESYSTEM_APP_APPCONSTANTS_H
