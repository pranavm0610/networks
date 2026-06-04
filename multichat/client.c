#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 9710
#define IP "127.0.0.1"
#define SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    socklen_t len = sizeof(addr);

    int pid = fork();  // Create a child process for receiving messages

    if (pid == 0) {  // Child process for receiving messages
        char msg[SIZE];
        while (1) {
            int n = recvfrom(sockfd, msg, sizeof(msg) - 1, 0, (struct sockaddr*)&addr, &len);
            if (n > 0) {
                msg[n] = '\0';  // Null-terminate the received message
                printf("MESSAGE RECEIVED: %s\n", msg);
            }
        }
    } else {  // Parent process for sending messages
        char msg[SIZE];
        while (1) {
            fgets(msg, sizeof(msg), stdin);
            msg[strcspn(msg, "\n")] = '\0';  // Remove newline character
            sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&addr, len);
        }
    }

    close(sockfd);  // Close socket when done
    return 0;
}
