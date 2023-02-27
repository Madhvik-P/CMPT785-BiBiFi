#ifndef FILESYSTEM_APP_FILEENCRYPTOR_H
#define FILESYSTEM_APP_FILEENCRYPTOR_H
#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <vector>
#include "KeyEncrypter.h"

using namespace std;

class FileEncrypter {

private:
    static unsigned char key[16];
    KeyEncrypter mKeyEncrypter;

public:
    // 
    static void encrypt_file(string file_name, string contents) {
        AES_KEY aes_key;
        AES_set_encrypt_key(key, 128, &aes_key);

        vector<unsigned char> in_buf(contents.begin(), contents.end());
        int padding = 16 - (in_buf.size() % 16);
        for (int i = 0; i < padding; i++) {
            in_buf.push_back('\0');
        }
        vector<unsigned char> out_buf;
        for (int i = 0; i < in_buf.size(); i += 16) {
            unsigned char block_in[16], block_out[16];
            for (int j = 0; j < 16; j++) {
                block_in[j] = in_buf[i + j];
            }
            AES_encrypt(block_in, block_out, &aes_key);
            for (int j = 0; j < 16; j++) {
                out_buf.push_back(block_out[j]);
            }
        }
        ofstream out_file(file_name, ios::out | ios::binary);
        if (!out_file.is_open()) {
            cout << "Error: Unable to create file " << file_name << endl;
            return;
        }
        out_file.write((char*)out_buf.data(), out_buf.size());
        out_file.close();
    }

    // 
    static vector<unsigned char> decrypt_file(string file_name, string actual_filename = "") {
        AES_KEY aes_key;
        AES_set_decrypt_key(key, 128, &aes_key);
        vector<unsigned char> out_buf;
        ifstream in_file(file_name, ios::in | ios::binary);
        if (!in_file.is_open()) {
            cout << actual_filename << " doesn't exist." << endl;
            return out_buf;
        }
        vector<unsigned char> in_buf(istreambuf_iterator<char>(in_file), {});
        in_file.close();
        for (int i = 0; i < in_buf.size(); i += 16) {
            unsigned char block_in[16], block_out[16];
            for (int j = 0; j < 16; j++) {
                block_in[j] = in_buf[i + j];
            }
            AES_decrypt(block_in, block_out, &aes_key);
            for (int j = 0; j < 16; j++) {
                out_buf.push_back(block_out[j]);
            }
        }
        int padding = 0;
        for (int i = out_buf.size() - 1; i >= 0; i--) {
            if (out_buf[i] == '\0') {
                padding++;
            } else {
                break;
            }
        }
        out_buf.resize(out_buf.size() - padding);
        return out_buf;
    }
    // 
    static void show_decrypted_data(string file_name, string actual_filename) {
        vector<unsigned char> out_buf = decrypt_file(file_name, actual_filename);
        cout.write((char*)out_buf.data(), out_buf.size());
        cout << endl;
    }
    // 
    static string get_decrypted_data(string file_name) {
        vector<unsigned char> out_buf = decrypt_file(file_name);
        return string((char*)out_buf.data(), out_buf.size());
    }

    void show_decrypted_data_dyn(string file_name, string actual_filename, vector<string> paths) {
        vector<unsigned char> out_buf = decrypt_file_dyn(file_name, actual_filename, paths);
        cout.write((char*)out_buf.data(), out_buf.size());
        cout << endl;
    }

    string get_decrypted_data_dyn(string file_name, string actual_filename, vector<string> paths) {
        vector<unsigned char> out_buf = decrypt_file_dyn(file_name, actual_filename, paths);
        return string((char*)out_buf.data(), out_buf.size());
    }

    void encrypt_file_dyn(string file_name, string contents, vector<string> paths, string username) {
        AES_KEY aes_key;
        AES_set_encrypt_key(mKeyEncrypter.decryptAESKey(paths,ADMIN), 128, &aes_key);

        vector<unsigned char> in_buf(contents.begin(), contents.end());
        int padding = 16 - (in_buf.size() % 16);
        for (int i = 0; i < padding; i++) {
            in_buf.push_back('\0');
        }
        vector<unsigned char> out_buf;
        for (int i = 0; i < in_buf.size(); i += 16) {
            unsigned char block_in[16], block_out[16];
            for (int j = 0; j < 16; j++) {
                block_in[j] = in_buf[i + j];
            }
            AES_encrypt(block_in, block_out, &aes_key);
            for (int j = 0; j < 16; j++) {
                out_buf.push_back(block_out[j]);
            }
        }
        ofstream out_file(file_name, ios::out | ios::binary);
        if (!out_file.is_open()) {
            cout << "Error: Unable to create file " << file_name << endl;
            return;
        }
        out_file.write((char*)out_buf.data(), out_buf.size());
        out_file.close();
    }


    // 
    vector<unsigned char> decrypt_file_dyn(string file_name, string actual_filename, vector<string> paths) {
        AES_KEY aes_key;
        AES_set_decrypt_key(mKeyEncrypter.decryptAESKey(paths,ADMIN), 128, &aes_key);
        vector<unsigned char> out_buf;
        ifstream in_file(file_name, ios::in | ios::binary);
        if (!in_file.is_open()) {
            cout << actual_filename << " doesn't exist." << endl;
            return out_buf;
        }
        vector<unsigned char> in_buf(istreambuf_iterator<char>(in_file), {});
        in_file.close();
        for (int i = 0; i < in_buf.size(); i += 16) {
            unsigned char block_in[16], block_out[16];
            for (int j = 0; j < 16; j++) {
                block_in[j] = in_buf[i + j];
            }
            AES_decrypt(block_in, block_out, &aes_key);
            for (int j = 0; j < 16; j++) {
                out_buf.push_back(block_out[j]);
            }
        }
        int padding = 0;
        for (int i = out_buf.size() - 1; i >= 0; i--) {
            if (out_buf[i] == '\0') {
                padding++;
            } else {
                break;
            }
        }
        out_buf.resize(out_buf.size() - padding);
        return out_buf;
    }
};

unsigned char FileEncrypter::key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
#endif //FILESYSTEM_APP_FILEENCRYPTOR_H