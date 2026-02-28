#include <iostream>
#include "integrity_checker.h"
#include <algorithm>

int main () {
    std::string command;
    std::string filePath;
    std::cout << "File Integrity Checker - v1:" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Commands: Record, Check, Reset, & Exit" << std::endl;
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
            std::cout << "Commands: Record, Check, Reset, & Exit" << std::endl;
            std::cout << std::endl;
            continue;
        }



        if (command == "exit") {
            std::cout << "Thank you, goodbye!" << std::endl;
            break;
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
            std::cout << "Valid commands are: Record, Check, Reset, & Exit" << std::endl;
        }
    }
        return 0;
    }