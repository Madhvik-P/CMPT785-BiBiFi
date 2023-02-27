#ifndef FILESYSTEM_APP_RANDOMIZER_H
#define FILESYSTEM_APP_RANDOMIZER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "Utils.h"
#include "FileEncrypter.h"

using namespace AppConstants::DefaultDirectory;
using namespace std;


class Randomizer {

public:

    static string generateRandomString(int length=30) {
        string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        string randomString = "";
        static bool initialized = false;
        if (!initialized) {
            srand(time(nullptr));
            initialized = true;
        }
        for (int i = 0; i < length; i++) {
            int randomIndex = rand() % chars.length();
            randomString += chars[randomIndex];
        }
        return randomString;
    }

    static map<string, string> getDataFromMetaFile(string root_dir) {
        string meta_file_path = root_dir + "/" + META_FILE;
        string decrypted_data = openMetaFileAndDecrypt(root_dir);
        stringstream ss(decrypted_data);
        map<string, string> key_value_pairs;
        string line;
        while (getline(ss, line)) {
            size_t pos = line.find(' ');
            if (pos != string::npos) {
                string _key = line.substr(0, pos);
                string value = line.substr(pos + 1);
                key_value_pairs[_key] = value;
            }
        }
        return key_value_pairs;
    }

    static map<string, string> getDataFromMetaFile(string root_dir) {
        string meta_file_path = root_dir + "/" + META_FILE;
        string decrypted_data = openMetaFileAndDecrypt(root_dir);
        stringstream ss(decrypted_data);
        map<string, string> key_value_pairs;
        string line;
        while (getline(ss, line)) {
            size_t pos = line.find(' ');
            if (pos != string::npos) {
                string _key = line.substr(0, pos);
                string value = line.substr(pos + 1);
                key_value_pairs[_key] = value;
            }
        }
        return key_value_pairs;
    }

    static string getMetaValue(string root_dir, string key) {
        map<string, string> key_value_pairs = getDataFromMetaFile(root_dir);
        auto it = key_value_pairs.find(key);
        if (it == key_value_pairs.end()) {
            return "";
        }
        return it->second;
    }

    static string getTranslatedPathRev(string root_dir, string input) {
        map<string, string> key_value_pairs = getDataFromMetaFile(root_dir);
        vector<string> dirs;
        istringstream iss(input);
        string dir;
        while (getline(iss, dir, '/')) {
            dirs.push_back(dir);
        }
        // Traverse the directories and replace the value with its key
        for (int i = 0; i < dirs.size(); i++) {
            for (auto const& [key, value] : key_value_pairs) {
                if (dirs[i] == value) {
                    dirs[i] = key;
                    break;
                }
            }
        }
        string output;
        for (int i = 0; i < dirs.size(); i++) {
            output += dirs[i];
            if (i != dirs.size() - 1) {
                output += "/";
            }
        }
        return output;
    }

    static string getTranslatedPath(string root_dir, string input) {
        map<string, string> key_value_pairs = getDataFromMetaFile(root_dir);
        // Split input into tokens
        vector<string> tokens;
        istringstream iss(input);
        string token;
        while (getline(iss, token, '/')) {
            tokens.push_back(token);
        }

        // Process tokens to replace directory names with corresponding key values
        for (int i = 0; i < tokens.size(); i++) {
            if (key_value_pairs.count(tokens[i]) > 0) {
                tokens[i] = key_value_pairs[tokens[i]];
            }
        }

        // Join tokens back together with slashes
        string output = "";
        for (int i = 0; i < tokens.size(); i++) {
            if (i > 0) {
                output += "/";
            }
            output += tokens[i];
        }
        return output;
    }

};

#endif //FILESYSTEM_APP_RANDOMIZER_H