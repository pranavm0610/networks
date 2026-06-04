#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>  // For close()

#define AUTH_PORT 8083
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in auth_addr, tld_addr;
    char buffer[BUFFER_SIZE], resolved_ip[BUFFER_SIZE];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Bind socket to authoritative DNS
    memset(&auth_addr, 0, sizeof(auth_addr));
    auth_addr.sin_family = AF_INET;
    auth_addr.sin_port = htons(AUTH_PORT);
    auth_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr*)&auth_addr, sizeof(auth_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Receive query from TLD DNS
    socklen_t len = sizeof(tld_addr);
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&tld_addr, &len);
    buffer[n] = '\0';
    printf("Received query for domain: %s\n", buffer);

    // Resolve and send IP back
    // Actual resolution for example.com -> 93.184.216.34
    if (strcmp(buffer, "example.com") == 0) {
        strcpy(resolved_ip, "93.184.216.34");
    } else if (strcmp(buffer, "google.com") == 0) {
        strcpy(resolved_ip, "12.345.678.90");
    } else {
        strcpy(resolved_ip, "0.0.0.0");  // Unknown domain
    }

    sendto(sock, resolved_ip, strlen(resolved_ip), 0, (struct sockaddr*)&tld_addr, len);
    close(sock);

    return 0;
}
