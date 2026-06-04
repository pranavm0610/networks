#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LOCAL_DNS_PORT 6000
#define ROOT_DNS_PORT 7000
#define MAXLINE 1024

int query_dns_server(char *domain, int server_port) {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAXLINE];

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Send query to DNS server
    sendto(sockfd, domain, strlen(domain), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    // Receive response from DNS server
    int len = sizeof(servaddr), n;
    n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';

    close(sockfd);

    // Return the resolved address
    return atoi(buffer);
}

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    int len, n;

    // Create UDP socket for Local DNS server
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up Local DNS server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(LOCAL_DNS_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to Local DNS server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    len = sizeof(cliaddr);

    // Listen for client queries
    while (1) {
        n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';

        printf("Received query for domain: %s\n", buffer);

        // Query the Root DNS server
        printf("Querying Root DNS server...\n");
        int tld_dns_ip = query_dns_server(buffer, ROOT_DNS_PORT);

        // Query the TLD DNS server
        printf("Querying TLD DNS server...\n");
        int auth_dns_ip = query_dns_server(buffer, tld_dns_ip);

        // Query the Authoritative DNS server for the final IP
        printf("Querying Authoritative DNS server...\n");
        int final_ip = query_dns_server(buffer, auth_dns_ip);

        // Send the resolved IP address back to the client
        sprintf(buffer, "%d", final_ip);
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&cliaddr, len);
        printf("Resolved IP sent to client: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
