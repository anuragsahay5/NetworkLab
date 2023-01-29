#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

char readMsg[80] = {0};
char buffer[80] = {0};
int socket_fd;

void *readFunc(void *data)

{
    while (1)
    {
        bzero(readMsg, 80);
        read(socket_fd, readMsg, 80);
        if (!(strcmp(readMsg, "exit")))
        {
            close(socket_fd);
            pthread_exit(NULL);
            exit(0);
        }
        printf("server : %s\n", readMsg);
    }
    pthread_exit(NULL);
}

int main()
{
    struct sockaddr_in server_addr;
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

    int status = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (status < 0)
    {
        perror("Connection with server Failed");
        exit(0);
    }

    pthread_t pid;
    pthread_create(&pid, NULL, readFunc, NULL);
    while (1)
    {
        bzero(buffer, 80);
        scanf("%s", buffer);
        write(socket_fd, buffer, strlen(buffer));
        if (!(strcmp(buffer, "exit")))
        {
            pthread_cancel(pid);
            break;
        }
    }

    close(socket_fd);
    pthread_exit(NULL);
    return 0;
}