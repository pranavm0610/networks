#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>  // For close()


#define LOCAL_PORT 9999
#define ROOT_PORT 9998
#define TLD_PORT 9997
#define AUTH_PORT 9996
#define BUFFER_SIZE 1024

void query_tld_server(const char *domain, char *resolved_ip);

int main() {
    int sock;
    struct sockaddr_in root_addr, local_addr;
    char buffer[BUFFER_SIZE], resolved_ip[BUFFER_SIZE];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Bind socket to root DNS
    memset(&root_addr, 0, sizeof(root_addr));
    root_addr.sin_family = AF_INET;
    root_addr.sin_port = htons(ROOT_PORT);
    root_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr*)&root_addr, sizeof(root_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Receive query from local DNS
    socklen_t len = sizeof(local_addr);
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&local_addr, &len);
    buffer[n] = '\0';
    printf("Received query for domain: %s\n", buffer);
    // Send resolved IP back to client
    sendto(sock, resolved_ip, strlen(resolved_ip), 0, (struct sockaddr*)&local_addr, len);

    close(sock);
    return 0;
}
