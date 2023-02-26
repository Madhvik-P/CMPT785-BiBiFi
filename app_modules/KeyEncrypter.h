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

        if (!public_key_fp) {
            cout << "Error opening public key file" << endl;
            return 1;
        }

        public_key = PEM_read_RSAPublicKey(public_key_fp, nullptr, nullptr, nullptr);
        fclose(public_key_fp);

        if (!public_key) {
            cout << "Error loading public key" << endl;
            return 1;
        }

        // Encrypt AES key
        const int RSA_ENCRYPTED_SIZE = RSA_size(public_key);
        unsigned char encrypted_key[RSA_ENCRYPTED_SIZE];
        int encrypted_key_size = RSA_public_encrypt(AES_KEY_SIZE / 8, aes_key, encrypted_key, public_key, RSA_PKCS1_OAEP_PADDING);
        if (encrypted_key_size == -1) {
            cout << "Error encrypting AES key" << endl;
            RSA_free(public_key);
            return 1;
        }

        // Write encrypted key to file
        string bin_file_name = paths[0] + "/" + username+ AES_KEY_EXT;

        char* encrypted_key_file = new char[bin_file_name.length() + 1];
        strcpy(encrypted_key_file, bin_file_name.c_str());
        ofstream encrypted_key_stream(encrypted_key_file, ios::out | ios::binary);

        if (!encrypted_key_stream.is_open()) {
            cout << "Error opening encrypted key file" << endl;
            RSA_free(public_key);
            return 1;
        }

        encrypted_key_stream.write((const char*) encrypted_key, encrypted_key_size);
        encrypted_key_stream.close();
        RSA_free(public_key);
        return 0;

    }

    unsigned char* decryptAESKey(vector<string> paths, string username) {

        // Load private key
        RSA *private_key = nullptr;
        string private_key_path = paths[1] + "/" + username + PRIVATE_KEY_EXT;
        char* private_key_file = new char[private_key_path.length() + 1];
        strcpy(private_key_file, private_key_path.c_str());
        FILE *private_key_fp = fopen(private_key_file, "r");

        if (!private_key_fp) {
            cout << "Error opening private key file" << endl;
            return nullptr;
        }

        private_key = PEM_read_RSAPrivateKey(private_key_fp, nullptr, nullptr, nullptr);
        fclose(private_key_fp);

        if (!private_key) {
            cout << "\nInvalid keyfile\n" << endl;
            return nullptr;
        }

        // Load encrypted key from file
        string aes_private_key_path = paths[0] + "/" + username + AES_KEY_EXT;

        char* encrypted_key_file = new char[aes_private_key_path.length() + 1];
        strcpy(encrypted_key_file, aes_private_key_path.c_str());
        ifstream encrypted_key_stream(encrypted_key_file, ios::in | ios::binary);
        if (!encrypted_key_stream.is_open()) {
            cout << "\nInvalid keyfile\n" << endl;
            RSA_free(private_key);
            return nullptr;
        }

        const int RSA_ENCRYPTED_SIZE = RSA_size(private_key);
        unsigned char encrypted_key[RSA_ENCRYPTED_SIZE];
        encrypted_key_stream.read((char*) encrypted_key, RSA_ENCRYPTED_SIZE);
        encrypted_key_stream.close();

        // Decrypt AES key
        const int AES_KEY_SIZE = 256;
        unsigned char* aes_key = new unsigned char[AES_KEY_SIZE / 8];
        int decrypted_key_size = RSA_private_decrypt(RSA_ENCRYPTED_SIZE, encrypted_key, aes_key, private_key, RSA_PKCS1_OAEP_PADDING);

        if (decrypted_key_size == -1) {

            cout << "\nInvalid keyfile\n" << endl;
            RSA_free(private_key);
            delete[] aes_key;
            return nullptr;

        }

        RSA_free(private_key);
        return aes_key;

    }

};

#endif //FILESYSTEM_APP_KEYENCRYPTER_H