#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    socklen_t len;
    int sockfd, newfd, n;
    struct sockaddr_in servaddr, cliaddr;
    char buff[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("cannot create socket");
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(7777);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind error");
        return 1;
    }

    listen(sockfd, 2);
    len = sizeof(cliaddr);

    // Receiving messages
    while (1) {
        newfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);
        if (newfd < 0) {
            perror("Accept error");
            return 1;
        }

        while (1) {
            n = read(newfd, buff, sizeof(buff) - 1);
            if (n < 0) {
                perror("Read error");
                close(newfd);
                return 1;
            } else if (n == 0) {
                // Connection closed by client
                break;
            }

            buff[n] = '\0'; // Null-terminate the string
            printf("Received Message: %s\n", buff);

            if (strcmp(buff, "END") == 0) {
                close(newfd);
                break;
            }
        }
        if (strcmp(buff, "END") == 0) {
            break;
        }
    }

    close(sockfd);
    return 0;
}
