#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define MAXLINE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAXLINE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while(1) 
    {
        printf("Client: ");
        fgets(buffer, MAXLINE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  
        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "BYE") == 0) 
        {
            printf("Client has terminated the chat.\n");
            break;
        }

        memset(buffer, 0, MAXLINE);
        int valread = recv(sock, buffer, MAXLINE, 0);
        if (valread < 0) 
        {
            perror("recv failed");
            close(sock);
            exit(EXIT_FAILURE);
        }

        printf("Server: %s\n", buffer);
        if (strcmp(buffer, "BYE") == 0) 
        {
            printf("Server has terminated the chat.\n");
            break;
        }
    }

    close(sock);
    return 0;
}