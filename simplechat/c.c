#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

void main() {
  struct sockaddr_in servaddr;
  int serverfd;
  char buffer[1024];
  
  serverfd = socket(AF_INET, SOCK_STREAM, 0);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);
  
  connect(serverfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  printf("Connected.\n");
  /*while (1) {
    memset(buffer, 0, sizeof(buffer));
    printf("Client: ");
    scanf("%s",buffer);
    send(serverfd, buffer, strlen(buffer),0);
    if (strcmp(buffer,"bye")==0) {
      printf("Closing connection.\n");
      break;
    }
    memset(buffer, 0, sizeof(buffer));
    int n = recv(serverfd, buffer, sizeof(buffer),0);
    buffer[n] = '\0';
    printf("Server: %s",buffer);
  }*/
  close(serverfd);
}
