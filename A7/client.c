#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX 1024
#define PORT 7676

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAX];
    char domain_name[50];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    socklen_t len = sizeof(servaddr);

    // Get domain name from user
    printf("Enter the server name: ");
    scanf("%s", domain_name);

    // Send domain name to server
    sendto(sockfd, domain_name, strlen(domain_name), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));

    // Receive IP address from server
    int n = recvfrom(sockfd, buffer, MAX, 0, (struct sockaddr*)&servaddr, &len);
    buffer[n] = '\0';

    printf("The IP addresses are: %s\n", buffer);

    close(sockfd);
    return 0;
}
