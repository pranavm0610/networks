#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024
#define PORT 42345

// Function to check if IP address is valid
int is_valid_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}

// Function to check if MAC address is valid
int is_valid_mac(const char *mac) {
    int i;
    if (strlen(mac) != 17) return 0;
    for (i = 0; i < 17; i++) {
        if ((i + 1) % 3 == 0) {
            if (mac[i] != '-') return 0;
        } else {
            if (!((mac[i] >= '0' && mac[i] <= '9') || (mac[i] >= 'A' && mac[i] <= 'F') || (mac[i] >= 'a' && mac[i] <= 'f'))) return 0;
        }
    }
    return 1;
}

void handle_client(int sockfd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char src_ip[16], src_mac[18], dest_ip[16], dest_mac[18];
    uint16_t data;
    ssize_t recv_len;

    while (1) {
        // Receive ARP Request
        recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }

        // Parse packet
        sscanf(buffer, "%15s %17s %15s %5hx", src_ip, src_mac, dest_ip, &data);

        printf("Received ARP Request:\n");
        printf("Source IP: %s\n", src_ip);
        printf("Source MAC: %s\n", src_mac);
        printf("Destination IP: %s\n", dest_ip);

        // Simulate ARP Request Handling
        if (strcmp(dest_ip, "155.157.65.128") == 0) {
            strcpy(dest_mac, "45-DA-62-21-1A-B2"); // Example MAC address

            // Prepare ARP Reply
            snprintf(buffer, sizeof(buffer), "%s %s %s %s %04X", src_ip, src_mac, dest_ip, dest_mac, data);
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);

            printf("ARP Reply Sent:\n");
            printf("Source IP: %s\n", src_ip);
            printf("Source MAC: %s\n", src_mac);
            printf("Destination IP: %s\n", dest_ip);
            printf("Destination MAC: %s\n", dest_mac);
            printf("Data: %04X\n", data);

            // Print and send the final packet to destination MAC
            snprintf(buffer, sizeof(buffer), "%s %s %s %s %04X", src_ip, src_mac, dest_ip, dest_mac, data);
            printf("Packet Sent: %s\n", buffer);
        } else {
            printf("Destination IP does not match.\n");
        }

        // Ask the user if they want to end the server
        char choice;
        printf("Do you want to end the server? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            break;
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for ARP requests...\n");

    handle_client(sockfd);

    close(sockfd);
    return 0;
}

