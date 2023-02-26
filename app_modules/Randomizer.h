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

};

#endif //FILESYSTEM_APP_RANDOMIZER_H