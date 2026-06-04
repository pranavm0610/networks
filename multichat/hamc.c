#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int sockfd, m, r, x, i, j;
    struct sockaddr_in servaddr;
    char buff[1024];
    
    // Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Set up server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(7228);
    
    // Connect to server
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    
    // Get message from user
    printf("Enter the message: ");
    scanf("%s", buff);
    m = strlen(buff);
    
    // Calculate the required number of parity bits
    for (i = 1; i < m; i++) {
        if ((1 << i) > m + i + 1) {
            r = i;
            break;
        }
    }
    
    int data[m + r];
    x = m - 1;
    j = 0;
    
    // Fill data array with message bits, reserving space for parity bits
    for (i = 0; i < m + r; i++) {
        if ((i + 1) == (1 << j)) {
            data[i] = 0;
            j++;
        } else {
            data[i] = buff[x--] - '0';
        }
    }
    
    // Calculate parity bits
    for (i = 0; i < r; i++) {
        int pos = 1 << i;
        for (j = 1; j < m + r + 1; j++) {
            if (j & pos) {
                data[pos - 1] ^= data[j - 1];
            }
        }
    }
    
    // Prepare data for transmission
    char newbuff[m + r + 1];
    for (i = 0; i < m + r; i++) {
        newbuff[i] = data[m + r - 1 - i] + '0';
    }
    newbuff[m + r] = '\0';
    
    // Send data to server
    strcpy(buff, newbuff);
    send(sockfd, buff, sizeof(buff), 0);
    printf("Message sent: %s\n", newbuff);
    
    // Close socket
    close(sockfd);
    return 0;
}