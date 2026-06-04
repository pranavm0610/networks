#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORTL 12112
#define IP "127.0.0.1"
#define SIZE 1024
#define PORTR "13112"


int main(){
	int sockfd;

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORTL);
	addr.sin_addr.s_addr = INADDR_ANY;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	socklen_t len = sizeof(addr);

	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) < 0){
		perror("BINDINg ERROR!");
	}

	char name[SIZE];

	recvfrom(sockfd,name,sizeof(name),0,(struct sockaddr*)&addr,&len);


	sendto(sockfd,PORTR,sizeof(PORTR),0,(struct sockaddr*)&addr,len);

	close(sockfd);
}