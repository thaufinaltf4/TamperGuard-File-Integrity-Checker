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
#include <filesystem>
#include <map>

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
        if (line.size() > filePath.size() &&
            line.substr(0, filePath.size()) == filePath &&
            line[filePath.size()] == ' ') {
            tempFile << filePath + " " + hash + "\n"; //it does
            found = true;
        } else {
            tempFile << line << "\n"; //it doesn't - copy line across
        }
    }

    if (!found) {
        tempFile << filePath + " " + hash + "\n"; //if the file's not found, its new
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
        if (line.size() > filePath.size() &&
            line.substr(0, filePath.size()) == filePath &&
            line[filePath.size()] == ' ') {
            return line.substr(filePath.size() + 1); //return hash
        }
    }

    return ""; //not found
}

std::map<std::string, std::string> scanDirectory(const std::string& dirPath) {
    namespace fs = std::filesystem;
    std::map<std::string, std::string> result;

    std::error_code ec;
    for (const auto& entry :
         fs::recursive_directory_iterator(dirPath,
             fs::directory_options::skip_permission_denied, ec)) {
        if (entry.is_regular_file(ec) && !ec) {
            std::string path = fs::absolute(entry.path()).string();
            std::string hash = computeHash(path);
            if (!hash.empty()) {
                result[path] = hash;
            }
        }
    }
    return result;
}

std::map<std::string, std::string> loadAllHashes() {
    std::map<std::string, std::string> result;
    std::ifstream inFile("hashes.db");
    std::string line;

    while (std::getline(inFile, line)) {
        // rfind handles paths with spaces — hash is always the last token
        size_t spacePos = line.rfind(' ');
        if (spacePos != std::string::npos && spacePos + 1 < line.size()) {
            result[line.substr(0, spacePos)] = line.substr(spacePos + 1);
        }
    }
    return result;
}

void saveAllHashes(const std::map<std::string, std::string>& hashes) {
    std::ofstream outFile("hashes.db", std::ios::trunc);
    for (const auto& [path, hash] : hashes) {
        outFile << path << " " << hash << "\n";
    }
}
