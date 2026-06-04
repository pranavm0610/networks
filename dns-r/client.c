#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PORT 10003
#define SIZE 1024
#define IP "127.0.0.1"

int main()
{
	int sockfd;

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(IP);

	socklen_t len = sizeof(addr);


	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	//char name[] = "www.google.com";
	char name[SIZE];
	char ip[SIZE];

	printf("\nEnter Name: ");
	scanf("%s",name);

	sendto(sockfd,name,sizeof(name),0,(struct sockaddr*)&addr,len);

	recvfrom(sockfd,ip,sizeof(ip),0,(struct sockaddr*)&addr,&len);


	printf("\nReceived Name: %s IP: %s",name,ip);
	close(sockfd);

}