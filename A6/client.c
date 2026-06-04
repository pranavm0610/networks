#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>

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

// Signal handler for SIGINT
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\nClient terminated.\n");
        exit(0);
    }
}

void client_program() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char client_ip[16], client_mac[18];
    char src_ip[16], src_mac[18], dest_ip[16], dest_mac[18];
    uint16_t data;
    socklen_t addr_len = sizeof(server_addr);

    // Set up signal handling
    signal(SIGINT, signal_handler);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Assuming server is running locally
    server_addr.sin_port = htons(PORT);

    printf("Enter your IP address: ");
    scanf("%15s", client_ip);
    if (!is_valid_ip(client_ip)) {
        printf("Invalid IP address.\n");
        close(sockfd);
        return;
    }

    printf("Enter your MAC address: ");
    scanf("%17s", client_mac);
    if (!is_valid_mac(client_mac)) {
        printf("Invalid MAC address.\n");
        close(sockfd);
        return;
    }

    // Prepare and send ARP Request
    snprintf(buffer, sizeof(buffer), "%s %s %s %04X", client_ip, client_mac, "155.157.65.128", 0xABCD);
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len) < 0) {
        perror("sendto failed");
        close(sockfd);
        return;
    }
    printf("ARP Request Sent.\n");

    // Receive ARP Reply
    ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&server_addr, &addr_len);
    if (recv_len < 0) {
        perror("recvfrom failed");
        close(sockfd);
        return;
    }

    buffer[recv_len] = '\0'; // Null-terminate the received data
    sscanf(buffer, "%15s %17s %15s %17s %5hx", src_ip, src_mac, dest_ip, dest_mac, &data);

    printf("ARP Request Received:\n");
    printf("Source IP: %s\n", src_ip);
    printf("Source MAC: %s\n", src_mac);
    printf("Destination IP: %s\n", dest_ip);

    if (strcmp(dest_ip, client_ip) == 0) {
        // Reply with ARP Response
        snprintf(buffer, sizeof(buffer), "%s %s %s %s %04X", src_ip, src_mac, dest_ip, client_mac, data);
        if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len) < 0) {
            perror("sendto failed");
            close(sockfd);
            return;
        }

        printf("ARP Reply Sent:\n");
        printf("Source IP: %s\n", src_ip);
        printf("Source MAC: %s\n", src_mac);
        printf("Destination IP: %s\n", dest_ip);
        printf("Destination MAC: %s\n", client_mac);
        printf("Data: %04X\n", data);

        // Receive packet from server
        recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom failed");
            close(sockfd);
            return;
        }

        buffer[recv_len] = '\0'; // Null-terminate the received data
        sscanf(buffer, "%15s %17s %15s %17s %5hx", src_ip, src_mac, dest_ip, dest_mac, &data);

        printf("Received Packet:\n");
        printf("Source IP: %s\n", src_ip);
        printf("Source MAC: %s\n", src_mac);
        printf("Destination IP: %s\n", dest_ip);
        printf("Destination MAC: %s\n", dest_mac);
        printf("Data: %04X\n", data);
    } else {
        printf("IP address does not match.\n");
    }

    close(sockfd);
}

int main() {
    client_program();
    return 0;
}

