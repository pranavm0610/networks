#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void generateHammingCode(int data[], int m, int r, int hammingCode[]);

int main() {
    int sock;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    // Get data bits from the user
    int m;
    printf("Enter the number of data bits: ");
    scanf("%d", &m);

    int data[m];
    printf("Enter the data bits (0 or 1):\n");
    for (int i = 0; i < m; i++) {
        printf("Bit %d: ", i + 1);
        scanf("%d", &data[i]);
    }

    // Calculate number of redundant bits
    int r = 0;
    while ((1 << r) < (m + r + 1)) {
        r++;
    }

    int totalBits = m + r;
    int hammingCode[totalBits];

    // Generate Hamming code
    generateHammingCode(data, m, r, hammingCode);

    printf("Hamming code: ");
    for (int i = 0; i < totalBits; i++) {
        printf("%d", hammingCode[i]);
    }
    printf("\n");

    // Send Hamming code to server
    send(sock, hammingCode, totalBits * sizeof(int), 0);
    printf("Hamming code sent to server.\n");

    // Close the socket
    close(sock);
    return 0;
}

void generateHammingCode(int data[], int m, int r, int hammingCode[]) {
    int totalBits = m + r;
    int dataPos = 0;
    int parityCount = 0;

    // Initialize hammingCode array with data and parity bits
    for (int i = 0; i < totalBits; i++) {
        // Check if current position is power of 2 (parity bit position)
        if ((i + 1) == (1 << parityCount)) {
            hammingCode[i] = 0;  // Initialize parity bits to 0
            parityCount++;
        } else {
            hammingCode[i] = data[dataPos];  // Place data bits
            dataPos++;
        }
    }

    // Calculate parity bits
    for (int i = 0; i < r; i++) {
        int parityPos = (1 << i);  // Position of current parity bit
        int parityValue = 0;

        // Calculate parity for current position
        for (int j = parityPos; j <= totalBits; j += 2 * parityPos) {
            for (int k = j; k < j + parityPos && k <= totalBits; k++) {
                parityValue ^= hammingCode[k - 1];  // XOR the data bits covered by this parity bit
            }
        }

        hammingCode[parityPos - 1] = parityValue;  // Set the parity bit
    }
}
