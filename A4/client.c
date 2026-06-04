#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(7777);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("Connection Failed\n");
        return -1;
    }

    char file_path[1024];
    printf("Enter the path of file: ");
    scanf("%s", file_path);
    send(sock, file_path, strlen(file_path), 0);
    printf("File Transferred.\n");
    printf("Save the file in path: ");
    char save_path[1024];
    scanf("%s", save_path);

    FILE *file = fopen(save_path, "wb");
    if (file == NULL) 
    {
        perror("Error creating file");
        close(sock);
        return -1;
    }

    while (1) 
    {
        int bytes_received = recv(sock, buffer, 1024, 0);
        if (bytes_received > 0) 
            fwrite(buffer, 1, bytes_received, file);

        if (bytes_received < 1024) 
        {
            if (feof(file)) 
                printf("File Transfer Completed.\n");
            if (ferror(file)) 
                perror("Error receiving the file");
            break;
        }
    }

    fclose(file);
    close(sock);
    return 0;
}