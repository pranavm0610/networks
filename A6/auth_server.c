#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define AUTH_DNS_PORT 9000
#define FINAL_IP "192.168.1.1"  // The actual IP of the domain
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

    // Set up Authoritative DNS server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(AUTH_DNS_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to Authoritative DNS server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    len = sizeof(cliaddr);

    // Listen for queries
    while (1) {
        n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("Authoritative DNS received query for domain: %s\n", buffer);

        // Respond with the final resolved IP address
        sendto(sockfd, FINAL_IP, strlen(FINAL_IP), 0, (struct sockaddr *)&cliaddr, len);
        printf("Sent resolved IP to Local DNS: %s\n", FINAL_IP);
    }

    close(sockfd);
    return 0;
}
