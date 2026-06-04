#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>  // For close()

#define LOCAL_PORT 9999
#define ROOT_PORT 9998
#define TLD_PORT 9997
#define AUTH_PORT 9996


#define BUFFER_SIZE 1024

void query_root_server(const char *domain, char *resolved_ip);
void query_tld_server(const char *domain, char *resolved_ip);
void query_tld_server(const char *domain, char *resolved_ip);
void query_authoritative_server(const char *domain, char *resolved_ip);

int main() {
    int sock;
    struct sockaddr_in local_addr, client_addr;
    char buffer[BUFFER_SIZE], resolved_ip[BUFFER_SIZE];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Bind socket to local DNS
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(LOCAL_PORT);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (const struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Receive domain from client
    socklen_t len = sizeof(client_addr);
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &len);
    buffer[n] = '\0';
    printf("Received query for domain: %s\n", buffer);

    // Query root DNS server
    query_root_server(buffer, resolved_ip);
    query_tld_server(buffer, resolved_ip);
    query_authoritative_server(buffer, resolved_ip);

    // Send resolved IP back to client
    sendto(sock, resolved_ip, strlen(resolved_ip), 0, (struct sockaddr*)&client_addr, len);

    close(sock);
    return 0;
}

void query_root_server(const char *domain, char *resolved_ip) {
    int sock;
    struct sockaddr_in root_addr;
    char buffer[BUFFER_SIZE];

    // Create socket for querying root server
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return;
    }

    // Define root server address
    memset(&root_addr, 0, sizeof(root_addr));
    root_addr.sin_family = AF_INET;
    root_addr.sin_port = htons(ROOT_PORT);
    root_addr.sin_addr.s_addr = INADDR_ANY;

    // Send domain query to root server
    sendto(sock, domain, strlen(domain), 0, (struct sockaddr*)&root_addr, sizeof(root_addr));

    // Receive the response (resolved IP)
    socklen_t len = sizeof(root_addr);
    
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&root_addr, &len);
    buffer[n] = '\0';

    // Copy the resolved IP back
    strcpy(resolved_ip, buffer);

    close(sock);
}


void query_tld_server(const char *domain, char *resolved_ip) {
    int sock;
    struct sockaddr_in tld_addr;
    char buffer[BUFFER_SIZE];

    // Create socket for querying TLD server
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return;
    }

    // Define TLD server address
    memset(&tld_addr, 0, sizeof(tld_addr));
    tld_addr.sin_family = AF_INET;
    tld_addr.sin_port = htons(TLD_PORT);
    tld_addr.sin_addr.s_addr = INADDR_ANY;

    // Send domain query to TLD server
    sendto(sock, domain, strlen(domain), 0, (struct sockaddr*)&tld_addr, sizeof(tld_addr));

    // Receive the response (resolved IP)
    socklen_t len = sizeof(tld_addr);
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&tld_addr, &len);
    buffer[n] = '\0';

    // Copy the resolved IP back
    strcpy(resolved_ip, buffer);

    close(sock);
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
