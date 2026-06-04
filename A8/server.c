#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888

// Function to send an acknowledgment
void send_acknowledgment(int new_socket, int seq_num) {
    char ack[10];
    sprintf(ack, "ACK%d", seq_num);
    send(new_socket, ack, strlen(ack), 0);
    printf("Sending Acknowledgment: %s\n", ack);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept the connection from the client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Clear the buffer before reading
        memset(buffer, 0, sizeof(buffer));

        // Read the data sent by the client
        int valread = read(new_socket, buffer, sizeof(buffer));
        if (valread > 0) {
            printf("\nReceived: %s\n", buffer);

            // Extract sequence number from the packet (assuming the sequence number is in the correct position)
            char seq_num = buffer[6] - '0';

            // Send an acknowledgment for the received packet
            send_acknowledgment(new_socket, seq_num);
        } else if (valread == 0) {
            printf("\nConnection closed by client.\n");
            break;
        } else {
            perror("recv");
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
