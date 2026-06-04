#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define PORT 10303
#define SIZE 1024
#define IP "127.0.0.1"


struct domain{
	char name[SIZE];
	char ip[SIZE];
};

int main(){

	struct domain domains[] = {{"www.ssn.com","30.0.0.1"},{"www.google.com","30.0.0.2"},{"www.niranjan.com","30.0.0.3"}};
	int sockfd;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	socklen_t len = sizeof(addr);

	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	int opt1 = 1;

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt1,sizeof(opt1));
	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) < 0){
		perror("Binding Error");
	}

	char name[SIZE];
	char ip[SIZE];

	recvfrom(sockfd,name,sizeof(name),0,(struct sockaddr*)&addr,&len);


	for(int i = 0; i < 3; i++){
		if (strcmp(name,domains[i].name) == 0){
			strcpy(ip,domains[i].ip);
			break;
		}

	}

	sendto(sockfd,ip,sizeof(ip),0,(struct sockaddr*)&addr,len);

	printf("\nReceived Name: %s IP: %s",name,ip);

	close(sockfd);

}