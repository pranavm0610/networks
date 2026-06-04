#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void checkAndCorrectHammingCode(int hammingData[], int m, int r);

int main() {
    int serverSock, newSock;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;

    // Create socket
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        printf("Socket creation failed.\n");
        return -1;
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to the port
    if (bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Bind failed.\n");
        return -1;
    }

    // Start listening for connections
    if (listen(serverSock, 5) < 0) {
        printf("Listen failed.\n");
        return -1;
    }

    printf("Server listening on port 8080...\n");

    // Accept incoming connection
    addr_size = sizeof(clientAddr);
    newSock = accept(serverSock, (struct sockaddr *)&clientAddr, &addr_size);
    if (newSock < 0) {
        printf("Connection accept failed.\n");
        return -1;
    }

    // Assume maximum size of data + redundant bits
    int m = 4; // Number of data bits (e.g., this can be dynamic depending on client)
    int r = 3; // Number of redundant bits
    int totalBits = m + r;
    int hammingData[totalBits]; // Adjust this size based on the received data

    // Receive Hamming code from client
    recv(newSock, hammingData, totalBits * sizeof(int), 0);
    printf("Received Hamming code from client.\n");

    // Check for errors and correct the data if necessary
    checkAndCorrectHammingCode(hammingData, m, r);

    // Close connection
    close(newSock);
    close(serverSock);
    return 0;
}

void checkAndCorrectHammingCode(int hammingData[], int m, int r) {
    int errorPos = 0, i, j;

    // Calculate the parity bits to detect error
    for (i = 0; i < r; i++) {
        int x = 1 << i;
        int parity = 0;
        for (j = x; j <= m + r; j += 2 * x) {
            for (int k = j; k < j + x && k <= m + r; k++) {
                parity ^= hammingData[k - 1];
            }
        }
        if (parity != 0)
            errorPos += x;
    }

    // If error detected, correct the error
    if (errorPos != 0) {
        printf("Error detected at position: %d\n", errorPos);
        hammingData[errorPos - 1] ^= 1; // Flip the bit to correct error
        printf("Corrected Hamming code: ");
    } else {
        printf("No errors found\n");
        printf("Hamming code: ");
    }

    // Display the corrected or unchanged Hamming code
    for (i = 0; i < m + r; i++) {
        printf("%d", hammingData[i]);
    }
    printf("\n");
}
