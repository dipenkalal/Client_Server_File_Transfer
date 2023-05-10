#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>


bool is_valid_date(const char *date) {
    if (strlen(date) != 10) return false;
    return (isdigit(date[0]) && isdigit(date[1]) && isdigit(date[2]) && isdigit(date[3]) &&
            date[4] == '-' && isdigit(date[5]) && isdigit(date[6]) && date[7] == '-' &&
            isdigit(date[8]) && isdigit(date[9]));
}

bool validate_command(const char *command) {
    char cmd[1024];
    strcpy(cmd, command);
    char *token = strtok(cmd, " ");

    if (strcmp(token, "greet") == 0) {
        printf("Command sent : OK \n");
        return true;
    } else if (strcmp(token, "findfile") == 0) {
        printf("Command sent : OK \n");
        token = strtok(NULL, " ");
        if (token != NULL) {
            return true;
        }
    } else if (strcmp(token, "sgetfiles") == 0) {
        printf("Command sent : OK \n");
        token = strtok(NULL, " ");
        if (token != NULL && atoi(token) >= 0) {
            token = strtok(NULL, " ");
            if (token != NULL && atoi(token) >= 0) {
                token = strtok(NULL, " ");
                if (token == NULL || strcmp(token, "-u") == 0) {
                    return true;
                }
            }
        }
    } else if (strcmp(token, "dgetfiles") == 0) {
        printf("Command sent : OK \n");
        token = strtok(NULL, " ");
        if (token != NULL && atoi(token) >= 0) {
            token = strtok(NULL, " ");
            if (token != NULL && atoi(token) >= 0) {
                token = strtok(NULL, " ");
                if (token == NULL || strcmp(token, "-u") == 0) {
                    return true;
                }
            }
        } else if (strcmp(token, "getfiles") == 0) {
            printf("Command sent : OK \n");
        int file_count = 0;
        bool has_u_option = false;
        while ((token = strtok(NULL, " ")) != NULL) {
            if (strcmp(token, "-u") == 0) {
                has_u_option = true;
                break;
            }
            file_count++;
        }
        if (file_count > 0 && file_count <= 6 && (!has_u_option || (has_u_option && strtok(NULL, " ") == NULL))) {
            return true;
        }
    } else if (strcmp(token, "gettargz") == 0) {
        printf("Command sent : OK \n");
        int ext_count = 0;
        bool has_u_option = false;
        while ((token = strtok(NULL, " ")) != NULL) {
            if (strcmp(token, "-u") == 0) {
                has_u_option = true;
                break;
            }
            ext_count++;
        }
        if (ext_count > 0 && ext_count <= 6 && (!has_u_option || (has_u_option && strtok(NULL, " ") == NULL))) {
            return true;
        }
    } else if (strcmp(token, "quit") == 0) {
        printf("Command sent : OK \n");
        return true;
    }

    return false;
}

}

/////////////////////
// void handle_sgetfiles(int client_socket, const char *command) {
//     send(client_socket, command, strlen(command), 0);
//
//     size_t file_size;
//     recv(client_socket, &file_size, sizeof(file_size), 0);
//
//     if (file_size == 0) {
//         printf("Server: No matching files found.\n");
//         return;
//     }
//
//     FILE *output_file = fopen("sgetfiles_output.txt", "w");
//     if (output_file == NULL) {
//         perror("fopen");
//         return;
//     }
//
//     char buffer[1024];
//     size_t bytes_received = 0;
//
//     while (bytes_received < file_size) {
//         ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
// /////
//         printf("Received: ");
// for (size_t i = 0; i < bytes_read; i++) {
//     unsigned char ch = (unsigned char)buffer[i];
//     if (isprint(ch)) {
//         printf("%c", ch);
//     } else {
//         printf("\\x%02X", ch);
//     }
// }
// printf("\n");
//
//         if (bytes_read <= 0) {
//             perror("recv");
//             break;
//         }
//
//         buffer[bytes_read] = '\0';
//         fprintf(output_file, "%s", buffer);
//         bytes_received += bytes_read;
// }
//
//     fclose(output_file);
//     printf("Server: sgetfiles output saved to sgetfiles_output.txt\n");
// }

