#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 1024
#define ROOT_SERVER_IP "127.0.0.1"
#define ROOT_SERVER_PORT 5000
#define LOCAL_PORT 6000

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, localaddr;
    int len, n;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Local address for the local DNS server
    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = INADDR_ANY;
    localaddr.sin_port = htons(LOCAL_PORT);

    // Bind the socket to the local address
    if (bind(sockfd, (const struct sockaddr *)&localaddr, sizeof(localaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Root server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(ROOT_SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(ROOT_SERVER_IP);

    // Query domain
    char domain[] = "example.com";
    sendto(sockfd, domain, strlen(domain), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Querying root server for domain: %s\n", domain);

    // Receive response from root server (TLD server IP)
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, NULL, NULL);
    buffer[n] = '\0';
    printf("Root server referred to TLD server at: %s\n", buffer);

    // Update servaddr to point to TLD server
    servaddr.sin_addr.s_addr = inet_addr(buffer);

    // Query TLD server
    sendto(sockfd, domain, strlen(domain), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Querying TLD server for domain: %s\n", domain);

    // Receive response from TLD server (Authoritative server IP)
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, NULL, NULL);
    buffer[n] = '\0';
    printf("TLD server referred to Authoritative server at: %s\n", buffer);

    // Update servaddr to point to Authoritative server
    servaddr.sin_addr.s_addr = inet_addr(buffer);

    // Query Authoritative server
    sendto(sockfd, domain, strlen(domain), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Querying Authoritative server for domain: %s\n", domain);

    // Receive response (IP address of domain)
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, NULL, NULL);
    buffer[n] = '\0';
    printf("Authoritative server returned IP address: %s\n", buffer);

    close(sockfd);
    return 0;
}
