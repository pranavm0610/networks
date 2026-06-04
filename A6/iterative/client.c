#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>  // For close()


#define LOCAL_PORT 9999
#define ROOT_PORT 9998
#define TLD_PORT 9997
#define AUTH_PORT 9996
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char domain[BUFFER_SIZE], buffer[BUFFER_SIZE];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Define local DNS server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LOCAL_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Input domain name
    printf("Enter domain name to resolve: ");
    scanf("%s", domain);

    // Send domain to local DNS server
    sendto(sock, domain, strlen(domain), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Receive response from server
    int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, NULL, NULL);
    buffer[n] = '\0';

    printf("Resolved IP: %s\n", buffer);

    close(sock);

    return 0;
}
