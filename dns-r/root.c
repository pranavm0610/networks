#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PORTL 10103
#define PORTR 10203
#define SIZE 1024
#define IP "127.0.0.1"

int main(){
	int sockfd;

	struct sockaddr_in l_addr;

	l_addr.sin_family = AF_INET;
	l_addr.sin_port = htons(PORTL);
	l_addr.sin_addr.s_addr = INADDR_ANY;

	int opt1 = 1;
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt1,sizeof(opt1));

	socklen_t len = sizeof(l_addr);

	if(bind(sockfd,(struct sockaddr*)&l_addr,sizeof(l_addr)) < 0){
		perror("Binding Error");
	}


	int newfd;

	struct sockaddr_in r_addr;

	r_addr.sin_family = AF_INET;
	r_addr.sin_port = htons(PORTR);
	r_addr.sin_addr.s_addr = inet_addr(IP);

	int opt2 = 1;
	newfd = socket(AF_INET,SOCK_DGRAM,0);
	setsockopt(newfd,SOL_SOCKET,SO_REUSEADDR,&opt2,sizeof(opt2));


	char name[SIZE];
	char ip[SIZE];

	recvfrom(sockfd,name,sizeof(name),0,(struct sockaddr*)&l_addr,&len);

	sendto(newfd,name,sizeof(name),0,(struct sockaddr*)&r_addr,len);

	recvfrom(newfd,ip,sizeof(ip),0,(struct sockaddr*)&r_addr,&len);

	printf("\nReceived Name: %s IP: %s",name,ip);

	sendto(sockfd,ip,sizeof(ip),0,(struct sockaddr*)&l_addr,len);

	close(sockfd);
	close(newfd);
}