void handle_sgetfiles(int client_socket, const char *command) {

    if(send(client_socket, command, strlen(command), 0) == -1){
        perror("send");
        return;
    }


    //send(client_socket, command, strlen(command), 0);

    size_t file_size;
    if(recv(client_socket, &file_size, sizeof(file_size), 0) == -1){
        perror("recv");
        return;
    }
    //recv(client_socket, &file_size, sizeof(file_size), 0);

    if (file_size == 0) {
        printf("Server: No matching files found.\n");
        return;
    }

    FILE *output_file = fopen("sgetfiles_output.txt", "w");
    if (output_file == NULL) {
        perror("fopen");
        return;
    }

    char buffer[1024];
    size_t bytes_received = 0;

    while (bytes_received < file_size) {
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Received %zd bytes.\n", bytes_read);

        if (bytes_read <= 0) {
            perror("recv");
            break;
        }

        if (fwrite(buffer, 1, bytes_read, output_file) != bytes_read) {
            perror("fwrite");
            break;
        }

        bytes_received += bytes_read;
    }


    // while (bytes_received < file_size) {
    //     ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    //     printf("Received: ");
    //     for (size_t i = 0; i < bytes_read; i++) {
    //         unsigned char ch = (unsigned char)buffer[i];
    //         if (isprint(ch)) {
    //             printf("%c", ch);
    //         } else {
    //             printf("\\x%02X", ch);
    //         }
    //     }
    //     printf("\n");
    //
    //     if (bytes_read <= 0) {
    //         perror("recv");
    //         break;
    //     }
    //
    //     buffer[bytes_read] = '\0';
    //     fprintf(output_file, "%s", buffer);
    //     bytes_received += bytes_read;
    // }

    fclose(output_file);
    printf("Server: sgetfiles output saved to sgetfiles_output.txt\n");
}


void handle_dgetfiles(int client_socket, const char *command) {
    send(client_socket, command, strlen(command), 0);

    size_t file_size;
    recv(client_socket, &file_size, sizeof(file_size), 0);

    if (file_size == 0) {
        printf("Server: No matching files found.\n");
        return;
    }

    FILE *output_file = fopen("dgetfiles_output.txt", "w");
    if (output_file == NULL) {
        perror("fopen");
        return;
    }

    char buffer[1024];
    size_t bytes_received = 0;

    while (bytes_received < file_size) {
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        printf("Received: ");
for (size_t i = 0; i < bytes_read; i++) {
    unsigned char ch = (unsigned char)buffer[i];
    if (isprint(ch)) {
        printf("%c", ch);
    } else {
        printf("\\x%02X", ch);
    }
}
printf("\n");

        if (bytes_read <= 0) {
            perror("recv");
            break;
        }

        buffer[bytes_read] = '\0';
        fprintf(output_file, "%s", buffer);
        bytes_received += bytes_read;
    }

    fclose(output_file);
    printf("Server: dgetfiles output saved to dgetfiles_output.txt\n");

}

void receive_response(int client_socket) {
    char buffer[1024] = {0};
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        exit(1);
    } else if (bytes_received == 0) {
        printf("Command sent : OK \n");
        sleep(1);
        printf("Okay Bye... \n");
        sleep(1);
        printf("Server closed the connection.\n");
        exit(0);
    } else {
        buffer[bytes_received] = '\0';
        printf("Server: %s\n", buffer);
    }
}

//process_command Functionality
// void process_command(int client_socket) {
//     char command[1024];
//     printf("Enter command: ");
//     fgets(command, sizeof(command), stdin);
//     command[strcspn(command, "\n")] = 0;
//
//     if (validate_command(command)) {
//         if (send(client_socket, command, strlen(command), 0) < 0) {
//             perror("send");
//             exit(1);
//         }
//
//         // Receive the response from the server
//         char server_response[1024];
//         ssize_t received_length = recv(client_socket, server_response, sizeof(server_response) - 1, 0);
//         if (received_length < 0) {
//             perror("recv");
//             exit(1);
//         }
//         server_response[received_length] = '\0'; // Make sure the received data is null-terminated
//         printf("Server: %s\n", server_response);
//
//     } else {
//         printf("Invalid command. Please enter a valid command.\n");
//     }
// }
//
void process_command(int client_socket) {
    char command[1024];
    printf("C$ ");
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = 0;

    if (validate_command(command)) {
        if (strncmp(command, "sgetfiles", 9) == 0) {
            handle_sgetfiles(client_socket, command);
        } else if (strncmp(command, "dgetfiles", 9) == 0) {
            handle_dgetfiles(client_socket, command);
        } else {
            if (send(client_socket, command, strlen(command), 0) < 0) {
                perror("send");
                exit(1);
            }
            receive_response(client_socket);
        }
    } else {
        printf("Invalid command. Please enter a valid command.\n");
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);
    printf("\nCommand received %s %s %s : OK\n", argv[0],argv[1],argv[2]);
    sleep(1);
    printf("Sending connection requestion to server at %s address \n", argv[1]);
    sleep(1);
    printf("Binding connection port requestion on %s port \n", argv[2]);
    sleep(1);
    printf("\nList of Commands: \n");
    printf("1. findfile filename \n");
    printf("2. sgetfiles size1 size2 -u \n");
    printf("3. dgetfiles date1 date2 -u \n");
    printf("4. getfiles file1 file2 file3 file4 file5 file6 -u \n");
    printf("5. gettargz <extension list> -u //up to 6 different file types \n");
    printf("6. quit \n \n");


    if (port <= 0 || port > 65535) {
        printf("Invalid port number. Please provide a valid port number between 1 and 65535.\n");
        return 1;
    }

     int client_socket;
     struct sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("inet_pton");
        exit(1);
    }

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connect");
        exit(1);
    }

    // Client processing loop
    while (1) {
        process_command(client_socket);
    }

    close(client_socket);

    return 0;
}
