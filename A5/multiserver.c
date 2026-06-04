#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#define PORT 8080
#define MAXLINE 1024
#define MAX_CLIENTS 4

int main() {
    int server_fd, new_socket, client_socket[MAX_CLIENTS], max_clients = MAX_CLIENTS, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    char buffer[MAXLINE];
    fd_set readfds;

    for (i = 0; i < max_clients; i++) 
        client_socket[i] = 0;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) 
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connections...\n");

    while(1) 
    {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;
        for (i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) 
            printf("select error");

        if (FD_ISSET(server_fd, &readfds)) 
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address)) < 0) 
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is : %s, port : %d \n",new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            for (i = 0; i < max_clients; i++) 
            {
                if (client_socket[i] == 0) 
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        for (i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) 
            {
                if ((valread = read(sd, buffer, MAXLINE)) == 0) 
                {
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&address);
                    printf("Host disconnected, ip %s, port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    close(sd);
                    client_socket[i] = 0;
                } 
                else 
                {
                    buffer[valread] = '\0';
                    printf("Client %d: %s\n", i + 1, buffer);
                    for (int j = 0; j < max_clients; j++) 
                    {
                        if (client_socket[j] != 0 && j != i) 
                            send(client_socket[j], buffer, strlen(buffer), 0);
                    }

                    if (strcmp(buffer, "BYE") == 0) 
                    {
                        printf("Chat terminated by client %d\n", i + 1);
                        close(sd);
                        client_socket[i] = 0;
                    }
                }
            }
        }
    }
    return 0;
}