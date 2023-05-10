#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>

#define _XOPEN_SOURCE 500
#include <ftw.h>

#define BUF_SIZE 1024
#define MAX_CLIENTS 4
#define COMMAND_SIZE 256

static const char *target_filename;
static char *found_path_ptr;

void process_client(int client_socket);
void search_file(const char *path, const char *filename, char *found_path);
char *format_time(time_t raw_time);
void search_files_by_size(const char *path, int size1, int size2, FILE *output);
void send_file(int client_socket, const char *filepath);
void search_files_by_date(const char *path, time_t date1, time_t date2, FILE *output);
time_t parse_date(const char *date_str);
void search_files_by_name(const char *path, const char *filenames[], int file_count, FILE *output, int *found_count);
void search_files_by_extensions(const char *path, const char *extensions[], int ext_count, FILE *output, int *found_count);
void forward_data(int source_socket, int destination_socket);

//Main main which uses user input port functionality
int main(int argc, char *argv[]) {
         if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_port> <mirror_ip> <mirror_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_port = atoi(argv[1]);
    const char *mirror_ip = argv[2];
    int mirror_port = atoi(argv[3]);

    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        printf("Invalid port number. Please provide a valid port number between 1 and 65535.\n");
        return 1;
    }

    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(1);
    }
    int optval = 1;
if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
    perror("setsockopt");
    exit(1);
}

server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = INADDR_ANY;
server_address.sin_port = htons(port);
if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    perror("bind");
    exit(1);
}
    if (listen(server_socket, 5) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Socket Created : OK \n");
    sleep(1);
    printf("Port Bind Successful : OK \n");
    sleep(1);
    printf("Server Running Successful : OK \n");
    sleep(1);
    printf("Server is listening on port %d : OK \n", port);
    sleep(1);
    // Test connection to mirror


// if (!connected_to_mirror) {
//     fprintf(stderr, "Unable to connect to mirror after %d retries\n", max_retries);
//     exit(EXIT_FAILURE);
// }
//
// close(test_mirror_socket);



    int connection_count = 0;
// while (1) {
//     struct sockaddr_in client_address;
//     socklen_t client_address_len = sizeof(client_address);
//     int new_socket;
//
//     connection_count++;
//
//     if (connection_count <= 4 || connection_count % 2 == 1) {
//         // Server handles the connection
//         printf("Accepting connection (connection count: %d)\n", connection_count);
//         new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
//         if (new_socket < 0) {
//             perror("Error on accept");
//             exit(EXIT_FAILURE);
//         }
//
//         int pid = fork();
//         if (pid < 0) {
//             perror("fork");
//             exit(1);
//         } else if (pid == 0) {
//             close(server_socket);
//             process_client(new_socket);
//             exit(0);
//         } else {
//             close(new_socket);
//         }
//
//     } else {
//     // Redirect the connection to the mirror
//     struct sockaddr_in mirror_address;
//     memset(&mirror_address, 0, sizeof(mirror_address));
//     mirror_address.sin_family = AF_INET;
//     mirror_address.sin_port = htons(mirror_port);
// //
// //     if (inet_pton(AF_INET, mirror_ip, &mirror_address.sin_addr) <= 0) {
// //         perror("Invalid mirror IP address");
// //         exit(EXIT_FAILURE);
// //     }
// //
// //     new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
// //     if (new_socket < 0) {
// //         perror("Error on accept");
// //         exit(EXIT_FAILURE);
// //     }
// //
// //     // Create a new socket to connect to the mirror
// //     int mirror_socket = socket(AF_INET, SOCK_STREAM, 0);
// //     if (mirror_socket < 0) {
// //         perror("Error creating mirror socket");
// //         exit(EXIT_FAILURE);
// //     }
// //
// //     // Connect to the mirror
// //     if (connect(mirror_socket, (struct sockaddr *)&mirror_address, sizeof(mirror_address)) < 0) {
// //         perror("Error connecting to mirror");
// //         exit(EXIT_FAILURE);
// //     }
// //
// //     // Send the client IP and port to the mirror process
// //     if (send(mirror_socket, &client_address, sizeof(client_address), 0) < 0) {
// //         perror("Error sending client address to mirror");
// //         exit(EXIT_FAILURE);
// //     }
// //
// //     // Close the mirror socket
// //     close(mirror_socket);
// //     close(new_socket);
// // }
//
// // Redirect the connection to the mirror
// int mirror_socket = -1;
// int num_retries = 0;
// int max_retries = 10;
// while (mirror_socket == -1 && num_retries < max_retries) {
//     mirror_socket = connect_to_mirror(mirror_ip, mirror_port);
//     if (mirror_socket == -1) {
//         num_retries++;
//         sleep(1);
//     }
// }
//
// if (mirror_socket == -1) {
//     fprintf(stderr, "Unable to connect to mirror after %d retries\n", max_retries);
// } else {
//     // Send the client IP and port to the mirror process
//     //sendto(mirror_socket, &client_address, sizeof(client_address), 0, (struct sockaddr *)&mirror_address, sizeof(mirror_address));
//     send(mirror_socket, &client_address, sizeof(client_address), 0);
//     printf("Sent client address to mirror\n");
//     close(mirror_socket);
// }
//
// }
// }
//
//     return 0;
// }


    while (1) {
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int new_socket;

    connection_count++;

    if (connection_count <= 4 || connection_count % 2 == 1) {
        // Server handles the connection
        printf("Accepting connection (connection count: %d)\n", connection_count);
        new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (new_socket < 0) {
            perror("Error on accept");
            exit(EXIT_FAILURE);
        }

        int pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            close(server_socket);
            process_client(new_socket);
            exit(0);
        } else {
            close(new_socket);
        }

    } else {
        // Redirect the connection to the mirror
        printf("Redirecting connection to mirror (connection count: %d)\n", connection_count);
        new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (new_socket < 0) {
            perror("Error on accept");
            exit(EXIT_FAILURE);
        }
        int mirror_socket = connect_to_mirror(mirror_ip, mirror_port);
        if (mirror_socket < 0) {
            perror("Error connecting to mirror");
            exit(EXIT_FAILURE);
        }
        // send(mirror_socket, &client_address, sizeof(client_address), 0);
        // printf("Sent client address to mirror\n");
        char redirect_command[] = "redirect";
send(mirror_socket, redirect_command, strlen(redirect_command), 0);
send(mirror_socket, &client_address, sizeof(client_address), 0);
printf("Sent redirect command and client address to mirror\n");

        close(mirror_socket);
        close(new_socket);
    }
}


