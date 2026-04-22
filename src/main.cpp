#include <iostream>
#include "integrity_checker.h"
#include <algorithm>
#include <map>
#include <filesystem>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

static std::atomic<bool> g_monitorRunning{false};
static void sigintHandler(int) { g_monitorRunning = false; }

int main () {
    std::string command;
    std::string filePath;
    std::cout << "File Integrity Checker - v1:" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Commands: Record, Check, Reset, Monitor, & Exit" << std::endl;
    std::cout << std::endl;

    while (true) {
        std::cout << "Enter Command: ";
        std::cin >> command;

        std::transform(command.begin(), command.end(), command.begin(), ::tolower);

        if (command == "reset") {
            std::cout << "Restarting..." << std::endl;
            std::cout << std::endl;
            std::cout << "File Integrity Checker - v1:" << std::endl;
            std::cout << "------------------------------------------" << std::endl;
            std::cout << "Commands: Record, Check, Reset, Monitor, & Exit" << std::endl;
            std::cout << std::endl;
            continue;
        }

        if (command == "exit") {
            std::cout << "Thank you, goodbye!" << std::endl;
            break;
        }

        if (command == "monitor") {
            std::string dirPath;
            std::cout << "Enter directory path: ";
            std::cin.ignore();
            std::getline(std::cin, dirPath);

            std::error_code ec;
            if (!std::filesystem::is_directory(dirPath, ec)) {
                std::cout << "Error: not a valid directory: " << dirPath << std::endl;
                continue;
            }

            std::cout << "Scanning and recording baseline for: " << dirPath << std::endl;

            std::map<std::string, std::string> knownState = scanDirectory(dirPath);

            //merge baseline into hashes.db, preserving unrelated entries
            std::map<std::string, std::string> allHashes = loadAllHashes();
            for (const auto& [path, hash] : knownState) {
                allHashes[path] = hash;
            }
            saveAllHashes(allHashes);

            std::cout << "Baseline recorded for " << knownState.size() << " file(s)." << std::endl;
            std::cout << "Monitoring every 10 seconds. Press Ctrl+C to stop." << std::endl;

            g_monitorRunning = true;
            std::signal(SIGINT, sigintHandler);

            while (g_monitorRunning) {
                std::this_thread::sleep_for(std::chrono::seconds(10));

                if (!g_monitorRunning) break;

                std::map<std::string, std::string> liveState = scanDirectory(dirPath);
                bool anyChange = false;

                //detect modified and deleted files
                for (const auto& [path, oldHash] : knownState) {
                    auto it = liveState.find(path);
                    if (it == liveState.end()) {
                        std::cout << "[DELETED]  " << path << std::endl;
                        anyChange = true;
                    } else if (it->second != oldHash) {
                        std::cout << "[MODIFIED] " << path << std::endl;
                        std::cout << "  Expected: " << oldHash << std::endl;
                        std::cout << "  Got:      " << it->second << std::endl;
                        anyChange = true;
                    }
                }

                //detect new files
                for (const auto& [path, newHash] : liveState) {
                    if (knownState.find(path) == knownState.end()) {
                        std::cout << "[NEW]      " << path << std::endl;
                        anyChange = true;
                    }
                }

                if (!anyChange) {
                    std::cout << "All files unchanged." << std::endl;
                }

                //update known state and persist — next cycle compares against latest
                knownState = liveState;
                std::map<std::string, std::string> allH = loadAllHashes();
                for (const auto& [path, hash] : knownState) {
                    allH[path] = hash;
                }
                //remove deleted files from hashes.db
                std::string prefix = std::filesystem::absolute(dirPath).string();
                for (auto it = allH.begin(); it != allH.end(); ) {
                    if (knownState.find(it->first) == knownState.end() &&
                        it->first.substr(0, prefix.size()) == prefix) {
                        it = allH.erase(it);
                    } else {
                        ++it;
                    }
                }
                saveAllHashes(allH);
            }

            std::signal(SIGINT, SIG_DFL);
            std::cout << std::endl << "Monitor stopped." << std::endl;
            continue;
        }

        std::cout << "Enter Filename: ";
        std::cin.ignore();
        std::getline(std::cin, filePath);

        if (command == "record") {
            //create hash of the file
            std::string hash = computeHash(filePath);

            //check if file is readable
            if (hash.empty()) {
                std::cout << "Error: unable to read file: " << filePath << std::endl;
                continue;
            }

            //save hash and confirm to the user
            saveHash(filePath, hash);
            std::cout << "Recording stored for: " << filePath << std::endl;
            std::cout << "SHA256: " << hash << std::endl;

        } else if (command == "check") {
            //load stored hash
            std::string storedHash = loadHash(filePath);

            if (storedHash.empty()) {
                std::cout << "Error: no recording found for: " << filePath << std::endl;
                std::cout << "Run 'Record' first to take an initial recording." << std::endl;
                continue;
            }

            //compute current hash
            std::string currentHash = computeHash(filePath);

            if (currentHash.empty()) {
                std::cout << "Error: unable to read file: " << filePath << std::endl;
                continue;
            }

            //compares hashes
            if (currentHash == storedHash) {
                std::cout << "File is unchanged: " << filePath << std::endl;
            } else {
                std::cout << "WARNING: File has been modified: " << filePath << std::endl;
                std::cout << "Expected: " << storedHash << std::endl;
                std::cout << "Got:      " << currentHash << std::endl;
            }

        } else {
            //unrecognized command
            std::cout << "Error: unknown command: " << command << std::endl;
            std::cout << "Valid commands are: Record, Check, Reset, Monitor, & Exit" << std::endl;
        }
    }
    return 0;
}
