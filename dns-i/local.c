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
#define PORT2 12216


char* query(char *name, int port){
	int newfd;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(IP);

	int opt = 1;

	newfd = socket(AF_INET,SOCK_DGRAM,0);
	socklen_t len = sizeof(addr);

	setsockopt(newfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	char ip[SIZE];

	sendto(newfd,name,sizeof(name),0,(struct sockaddr*)&addr,len);
	printf("\n%s : ",name);
	recvfrom(newfd,ip,sizeof(ip),0,(struct sockaddr*)&addr,&len);
	printf("\n%s : ",ip);

	close(newfd);
	return ip;


}

int main(){
	int sockfd;

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	socklen_t len = sizeof(addr);

	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) < 0){
		perror("BINDINg ERROR!");
	}

	char name[SIZE];

	recvfrom(sockfd,name,sizeof(name),0,(struct sockaddr*)&addr,&len);

	char TLD[SIZE];
	strcpy(TLD,query(name,PORT2));

	printf("TLD : %s",TLD);
	int tld  = atoi(TLD);
	printf("TLD port : %d",tld);

	char AUTH[SIZE];
	strcpy(AUTH,query(name,tld));

	int auth = atoi(AUTH);
	printf("AUTH port : %d",auth);

	char ip[SIZE];
	strcpy(ip,query(name,auth));
	printf("IP : %s",ip);

	sendto(sockfd,ip,sizeof(ip),0,(struct sockaddr*)&addr,len);

	close(sockfd);


}

