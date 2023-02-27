#include "app_modules/Auth.h"
#include "app_modules/Menu.h"
#include "app_modules/AppConstants.h"
#include <iostream>
#include <string>

using namespace AppConstants::DefaultDirectory;
using namespace std;

int main(int argc, char* argv[]) {

    if(argc != 2) {
        cout << "\nPlease provide the keyfile for authentication purposes.\n" << endl;
        return 0;
    }

    Auth auth;
    KeyEncrypter mKeyEncrypter;

    string username = auth.getUsernameFromPrivateKey(argv[1]);
    if(username.empty()) return 0;

    bool is_admin = username == ADMIN;

    int status = auth.checkFileSystemExits(is_admin);
    if(status == 404 && !is_admin) {
        cout << "\nIt appears that you are a new user on this application. To get started, you will need to create an administrator account first.\n" << endl;
        return 0;
    }

    int create_admin_status = auth.checkAndCreateAdmin(is_admin);

    unsigned char* login_status;
    if(create_admin_status == 201) {
        login_status = mKeyEncrypter.decryptAESKey(Utils::getPublicAndPrivateKeyPaths(),username);
    }

    if (create_admin_status != 201) {
        login_status = auth.login(username);
    }

    if (login_status == nullptr) return 0;

    auth.changeDirToUsernameIfNotAdmin(is_admin, username);
    Menu menu(username, is_admin);
    menu.showCMD();
    return 0;
}