//     while (1) {
//     struct sockaddr_in client_address;
//     socklen_t client_address_len = sizeof(client_address);
//     int new_socket;
//
//     connection_count++;
//
//     if (connection_count <= 4 || connection_count % 2 == 1) {
//         // Server handles the connection
//         printf("Accepting connection (connection count: %d)\n", connection_count);
//         new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
//         if (new_socket < 0) {
//             perror("Error on accept");
//             exit(EXIT_FAILURE);
//         }
//
//         int pid = fork();
//         if (pid < 0) {
//             perror("fork");
//             exit(1);
//         } else if (pid == 0) {
//             close(server_socket);
//             process_client(new_socket);
//             exit(0);
//         } else {
//             close(new_socket);
//         }
//
//     } else {
//         // Redirect the connection to the mirror
//         printf("Redirecting connection to mirror (connection count: %d)\n", connection_count);
//         new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
//         if (new_socket < 0) {
//             perror("Error on accept");
//             exit(EXIT_FAILURE);
//         }
//         int mirror_socket = connect_to_mirror(mirror_ip, mirror_port);
//         if (mirror_socket < 0) {
//             perror("Error connecting to mirror");
//             exit(EXIT_FAILURE);
//         }
//         send(mirror_socket, &client_address, sizeof(client_address), 0);
//         printf("Sent client address to mirror\n");
//         close(mirror_socket);
//         close(new_socket);
//     }
// }
}
int connect_to_mirror(const char *mirror_ip, int mirror_port) {
    int mirror_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (mirror_socket < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in mirror_address;
    memset(&mirror_address, 0, sizeof(mirror_address));
    mirror_address.sin_family = AF_INET;
    mirror_address.sin_port = htons(mirror_port);

    if (inet_pton(AF_INET, mirror_ip, &mirror_address.sin_addr) <= 0) {
        perror("Invalid mirror IP address");
        return -1;
    }

    if (connect(mirror_socket, (struct sockaddr *)&mirror_address, sizeof(mirror_address)) < 0) {
        perror("Error connecting to mirror");
        return -1;
    }

    return mirror_socket;
}

// int connect_to_mirror(const char *mirror_ip, int mirror_port) {
//     int mirror_socket = socket(AF_INET, SOCK_DGRAM, 0); // Change to SOCK_DGRAM
//     if (mirror_socket < 0) {
//         perror("socket");
//         return -1;
//     }
//
//     struct sockaddr_in mirror_address;
//     memset(&mirror_address, 0, sizeof(mirror_address));
//     mirror_address.sin_family = AF_INET;
//     mirror_address.sin_port = htons(mirror_port);
//
//     if (inet_pton(AF_INET, mirror_ip, &mirror_address.sin_addr) <= 0) {
//         perror("Invalid mirror IP address");
//         return -1;
//     }
//
//     return mirror_socket;
// }



void process_client(int client_socket) {
    printf("Server is handling the client");
    char buffer[BUF_SIZE];
    char command[COMMAND_SIZE];
    ssize_t read_size;
    while (1) {
        memset(buffer, 0, BUF_SIZE);
        read_size = read(client_socket, buffer, BUF_SIZE - 1);
        if (read_size <= 0) {
            break;
        }
        sscanf(buffer, "%s", command);
//Find File Funnctionality
        if (strcmp(command, "findfile") == 0) {
        char filename[COMMAND_SIZE];
        char found_path[COMMAND_SIZE] = {0};
        sscanf(buffer, "%*s %s", filename);
        printf("findfile command received : OK \n");
        //modify here
        search_file("/", filename, found_path);
        if (strlen(found_path) > 0) {
            struct stat file_stat;
            stat(found_path, &file_stat);
            off_t file_size = file_stat.st_size;
            time_t creation_time = file_stat.st_ctime;
            char *formatted_time = format_time(creation_time);
            snprintf(buffer, BUF_SIZE, "File: %s\nSize: %ld bytes\nDate created: %s\n", found_path, file_size, formatted_time);
            free(formatted_time);
            printf("List Retrieved : Sent OK \n");
        } else {
            strncpy(buffer, "File not found\n", BUF_SIZE - 1);
            printf("No file found : Sent OK \n");
        }
        send(client_socket, buffer, strlen(buffer), 0);
        }
        //sgetfile Functionality
        else if (strcmp(command, "sgetfiles") == 0) {
        int size1, size2;
        sscanf(buffer, "%*s %d %d", &size1, &size2);
        printf("sgetfiles command received : OK \n");
        // Create a temporary file to store the list of files to be included in the tar.gz file
        FILE *file_list = tmpfile();
        search_files_by_size(".", size1, size2, file_list);
        rewind(file_list);
        // Create a tar.gz file containing the files within the specified size range
        char tar_filename[] = "/tmp/sgetfilesXXXXXX.tar.gz";
        int tar_fd = mkstemps(tar_filename, 7);
        close(tar_fd); // Close the file descriptor since we will use the filename with popen()
        char tar_command[COMMAND_SIZE];
        snprintf(tar_command, COMMAND_SIZE, "tar czf %s -T /dev/fd/%d", tar_filename, fileno(file_list));
        FILE *tar_process = popen(tar_command, "w");
        pclose(tar_process);
        printf("sgetfiles tar sending to client : OK \n");
        // Send the tar.gz file to the client
        send_file(client_socket, tar_filename);
        // Clean up temporary files
        fclose(file_list);
        remove(tar_filename);
        }
        //dgetfile Functionality
        else if (strcmp(command, "dgetfiles") == 0) {
        char date1_str[11], date2_str[11];
        printf("dgetfiles command received : OK \n");
        sscanf(buffer, "%*s %s %s", date1_str, date2_str);
        time_t date1 = parse_date(date1_str);
        time_t date2 = parse_date(date2_str);
        // Create a temporary file to store the list of files to be included in the tar.gz file
        FILE *file_list = tmpfile();
        search_files_by_date(".", date1, date2, file_list);
        rewind(file_list);
        // Create a tar.gz file containing the files within the specified date range
        char tar_filename[] = "/tmp/dgetfilesXXXXXX.tar.gz";
        int tar_fd = mkstemps(tar_filename, 7);
        close(tar_fd); // Close the file descriptor since we will use the filename with popen()
        char tar_command[COMMAND_SIZE];
        snprintf(tar_command, COMMAND_SIZE, "tar czf %s -T /dev/fd/%d", tar_filename, fileno(file_list));
        FILE *tar_process = popen(tar_command, "w");
        pclose(tar_process);
        printf("dgetfiles tar sending to client : OK \n");
        // Send the tar.gz file to the client
        send_file(client_socket, tar_filename);
        // Clean up temporary files
        fclose(file_list);
        remove(tar_filename);
        }
        //getfiles Functionality
        else if (strcmp(command, "getfiles") == 0) {
        const char *filenames[6];
        int file_count = sscanf(buffer, "%*s %ms %ms %ms %ms %ms %ms", &filenames[0], &filenames[1], &filenames[2], &filenames[3], &filenames[4], &filenames[5]);
        printf("getfiles command received : OK \n");
        // Create a temporary file to store the list of files to be included in the tar.gz file
        FILE *file_list = tmpfile();
        int found_count = 0;
        search_files_by_name(".", filenames, file_count, file_list, &found_count);
        rewind(file_list);
        if (found_count > 0) {
            // Create a tar.gz file containing the specified files
            char tar_filename[] = "/tmp/getfilesXXXXXX.tar.gz";
            printf("getfiles tar Created : OK");
            int tar_fd = mkstemps(tar_filename, 7);
            close(tar_fd); // Close the file descriptor since we will use the filename with popen()
            char tar_command[COMMAND_SIZE];
            snprintf(tar_command, COMMAND_SIZE, "tar czf %s -T /dev/fd/%d", tar_filename, fileno(file_list));
            FILE *tar_process = popen(tar_command, "w");
            pclose(tar_process);
            printf("getfiles tar sending to client : OK \n");
            // Send the tar.gz file to the client
            send_file(client_socket, tar_filename);
            // Clean up temporary files
            remove(tar_filename);
        } else {
            // Send "No file found" message to the client
            char *msg = "No file found";
            printf("No file found : Sent OK \n");
            send(client_socket, msg, strlen(msg), 0);
        }
        fclose(file_list);
        for (int i = 0; i < file_count; i++) {
            free((char *)filenames[i]);
        }
        }
        //gettargz Functionality
        else if (strcmp(command, "gettargz") == 0) {
        const char *extensions[6];
        int ext_count = sscanf(buffer, "%*s %ms %ms %ms %ms %ms %ms", &extensions[0], &extensions[1], &extensions[2], &extensions[3], &extensions[4], &extensions[5]);
        printf("gettargz command received : OK \n");
        // Create a temporary file to store the list of files to be included in the tar.gz file
        FILE *file_list = tmpfile();
        int found_count = 0;

        search_files_by_extensions(".", extensions, ext_count, file_list, &found_count);
        rewind(file_list);
        if (found_count > 0) {
            // Create a tar.gz file containing the files with the specified extensions
            char tar_filename[] = "/tmp/gettargzXXXXXX.tar.gz";
            printf("gettargz Created : OK");
            int tar_fd = mkstemps(tar_filename, 7);
            close(tar_fd); // Close the file descriptor since we will use the filename with popen()
            char tar_command[COMMAND_SIZE];
            snprintf(tar_command, COMMAND_SIZE, "tar czf %s -T /dev/fd/%d", tar_filename, fileno(file_list));
            FILE *tar_process = popen(tar_command, "w");
            pclose(tar_process);
            printf("gettargz sending to client : OK \n");
            // Send the tar.gz file to the client
            send_file(client_socket, tar_filename);
            // Clean up temporary files
            remove(tar_filename);
        } else {
            // Send "No file found" message to the client
            char *msg = "No file found";
            printf("No file found : Sent OK \n");
            send(client_socket, msg, strlen(msg), 0);
        }
        fclose(file_list);
        for (int i = 0; i < ext_count; i++) {
            free((char *)extensions[i]);
        }
        }
        else if (strcmp(command, "greet") == 0) {
        char greeting[] = "Hello from the server! \n";
        printf("Hello from the server : Sent OK \n");
        send(client_socket, greeting, strlen(greeting), 0);
        }
        //quit Functionality
        else if (strcmp(command, "quit") == 0) {
            sleep(1);
            printf("quit command received from client : OK \n");
            sleep(1);
            printf("Closing connection for client : OK \n");
            sleep(1);
            printf("Client released : OK \n");
            break;
        } else {
            char error_msg[] = "Invalid command received. Please try again.\n";
            printf("Invalid command recevied : Sent OK \n");
            send(client_socket, error_msg, strlen(error_msg), 0);
        }
    }

    close(client_socket);
}

void search_file(const char *path, const char *filename, char *found_path) {
    DIR *dir = opendir(path);
    if (!dir) {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char new_path[COMMAND_SIZE];
            snprintf(new_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
            search_file(new_path, filename, found_path);
            if (strlen(found_path) > 0) {
                break;
            }
        } else if (entry->d_type == DT_REG) {
            if (strcmp(entry->d_name, filename) == 0) {
                snprintf(found_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
                break;
            }
        }
    }
    closedir(dir);
}

char *format_time(time_t raw_time) {
    struct tm *time_info = localtime(&raw_time);
    char *formatted_time = (char *)malloc(sizeof(char) * 20);
    strftime(formatted_time, 20, "%Y-%m-%d %H:%M:%S", time_info);
    return formatted_time;
}
void search_files_by_size(const char *path, int size1, int size2, FILE *output) {
    DIR *dir = opendir(path);
    if (!dir) {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char new_path[COMMAND_SIZE];
            snprintf(new_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
            search_files_by_size(new_path, size1, size2, output);
        } else if (entry->d_type == DT_REG) {
            char file_path[COMMAND_SIZE];
            snprintf(file_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
            struct stat file_stat;
            stat(file_path, &file_stat);
            off_t file_size = file_stat.st_size;
            if (file_size >= size1 && file_size <= size2) {
                fprintf(output, "%s\n", file_path);
            }
        }
    }
    closedir(dir);
}

void send_file(int client_socket, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    send(client_socket, &file_size, sizeof(file_size), 0);

    char buffer[1024];
    size_t bytes_sent = 0;
    while (bytes_sent < file_size) {
        size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
        if (bytes_read <= 0) {
            break;
        }
        printf("Sending: ");
for (size_t i = 0; i < bytes_read; i++) {
    printf("%02X ", (unsigned char)buffer[i]);
}
printf("\n");

    }

    fclose(file);
}
void search_files_by_date(const char *path, time_t date1, time_t date2, FILE *output) {
    DIR *dir = opendir(path);
    if (!dir) {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char new_path[COMMAND_SIZE];
            snprintf(new_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
            search_files_by_date(new_path, date1, date2, output);
        } else if (entry->d_type == DT_REG) {
            char file_path[COMMAND_SIZE];
            snprintf(file_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
            struct stat file_stat;
            stat(file_path, &file_stat);
            time_t creation_time = file_stat.st_ctime;
            if (creation_time >= date1 && creation_time <= date2) {
                fprintf(output, "%s\n", file_path);
            }
        }
    }
    closedir(dir);
}
time_t parse_date(const char *date_str) {
    struct tm time_info = {0};
    strptime(date_str, "%Y-%m-%d", &time_info);
    return mktime(&time_info);
}
void search_files_by_name(const char *path, const char *filenames[], int file_count, FILE *output, int *found_count) {
    DIR *dir = opendir(path);
    if (!dir) {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char new_path[COMMAND_SIZE];
            snprintf(new_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
            search_files_by_name(new_path, filenames, file_count, output, found_count);
        } else if (entry->d_type == DT_REG) {
            for (int i = 0; i < file_count; i++) {
                if (strcmp(entry->d_name, filenames[i]) == 0) {
                    char file_path[COMMAND_SIZE];
                    snprintf(file_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
                    fprintf(output, "%s\n", file_path);
                    (*found_count)++;
                    break;
                }
            }
        }
    }
    closedir(dir);
}
void search_files_by_extensions(const char *path, const char *extensions[], int ext_count, FILE *output, int *found_count) {
    DIR *dir = opendir(path);
    if (!dir) {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char new_path[COMMAND_SIZE];
            snprintf(new_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
            search_files_by_extensions(new_path, extensions, ext_count, output, found_count);
        } else if (entry->d_type == DT_REG) {
            for (int i = 0; i < ext_count; i++) {
                const char *ext = strrchr(entry->d_name, '.');
                if (ext && strcmp(ext, extensions[i]) == 0) {
                    char file_path[COMMAND_SIZE];
                    snprintf(file_path, COMMAND_SIZE, "%s/%s", path, entry->d_name);
                    fprintf(output, "%s\n", file_path);
                    (*found_count)++;
                    break;
                }
            }
        }
    }
    closedir(dir);
}

void forward_data(int source_socket, int destination_socket) {
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = recv(source_socket, buffer, sizeof(buffer), 0)) > 0) {
        send(destination_socket, buffer, bytes_read, 0);
    }
}


