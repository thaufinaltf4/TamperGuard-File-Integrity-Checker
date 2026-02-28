//
// Created by Thaufin Altaf on 2/18/26.
//

#ifndef INTEGRITY_CHECKER_H
#define INTEGRITY_CHECKER_H

#include <string>

//takes a file path, returns its SHA256 hash as a hex string
//returns empty string if the file can't be read
std::string computeHash(const std::string& filePath);

//saves a filename w/ hash pair to hashes.db
void saveHash(const std::string& filePath, const std::string& hash);

//loads stored hash for file from hashes.db
//returns empty string if not found
std::string loadHash(const std::string& filePath);

#endif