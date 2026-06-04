#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ROOT_DNS_PORT 7000
#define TLD_DNS_IP "8000"  // TLD DNS server port
#define MAXLINE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLINE];
    int len, n;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up Root DNS server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(ROOT_DNS_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to Root DNS server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    len = sizeof(cliaddr);

    // Listen for queries
    while (1) {
        n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("Root DNS received query for domain: %s\n", buffer);

        // Respond with TLD DNS server IP
        sendto(sockfd, TLD_DNS_IP, strlen(TLD_DNS_IP), 0, (struct sockaddr *)&cliaddr, len);
        printf("Sent TLD DNS IP to Local DNS.\n");
    }

    close(sockfd);
    return 0;
}
