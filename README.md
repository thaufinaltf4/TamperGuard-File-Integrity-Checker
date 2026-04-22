# File Integrity Checker

A command-line tool that detects file tampering using SHA256 hashing. Built in C++ with OpenSSL.

---

## Ideation

I wanted to build something practical with C++ that went beyond the basic data structures I learned in my DSA class. File integrity checking is a real problem, used in everything from malware detection to verifying software downloads. The core idea is simple: take a snapshot of a file's hash, then compare it later to see if anything changed.

---

## How it works

The tool has three commands:

- **Record** - computes a SHA256 hash of a file and saves it to a local database (`hashes.db`)
- **Check** - recomputes the hash and compares it against the stored one, warning you if anything changed
- **Reset** - clears the screen and restarts the session

Hashes are stored in a plain text file with one `filepath hash` entry per line. When updating an existing entry, it writes to a temp file first then swaps it in to avoid corrupting the database mid-write.

---

## Stack

- C++17
- OpenSSL (EVP API for SHA256)
- CMake

---

## Building locally

You'll need OpenSSL installed. On macOS:

```bash
brew install openssl
```

Then build with CMake:

```bash
mkdir build && cd build
cmake ..
make
./file_integrity_checker
```

---

## Usage

```
Commands: Record, Check, Reset, & Exit

Enter Command: record
Enter Filename: /path/to/file.txt
Recording stored for: /path/to/file.txt
SHA256: 2cf24dba5fb0a30e...

Enter Command: check
Enter Filename: /path/to/file.txt
File is unchanged: /path/to/file.txt
```

If a file has been modified since it was recorded:

```
WARNING: File has been modified: /path/to/file.txt
Expected: 2cf24dba5fb0a30e...
Got:      82dea8e2b3c1f49a...
```
