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
  struct sockaddr_in servaddr, clientaddr;
  int serverfd, clientfd;
  socklen_t len = sizeof(clientaddr);
  char buffer[1024];
  bzero(&clientaddr,sizeof(clientaddr));
  serverfd = socket(AF_INET, SOCK_STREAM, 0);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);
  
  bind(serverfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  listen(serverfd,5);
  printf("hi");
  
  clientfd = accept(serverfd, (struct sockaddr *)&clientaddr, &len);
  printf("Client connected.");
  /*
  while (1) {
    memset(buffer, 0, sizeof(buffer));
    int n = recv(clientfd, buffer, sizeof(buffer),0);
    buffer[n] = '\0';
    printf("\nClient: %s",buffer);
    if (strcmp(buffer,"bye")==0) {
      printf("Closing connection.\n");
      break;
    }
    send(clientfd, buffer, strlen(buffer),0);
  }*/
  close(clientfd);
  close(serverfd);
  
}
