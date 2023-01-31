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
pthread_t pid;
int cid;

void *writeFunc(void *data)

{

    while (1)
    {
        bzero(buffer, 80);
        scanf("%[^\n]%*c", buffer);
        if (!(strcmp(buffer, "exit")))
        {
            exit(0);
        }
        write(socket_fd, buffer, strlen(buffer));
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
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
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = inet_addr("10.2.1.49");

    int status = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (status < 0)
    {
        perror("Connection with server Failed");
        exit(0);
    }

    read(socket_fd, readMsg, 80);
    printf("Your Id is %s. Use 0 to send msg to Server\n", readMsg);
    pthread_create(&pid, NULL, writeFunc, NULL);

    while (1)
    {
        bzero(readMsg, 80);
        int r = read(socket_fd, readMsg, 80);
        if (!r)
        {
            pthread_cancel(pid);
            break;
        }
        if (!(strcmp(readMsg, "Server : exit")))
        {
            pthread_cancel(pid);
            break;
        }
        printf("%s\n", readMsg);
    }

    close(socket_fd);
    printf("Chat Terminated...\n");
    return 0;
}