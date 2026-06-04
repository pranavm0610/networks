#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

struct sockaddr_in clients[MAX_CLIENTS];
int client_count = 0;

void add_client(struct sockaddr_in client_addr) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].sin_addr.s_addr == client_addr.sin_addr.s_addr &&
            clients[i].sin_port == client_addr.sin_port) {
            return;  // Client already exists
        }
    }
    clients[client_count++] = client_addr;
}

void broadcast_message(int sockfd, char *message, struct sockaddr_in sender) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].sin_addr.s_addr != sender.sin_addr.s_addr ||
            clients[i].sin_port != sender.sin_port) {
            sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&clients[i], sizeof(clients[i]));
        }
    }
}

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Chat Server started on port %d\n", PORT);

    while (1) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
        buffer[n] = '\0';

        add_client(client_addr);
        printf("Received message: %s\n", buffer);

        broadcast_message(sockfd, buffer, client_addr);
    }

    close(sockfd);
    return 0;
}

