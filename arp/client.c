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
	addr1.sin_addr.s_addr = INADDR_ANY;


	int opt = 1;

	socklen_t len = sizeof(addr1);
	sockfd = socket(AF_INET,SOCK_DGRAM,0);


	char source_ip[bsize];
	char source_mac[bsize];
	char dest_ip[bsize];
	char dest_mac[bsize];
	char data[bsize];


	char client_ip[bsize];
	char client_mac[bsize];

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	if(bind(sockfd,(struct sockaddr*)&addr1,sizeof(addr1)) < 0){
		perror("Binding Error!");
	}

	printf("\nSource IP: ");
	scanf("%s",client_ip);
	printf("\nSource MAC: ");
	scanf("%s",client_mac);


	char packet[bsize];
	recvfrom(sockfd,packet,sizeof(packet) - 1,0,(struct sockaddr*)&addr1,&len);

	printf("\n\n%s",packet);

	sscanf(packet,"%[^|]|%[^|]|%[^|]|",source_ip,source_mac,dest_ip);

	printf("\n\n %s %s ",client_ip,dest_ip);


	if(strcmp(dest_ip,client_ip) == 0){
		printf("\nIP ADDRESS IS A MATCH!\n\n");
		strcat(packet,client_mac);
		strcat(packet,"|");

		int newfd;
		struct sockaddr_in addr2;

		addr2.sin_family = AF_INET;
		addr2.sin_port = htons(port);
		addr2.sin_addr.s_addr = inet_addr(source_ip);

		newfd = socket(AF_INET,SOCK_STREAM,0);

		if(connect(newfd,(struct sockaddr*)&addr2,sizeof(addr2)) < 0){
			perror("CONNECTION ERROR !");
		}
		send(newfd,packet,sizeof(packet),0);


		int n = recv(newfd,packet,sizeof(packet),0);
		packet[n] = '\0';

		sscanf(packet,"%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|",source_ip,source_mac,dest_ip,dest_mac,data);

		printf("\n\nDATA: %s",data);

		close(newfd);
	}
	else{
		printf("\nIP ADDRESS WRONG!");
	}

	close(sockfd);
	

}