//
// Created by Thaufin Altaf on 2/18/26.
//

#include "integrity_checker.h"
#include <openssl/evp.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdio>

std::string computeHash(const std::string& filePath) {
    //opens file in binary mode
    std::ifstream file (filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";

    }

    //openssl hashing context
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);

    //read file in chunks and send them to hasher
    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        EVP_DigestUpdate(ctx, buffer, file.gcount());
    }
    EVP_DigestUpdate(ctx, buffer, file.gcount());

    //finish and get raw hash bytes
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    EVP_DigestFinal_ex(ctx, hash, &hashLen);
    EVP_MD_CTX_free(ctx);

    //convert raw bytes into hex string
    std::stringstream ss;
    for (unsigned int i = 0; i < hashLen; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

void saveHash(const std::string& filePath, const std::string& hash) {
    std::ifstream inFile("hashes.db"); //load existing file
    std::ofstream tempFile("hashes.tmp"); //write to temp file to avoid complications

    std::string line;
    bool found = false;

    //go through line-by-line of the existing file, check if it belongs
    while (std::getline(inFile, line)) {
        if (line.substr(0, filePath.size()) == filePath) {
            tempFile << filePath + " " + hash + "\n"; //it does
            found = true;
        } else {
            tempFile << line << "\n"; //it doesn't - copy line across
        }
    }

    if (!found) {
        tempFile <<filePath + " " + hash + "\n"; //if the file's not found, its new
    }

    inFile.close();
    tempFile.close();

    //replace with updated temp file
    std::remove("hashes.db");
    std::rename("hashes.tmp", "hashes.db");
}

std::string loadHash(const std::string& filePath) {
    std::ifstream inFile("hashes.db"); //open
    std::string line;


    while (std::getline(inFile, line)) {
        if (line.substr(0, filePath.size()) == filePath) { //check if it belongs
            return line.substr(filePath.size() + 1); //return hash
        }
    }

    return ""; //not found
}