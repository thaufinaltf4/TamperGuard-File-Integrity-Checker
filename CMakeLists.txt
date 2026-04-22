cmake_minimum_required(VERSION 3.20)
project(file_integrity_checker)

set(CMAKE_CXX_STANDARD 17)

set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@3)

find_package(OpenSSL REQUIRED)

add_executable(file_integrity_checker
        src/main.cpp
        include/integrity_checker.h
        src/integrity_checker.cpp
)

target_include_directories(file_integrity_checker PRIVATE include)
target_link_libraries(file_integrity_checker OpenSSL::Crypto)