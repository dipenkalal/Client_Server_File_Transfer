# Client_Server_File_Transfer 🌐

![GitHub stars](https://img.shields.io/github/stars/dipenkalal/Client_Server_File_Transfer?style=flat-square)
![GitHub forks](https://img.shields.io/github/forks/dipenkalal/Client_Server_File_Transfer?style=flat-square)

This repository contains a distributed file retrieval system implemented in C, featuring a client-server architecture. Clients can request a file or a set of files from the server, which searches for the requested files in its file directory rooted at its home directory. The server returns a compressed tarball (.tar.gz) of the requested files to the client, or an appropriate message if the files are not found. The system supports multiple clients connecting from different machines, processing client requests concurrently using separate child processes.

## Compatibility ✅

This project has been tested on the following operating systems:

- Garuda Linux

It is expected to work on other POSIX-compliant systems as well, such as other Linux distributions, macOS, and WSL on Windows.

## Features 🌟

- Search and retrieve files based on various criteria: filename, size range, date range, file extensions
- Server-side file compression using tar
- Concurrent handling of multiple client requests using process forking
- Alternating client connections between the main server and a mirror server
- Basic command syntax validation and error handling on the client-side

## Prerequisites 🔧

GCC (GNU Compiler Collection)
<br>
GNU Make
<br>
POSIX-compliant system (Linux, macOS, or WSL on Windows)

## Compilation 🛠️
To compile the project, run the following command in the root directory of the repository:
```
make
```

## Usage 🚀
### Server and Mirror
To start the server and the mirror, run the following commands on separate machines or terminals:
```
./mirror <mirror_port>
```
```
./server <server_port> <mirror_ip> <mirror_port>
```
### Client
To connect to the server and send a request, run the following command:
```
./client <server_ip> <port>
```



## List of Client Commands 📜
```
findfile <filename>
sgetfiles <size1> <size2> <-u>
dgetfiles <date1> <date2> <-u>
getfiles <file1> <file2> ... <file6> <-u>
gettargz <extension_list> <-u>
quit
```
