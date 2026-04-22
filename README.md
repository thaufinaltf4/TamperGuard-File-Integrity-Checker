# File Integrity Checker

A command-line tool that detects file tampering using SHA256 hashing. Built in C++ with OpenSSL.

---

## Ideation

I wanted to build something practical with C++ that went beyond the basic data structures I learned in my DSA class. File integrity checking is a real problem, used in everything from malware detection to verifying software downloads. The core idea is simple: take a snapshot of a file's hash, then compare it later to see if anything changed.

---

## How it works

The tool has four commands:

- **Record** - computes a SHA256 hash of a file and saves it to a local database (`hashes.db`)
- **Check** - recomputes the hash and compares it against the stored one, warning you if anything changed
- **Monitor** - recursively scans a directory, records a baseline of all files, then polls every 10 seconds detecting modified, new, and deleted files in real time
- **Reset** - clears the screen and restarts the session

Hashes are stored in a plain text file with one `filepath hash` entry per line. When updating an existing entry, it writes to a temp file first then swaps it in to avoid corrupting the database mid-write.

The monitor command uses `std::filesystem` for recursive directory traversal and `std::atomic` with a SIGINT handler for clean Ctrl+C exit.

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

**Recording and checking a single file:**

```
Commands: Record, Check, Reset, Monitor, & Exit

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

**Monitoring a directory:**

```
Enter Command: monitor
Enter directory path: /path/to/dir
Scanning and recording baseline for: /path/to/dir
Baseline recorded for 3 file(s).
Monitoring every 10 seconds. Press Ctrl+C to stop.
All files unchanged.
[MODIFIED] /path/to/dir/config.txt
  Expected: 2cf24dba5fb0a30e...
  Got:      82dea8e2b3c1f49a...
[NEW]      /path/to/dir/suspicious.sh
[DELETED]  /path/to/dir/original.txt
```

---

## Note on commit history

The initial version of this project (Record, Check, Reset commands) was built throughout February 2026. The monitor command was added on April 22, 2026.

The git history doesn't reflect this accurately — a `git pull --rebase` went wrong during the April push and wiped the local commit history, so everything ended up in a single root commit. The timeline above is the accurate one.
