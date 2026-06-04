#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAXLINE] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the network address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Use loopback address for local machine
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Accept a connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Chat loop
    while(1) {
        memset(buffer, 0, MAXLINE);
        int valread = recv(new_socket, buffer, MAXLINE, 0);
        if (valread < 0) {
            perror("recv failed");
            close(new_socket);
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("Client: %s\n", buffer);
        if (strcmp(buffer, "BYE") == 0) {
            printf("Client has terminated the chat.\n");
            break;
        }

        printf("Server: ");
        fgets(buffer, MAXLINE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Remove trailing newline

        send(new_socket, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "BYE") == 0) {
            printf("Server has terminated the chat.\n");
            break;
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
