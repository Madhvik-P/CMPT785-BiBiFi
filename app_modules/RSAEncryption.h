#ifndef FILESYSTEM_APP_RSAENCRYPTION_H
#define FILESYSTEM_APP_RSAENCRYPTION_H

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
#include <fstream>
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "Auth.h"
#include "Utils.h"
#include "AppConstants.h"
#include "FileEncrypter.h"
#include "CreateUser.h"
#include "Randomizer.h"

using namespace std;
using namespace AppUtils;
using namespace AppConstants::DefaultDirectory;

class RSAEncryption {

private:
    FileEncrypter mFileEncrypter;

public:

    void printDecryptedContent(const string& encryptedFile, const string& privateKeyFile, bool is_admin, const string& private_key_name_for_admin, vector<string> paths){
        string content = decryptRSA(encryptedFile, privateKeyFile, is_admin, private_key_name_for_admin, paths);
        cout.write((char*)content.data(), content.size());
        cout << endl;
    }

    string read_file(string file_path) {
        ifstream file(file_path);
        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

};

#endif //FILESYSTEM_APP_RSAENCRYPTION_H