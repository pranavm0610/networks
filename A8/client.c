#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888

// Function to send a packet with flow control
int send_packet(int sock, char* packet, int seq_num) {
    char buffer[1024] = {0};

    printf("\nPacket%d: %s\n", seq_num, packet);
    char decision;
    printf("Send or Not (Y/N): ");
    scanf(" %c", &decision);

    if (decision == 'Y' || decision == 'y') {
        send(sock, packet, strlen(packet), 0);
        printf("Sent Packet%d\n", seq_num);
        read(sock, buffer, sizeof(buffer));
        printf("Received Acknowledgement: %s\n", buffer);
        return 1; // Packet was sent successfully
    } else {
        return 0; // Packet was not sent
    }
}

// Function to create a packet with sequence number, source, destination, and data
void create_packet(char* packet, char* src_addr, char* dest_addr, int seq_num, char* data) {
    sprintf(packet, "F-%s-%d-%s-FCS-F", dest_addr, seq_num, data);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char data[1024];
    char src_addr[10], dest_addr[10];
    int packets_sent[100] = {0};  // Keep track of sent packets, 0 = not sent, 1 = sent

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while (1) {
        printf("\nData: ");
        scanf("%s", data);

        int len = strlen(data);
        int total_packets = (len + 3) / 4; // Calculate number of 4-bit packets

        printf("\nPackets: ");
        for (int i = 0; i < len; i += 4) {
            for (int j = i; j < i + 4 && j < len; ++j) {
                printf("%c", data[j]);
            }
            printf(" ");
        }
        printf("\n%d packets\n", total_packets);

        printf("Enter source address: ");
        scanf("%s", src_addr);

        printf("Enter destination address: ");
        scanf("%s", dest_addr);

        // Send packets
        for (int i = 0; i < total_packets; i++) {
            if (packets_sent[i] == 0) {  // Only send packets that haven't been sent
                char packet[1024] = {0};
                char chunk[5] = {0};

                strncpy(chunk, &data[i * 4], 4); // Extract 4-bit chunk
                create_packet(packet, src_addr, dest_addr, i, chunk);

                if (send_packet(sock, packet, i)) {
                    packets_sent[i] = 1;  // Mark packet as sent
                }
            }
        }

        // Retransmit unsent packets if any
        for (int i = 0; i < total_packets; i++) {
            if (packets_sent[i] == 0) {  // Check for unsent packets
                printf("\nRetransmitting:\n");
                char packet[1024] = {0};
                char chunk[5] = {0};

                strncpy(chunk, &data[i * 4], 4); // Extract 4-bit chunk
                create_packet(packet, src_addr, dest_addr, i, chunk);

                send_packet(sock, packet, i);  // Attempt to resend
            }
        }

        char end_transmission[5];
        printf("\nEnd Transmission (Yes/No): ");
        scanf("%s", end_transmission);

        if (strcmp(end_transmission, "Yes") == 0 || strcmp(end_transmission, "yes") == 0) {
            close(sock);
            printf("Transmission Ended\n");
            break;
        }

        // Clear buffer for new input
        memset(data, 0, sizeof(data));
        memset(packets_sent, 0, sizeof(packets_sent));  // Reset sent packets tracking
    }

    return 0;
}
