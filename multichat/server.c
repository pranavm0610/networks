#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 9710
#define SIZE 1024
#define MAX_CLIENTS 5

struct sockaddr_in clients[MAX_CLIENTS];
int client_count = 0;


void check(struct sockaddr_in client_addr) {
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
    struct sockaddr_in addr, clieaddr;
    socklen_t len = sizeof(clieaddr);
    char msg[SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server address setup
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Set socket options
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Binding error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server running on port %d\n", PORT);

    // Main server loop
    while (1) {
        int n = recvfrom(sockfd, msg, SIZE - 1, 0, (struct sockaddr*)&clieaddr, &len);
        if (n > 0) {
            msg[n] = '\0';  // Null-terminate the received message
            check(clieaddr);  // Register new client if necessary
            printf("\nMessage received: %s\n", msg);
            broadcast_message(sockfd, msg, clieaddr);  // Broadcast to all other clients
        } else if (n < 0) {
            perror("Receive error");
        }
    }

    close(sockfd);
    return 0;
}
