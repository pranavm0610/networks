#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>  // For close()

#define TLD_PORT 8082
#define AUTH_PORT 8083
#define BUFFER_SIZE 1024

void query_authoritative_server(const char *domain, char *resolved_ip);

int main() {
    int sock;
    struct sockaddr_in tld_addr, root_addr;
    char buffer[BUFFER_SIZE], resolved_ip[BUFFER_SIZE];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Bind socket to TLD DNS
    memset(&tld_addr, 0, sizeof(tld_addr));
    tld_addr.sin_family = AF_INET;
    tld_addr.sin_port = htons(TLD_PORT);
    tld_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr*)&tld_addr, sizeof(tld_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Receive query from root DNS
    socklen_t len = sizeof(root_addr);
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&root_addr, &len);
    buffer[n] = '\0';
    printf("Received query for domain: %s\n", buffer);

    // Query authoritative server
    query_authoritative_server(buffer, resolved_ip);

    // Send resolved IP back to root DNS
    sendto(sock, resolved_ip, strlen(resolved_ip), 0, (struct sockaddr*)&root_addr, len);

    close(sock);
    return 0;
}

void query_authoritative_server(const char *domain, char *resolved_ip) {
    int sock;
    struct sockaddr_in auth_addr;
    char buffer[BUFFER_SIZE];
    
    // Create socket for querying authoritative server
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return;
    }

    // Define authoritative server address
    memset(&auth_addr, 0, sizeof(auth_addr));
    auth_addr.sin_family = AF_INET;
    auth_addr.sin_port = htons(AUTH_PORT);
    auth_addr.sin_addr.s_addr = INADDR_ANY;

    // Send domain query to authoritative server
    sendto(sock, domain, strlen(domain), 0, (struct sockaddr*)&auth_addr, sizeof(auth_addr));

    // Receive the response (resolved IP)
    socklen_t len = sizeof(auth_addr);
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&auth_addr, &len);
    buffer[n] = '\0';

    // Copy the resolved IP back
    strcpy(resolved_ip, buffer);

    close(sock);
}
