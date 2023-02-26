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

    int encryptRSA(const string& plainText, const string& publicKeyFile, const string& outputEncryptedFile) {
        // Read public key from file
        FILE* publicKey = fopen(publicKeyFile.c_str(), "rb");
        if (!publicKey) {
            cout << "Error opening public key file." << endl;
            return 1;
        }
        RSA* rsa = PEM_read_RSAPublicKey(publicKey, NULL, NULL, NULL);
        fclose(publicKey);
        if (!rsa) {
            cout << "Error reading public key file." << endl;
            return 1;
        }

        // Encrypt plaintext
        int maxDataLen = RSA_size(rsa) - 42; // Maximum plaintext length that can be encrypted with RSA
        if (plainText.length() > maxDataLen) {
            cout << "Plaintext is too long to be encrypted with RSA." << endl;
            RSA_free(rsa);
            return 1;
        }
        vector<unsigned char> encryptedData(RSA_size(rsa));
        int encryptedDataLen = RSA_public_encrypt(plainText.length(), (unsigned char*)plainText.c_str(), &encryptedData[0], rsa, RSA_PKCS1_OAEP_PADDING);
        if (encryptedDataLen == -1) {
            cout << "Error encrypting plaintext." << endl;
            RSA_free(rsa);
            return 1;
        }

        // Write encrypted data to output file
        ofstream outputFile(outputEncryptedFile, ios::out | ios::binary);
        if (!outputFile) {
            cout << "Error opening output file." << endl;
            RSA_free(rsa);
            return 1;
        }
        outputFile.write((char*)&encryptedData[0], encryptedDataLen);
        outputFile.close();

        RSA_free(rsa);
        return 0;
    }

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