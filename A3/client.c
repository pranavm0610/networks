#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    int sockfd, n;
    struct sockaddr_in servaddr;
    char buff[1024];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server-ip-address>\n", argv[0]);
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("cannot create socket");
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(7777);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connect error");
        close(sockfd);
        return 1;
    }

    // Sending the message
    while (1) {
        printf("Enter the message: ");
        if (fgets(buff, sizeof(buff), stdin) == NULL) {
            perror("Input error");
            close(sockfd);
            return 1;
        }
        buff[strcspn(buff, "\n")] = '\0'; // Remove newline

        n = write(sockfd, buff, strlen(buff) + 1);
        if (n < 0) {
            perror("Write error");
            close(sockfd);
            return 1;
        }

        if (strcmp(buff, "END") == 0) {
            break;
        }
    }

    close(sockfd);
    return 0;
}
