#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(7777);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) 
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for client...\n");

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) 
    {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    read(new_socket, buffer, 1024);
    printf("File to be transferred is %s\n", buffer);
    FILE *file = fopen(buffer, "rb");
    if (file == NULL) 
    {
        perror("File not found");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) 
    {
        int bytes_read = fread(buffer, 1, 1024, file);
        if (bytes_read > 0) 
            send(new_socket, buffer, bytes_read, 0);

        if (bytes_read < 1024) 
        {
            if (feof(file)) 
                printf("File Transfer Completed.\n");
            if (ferror(file)) 
                perror("Error reading the file");
            break;
        }
    }

    fclose(file);
    close(new_socket);
    close(server_fd);
    return 0;
}