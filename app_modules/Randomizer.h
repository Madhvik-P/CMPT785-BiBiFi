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

    static void createMetaDataFile(string root_path, map<string, string> folder_key_values) {
        ofstream myfile;

        string decrypted_data = openMetaFileAndDecrypt(root_path);

        stringstream ss;
        ss << decrypted_data;
        for (const auto& pair : folder_key_values) {
            ss << pair.first << " " << pair.second << "\n";
        }
        string content = ss.str();

        FileEncrypter::encrypt_file(root_path + "/" + META_FILE, content);
    }

    static string openMetaFileAndDecrypt(string root_path) {
        ifstream file(root_path + "/" + META_FILE);

        if (!file.is_open()) {
            return "";
        }

        string content((istreambuf_iterator<char>(file)),(istreambuf_iterator<char>()));
        file.close();
        return FileEncrypter::get_decrypted_data(root_path + "/" + META_FILE);
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

    static void updateTheSharedStatus(string root_dir, string parent_file_name, string shared_username) {
        string meta_file_path = root_dir + "/" + META_FILE;
        string decrypted_data = openMetaFileAndDecrypt(root_dir);
        size_t pos = decrypted_data.find(parent_file_name);
        if (pos != string::npos) {
            // Find the end of the line containing the file name
            size_t endPos = decrypted_data.find('\n', pos);
            string line = decrypted_data.substr(pos, endPos-pos);

            // cout << "BEFORE: " << endl;
            // cout << line << endl;

            if(line.find(shared_username) != string::npos) return;

            if(line.find(SHARED) != string::npos) line = line + " " + shared_username + "\n";
            else line = line + " " + SHARED + " " + shared_username + "\n";
            decrypted_data.replace(pos, endPos - pos + 1, line);

            // cout << "AFTER: " << endl;
            // cout << line << endl;

            FileEncrypter::encrypt_file(root_dir + "/" + META_FILE, decrypted_data);
        }

    }

    static bool checkTheShareStatus(string root_dir, string parent_file_name) {
        map<string, string> key_value_pairs = getDataFromMetaFile(root_dir);
        auto it = key_value_pairs.find(parent_file_name);
        if (it == key_value_pairs.end()) {
            return false;
        }
        string val = it->second;
        if(val.find(SHARED) != string::npos) return true;
        return false;
    }

    static vector<string> getSharedUsernames(string root_dir, string parent_file_name) {
        map<string, string> key_value_pairs = getDataFromMetaFile(root_dir);
        auto it = key_value_pairs.find(parent_file_name);
        if (it == key_value_pairs.end()) {
            return {};
        }
        string input = it->second;
        // Find the start and end positions of the "shared" substring
        size_t start = input.find(SHARED) + SHARED.length() + 1; // add 7 to skip past "shared "
        size_t end = input.length();

        // Extract the substring containing the values we're interested in
        vector<string> users;
        string values = input.substr(start, end - start);

        // Use a string stream to split the values into individual strings
        istringstream iss(values);
        string value;
        while (iss >> value) {
            users.push_back(value);
        }
        return users;
    }

    static string getMetaValue(string root_dir, string key) {
        map<string, string> key_value_pairs = getDataFromMetaFile(root_dir);
        auto it = key_value_pairs.find(key);
        if (it == key_value_pairs.end()) {
            return "";
        }
        if(it->second.find(' ')) {
            string val = split(it->second, ' ')[0];
            return val;
        }
        return it->second;
    }

    static vector<string> split(string s, char delimiter) {
        vector<string> words;
        istringstream iss(s);
        string word;

        while (getline(iss, word, delimiter)) {
            words.push_back(word);
        }

        return words;
    };

    static string getMetaKey(string root_dir, string value) {
        map<string, string> key_value_pairs = getDataFromMetaFile(root_dir);
        for (auto it = key_value_pairs.begin(); it != key_value_pairs.end(); ++it) {
            string val = it->second;
            if(it->second.find(' ')) {
                 val = split(it->second, ' ')[0];
            }
            if (val == value) {
                return it->first;
            }
        }
        return "";
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
