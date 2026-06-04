#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORTL 12216
#define IP "127.0.0.1"
#define SIZE 1024
#define PORTR "12112"


int main(){
	int sockfd;

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORTL);
	addr.sin_addr.s_addr = INADDR_ANY;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	int a = 1;

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a));
	socklen_t len = sizeof(addr);

	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) < 0){
		perror("BINDINg ERROR!");
	}

	printf("IP : %s",PORTR);

	char name[SIZE];
	
	recvfrom(sockfd,name,sizeof(name),0,(struct sockaddr*)&addr,&len);


	sendto(sockfd,PORTR,sizeof(PORTR),0,(struct sockaddr*)&addr,len);

	printf("IP : %s",PORTR);
	close(sockfd);
}