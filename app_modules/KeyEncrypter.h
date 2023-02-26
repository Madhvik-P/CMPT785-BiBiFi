#ifndef FILESYSTEM_APP_KEYENCRYPTER_H
#define FILESYSTEM_APP_KEYENCRYPTER_H

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "AppConstants.h"
#include "FileEncrypter.h"

using namespace std;
using namespace AppConstants::DefaultDirectory;

class KeyEncrypter {

public: 

    int generateAndStoreAESKey(string username, vector<string> paths){

        // Generate AES key
        const int AES_KEY_SIZE = 256;
        unsigned char aes_key[AES_KEY_SIZE / 8];
        if (!RAND_bytes(aes_key, AES_KEY_SIZE / 8)) {
            cout << "Error generating AES key" << endl;
            return 1;
        }

        // Load public key
        RSA *public_key = nullptr;
        string public_key_name = paths[0] + "/" + username+ PUBLIC_KEY_EXT;
        char* public_key_file = new char[public_key_name.length() + 1];
        strcpy(public_key_file, public_key_name.c_str());
        FILE *public_key_fp = fopen(public_key_file, "r");

        



    }




};

#endif //FILESYSTEM_APP_KEYENCRYPTER_H