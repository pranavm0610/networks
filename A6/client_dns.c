#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LOCAL_DNS_PORT 6000
#define MAXLINE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAXLINE];
    char domain[MAXLINE];

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Local DNS server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(LOCAL_DNS_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Get domain name from user
    printf("Enter domain name: ");
    scanf("%s", domain);

    // Send domain name to the Local DNS server
    sendto(sockfd, domain, strlen(domain), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Query sent to Local DNS server.\n");

    // Receive resolved IP address from the Local DNS server
    int len = sizeof(servaddr), n;
    n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';

    printf("Resolved IP address: %s\n", buffer);

    close(sockfd);
    return 0;
}
