#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

char readMsg[80] = {0};
char buffer[80] = {0};
int socket_fd, socket_client;
pthread_t pid;

void *readFunc(void *data)
{
    while (1)
    {
        bzero(readMsg, 80);
        read(socket_client, readMsg, 80);
        printf("client : %s\n", readMsg);
    }
    pthread_exit(NULL);
}

int main()
{

    struct sockaddr_in server_addr, client_addr;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("Socket creation failed");
        exit(0);
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int status = bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (status < 0)
    {
        perror("Bind Failed");
        exit(0);
    }

    status = listen(socket_fd, 1);
    if (status < 0)
    {
        perror("Listening Failed");
        exit(0);
    }

    int client_addr_length = sizeof(client_addr);

    bzero(&client_addr, sizeof(client_addr));
    socket_client = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_length);
    if (socket_client < 0)
    {
        perror("Cannot establish connection with client");
        exit(0);
    }

    pthread_create(&pid, NULL, readFunc, NULL);

    while (1)
    {
        bzero(buffer, 80);
        scanf("%s", buffer);
        write(socket_client, buffer, strlen(buffer));
        if (!(strcmp(buffer, "exit")))
        {
            pthread_cancel(pid);
            break;
        }
    }
    close(socket_fd);
    close(socket_client);
    printf("Chat Terminated...\n");
    return 0;
}