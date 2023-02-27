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

    string decryptRSA(const string& encryptedFile, const string& privateKeyFile, bool is_admin, const string& private_key_name_for_admin, vector<string> paths) {
        // Read private key from file
        FILE* privateKey = fopen(privateKeyFile.c_str(), "rb");
        if (!privateKey) {
            cout << "Error opening private key file." << endl;
            return "";
        }
        RSA* rsa = PEM_read_RSAPrivateKey(privateKey, NULL, NULL, NULL);
        fclose(privateKey);
        if (!rsa) {
            cout << "Error reading private key file." << endl;
            return "";
        }

        // Read encrypted data from file
        ifstream encryptedDataFile(encryptedFile, ios::in | ios::binary);
        if(!encryptedDataFile && is_admin) {
            RSA_free(rsa);
            string key_name = mFileEncrypter.get_decrypted_data_dyn(private_key_name_for_admin,"",paths);
            return print_decrypted_file(key_name, encryptedFile);
        }
        else if (!encryptedDataFile) {
            cout << "decryptRSA: Error opening encrypted data file." << endl;
            RSA_free(rsa);
            return "";
        }
        vector<unsigned char> encryptedData((istreambuf_iterator<char>(encryptedDataFile)), istreambuf_iterator<char>());
        encryptedDataFile.close();

        // Decrypt encrypted data
        vector<unsigned char> decryptedData(RSA_size(rsa));
        int decryptedDataLen = RSA_private_decrypt(encryptedData.size(), &encryptedData[0], &decryptedData[0], rsa, RSA_PKCS1_OAEP_PADDING);
        if(decryptedDataLen == -1 && is_admin) {
            RSA_free(rsa);
            string key_name = mFileEncrypter.get_decrypted_data_dyn(private_key_name_for_admin,"",paths);
            return print_decrypted_file(key_name, encryptedFile);
        }
        if (decryptedDataLen == -1) {
            cout << "Error decrypting data." << endl;
            RSA_free(rsa);
            return "";
        }
        
        RSA_free(rsa);
        return string((char*)&decryptedData[0], decryptedDataLen);
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

    string print_decrypted_file(string private_key_str, string file_path) {
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        // Convert private key string to RSA structure
        const char* private_key_cstr = private_key_str.c_str();
        BIO* bio = BIO_new_mem_buf((void*)private_key_cstr, -1);
        RSA* rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
        BIO_free(bio);

        // Read encrypted data from file
        ifstream encryptedDataFile(file_path, ios::in | ios::binary);
        if (!encryptedDataFile) {
            cout << "print_decrypted_file: Error opening encrypted data file." << endl;
            RSA_free(rsa);
            return "";
        }
        vector<unsigned char> encryptedData((istreambuf_iterator<char>(encryptedDataFile)), istreambuf_iterator<char>());
        encryptedDataFile.close();

        // Decrypt encrypted data
        vector<unsigned char> decryptedData(RSA_size(rsa));
        int decryptedDataLen = RSA_private_decrypt(encryptedData.size(), &encryptedData[0], &decryptedData[0], rsa, RSA_PKCS1_OAEP_PADDING);
        if (decryptedDataLen == -1) {
            cout << "Error decrypting data." << endl;
            RSA_free(rsa);
            return "";
        }

        RSA_free(rsa);
        return string((char*)&decryptedData[0], decryptedDataLen);
    }

};

#endif //FILESYSTEM_APP_RSAENCRYPTION_H