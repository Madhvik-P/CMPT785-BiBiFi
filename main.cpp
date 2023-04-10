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





//#include <iostream>
//#include <string>
//
//#include <iostream>
//#include <string>
//
//void updateFileLine(std::string& inputString, const std::string& fileName) {
//    size_t pos = inputString.find(fileName);
//    if (pos != std::string::npos) {
//        // Find the end of the line containing the file name
//        size_t endPos = inputString.find('\n', pos);
//        std::string line = inputString.substr(pos, endPos-pos);
//
//        if(line.find("SHARED") != std::string::npos){
//            line += " aa bb\n";
//            std::cout << line << std::endl;
//            // Replace the line containing the file name
//            inputString.replace(pos, endPos - pos + 1, line);
//        }else {
//            line += " SHARED\n";
//            std::cout << line << std::endl;
//            // Replace the line containing the file name
//            inputString.replace(pos, endPos - pos + 1, line);
//        }
//    }
//}
//
//int main() {
//    std::string inputString = "____user____aa F0I3T0uY7QqIpABxnjLxEyq4IH9M24\n"
//                              "aa XTmb96z37Hf9BaXVNFYLKsjKzbmSDg\n"
//                              "t1.txt Lr2Hu2Tusaahnts5RQfq3YxGUzMZzC SHARED aksjhdjk ashdask dhaskd haskd has hello all\n"
//                              "____user____bb sRzdUdISWLQ9KgGXGrpcAExRI5BKQy SHARED\n"
//                              "bb jWqCKEy5QqCuCJf1sM3GJxOHQCGE2I\n"
//                              "aa-t1.txt GUVd73kZBunCD1Gq330WFxkgNY8rUo\n";
//    std::string fileName = "t1.txt";
//
//    updateFileLine(inputString, fileName);
//    std::cout << inputString;
//
//    return 0;
//}
