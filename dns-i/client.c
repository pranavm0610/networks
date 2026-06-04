#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 11200
#define IP "127.0.0.1"
#define SIZE 1024

int main()
{	
	int sockfd;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(IP);

	socklen_t len = sizeof(addr);
	int opt = 1;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	char name[] = "www.shaun.com";
	char ip[SIZE];
	
	sendto(sockfd,name,sizeof(name),0,(struct sockaddr*)&addr,len);

	recvfrom(sockfd,ip,sizeof(ip),0,(struct sockaddr*)&addr,&len);


	printf("Name : %s IP : %s",name,ip);
	return 0;
}