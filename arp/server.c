#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define Broad_port 7109
#define port 12662
#define bsize 1024
#define broad_ip "255.255.255.255"

int main(){
	int sockfd;

	struct sockaddr_in addr1;

	addr1.sin_family = AF_INET;
	addr1.sin_port = htons(Broad_port);
	addr1.sin_addr.s_addr = inet_addr(broad_ip);


	socklen_t len = sizeof(addr1);
	sockfd = socket(AF_INET,SOCK_DGRAM,0);


	char source_ip[bsize];
	char source_mac[bsize];
	char dest_ip[bsize];
	char dest_mac[bsize];
	char data[bsize];

	int opt = 1;

	setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&opt,sizeof(opt));



	printf("\nSource IP: ");
	scanf("%s",source_ip);
	printf("\nSource MAC: ");
	scanf("%s",source_mac);
	printf("\nDEST IP: ");
	scanf("%s",dest_ip);
	printf("\nData: ");
	scanf("%s",data);


	char packet[bsize];

	snprintf(packet,sizeof(packet),"%s|%s|%s|",source_ip,source_mac,dest_ip);

	sendto(sockfd,packet,sizeof(packet),0,(struct sockaddr*)&addr1,len);


	int newfd,cliefd;
	struct sockaddr_in addr2,clieaddr;

	addr2.sin_family = AF_INET;
	addr2.sin_port = htons(port);
	addr2.sin_addr.s_addr = INADDR_ANY;

	newfd = socket(AF_INET,SOCK_STREAM,0);

	if(bind(newfd,(struct sockaddr*)&addr2,sizeof(addr2)) < 0){
		perror("OTHER BIND ERROR!");
	}


	listen(newfd,3);

	cliefd = accept(newfd,(struct sockaddr*)&clieaddr,&len);

	recv(cliefd,packet,sizeof(packet),0);

	strcat(packet,data);
	strcat(packet,"|");

	send(cliefd,packet,sizeof(packet),0);

	close(sockfd);
	close(newfd);

}