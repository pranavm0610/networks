#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cliaddr;
    char buff[1024];

    // Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Set up server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(7228);
    
    // Bind and listen
    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(sockfd, 5);

    printf("Server listening on port 7228...\n");

    // Accept client connection
    len = sizeof(cliaddr);
    connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);
    
    // Receive data
    recv(connfd, buff, sizeof(buff), 0);
    printf("Received message: %s\n", buff);

    // Convert received data to integer array
    int m_r = strlen(buff);
    int data[m_r];
    for (int i = 0; i < m_r; i++) {
        data[i] = buff[m_r - 1 - i] - '0';
    }

    // Error detection and correction
    int r = 0;
    while ((1 << r) < m_r + 1) r++;
    int error_pos = 0;

    for (int i = 0; i < r; i++) {
        int pos = 1 << i;
        int parity = 0;

        for (int j = 1; j < m_r + 1; j++) {
            if (j & pos) {
                parity ^= data[j - 1];
            }
        }

        if (parity) {
            error_pos += pos;
        }
    }

    // Display corrected message
    if (error_pos) {
        printf("Error detected at position %d. Correcting...\n", error_pos);
        data[error_pos - 1] ^= 1;
    } else {
        printf("No error detected.\n");
    }

    printf("Corrected message: ");
    for (int i = m_r - 1; i >= 0; i--) {
        printf("%d", data[i]);
    }
    printf("\n");

    // Close connections
    close(connfd);
    close(sockfd);
    return 0;
}