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
    int newfd, sockfd, n;
    struct sockaddr_in servaddr, cliaddr;
    char buff[1024];
    sockfd=socket(AF_INET,SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=INADDR_ANY;
    servaddr.sin_port=htons(8888);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connect error");
        close(sockfd);
        return 1;
    }

    while(1)
    {
        printf("Enter the message: ");
        if (fgets(buff, sizeof(buff), stdin) == NULL) {
            perror("Input error");
            close(sockfd);
            return 1;
        }
        buff[strcspn(buff,"\n")]='\0';
        n = write(sockfd, buff, strlen(buff) + 1);
        if (strcmp(buff, "END") == 0) 
            break;
    }
    close(sockfd);
    return 0;
}