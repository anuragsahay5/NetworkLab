#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void decode(char *str)
{
    int len = strlen(str);
    for (int i = 2; i < len; i++)
    {
        str[i - 2] = str[i];
    }
    str[len - 2] = '\0';
}

#define clientSize 3

int idx = 0;

char readMsg[80] = {0};
char buffer[80] = {0};
char inp[80] = {0};
char oup[80] = {0};

int socket_fd;
int socket_client[clientSize];

pthread_t pid[clientSize];

void *readFunc(void *data)
{
    int sc = idx;
    int sa;
    while (1)
    {
        bzero(readMsg, 80);
        bzero(oup, 80);
        int r = read(socket_client[sc], readMsg, 80);
        if (!r)
        {
            break;
        }
        sa = atoi(readMsg);
        if (sa > clientSize)
        {
            continue;
        }
        decode(readMsg);
        strcpy(oup, "Client ");
        oup[7] = (char)(49 + sc);
        oup[8] = '\0';
        strcat(oup, " : ");
        strcat(oup, readMsg);
        if (!sa)
        {
            printf("%s\n", oup);
        }
        else
        {
            write(socket_client[sa - 1], oup, strlen(oup));
        }
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

    status = listen(socket_fd, clientSize);
    if (status < 0)
    {
        perror("Listening Failed");
        exit(0);
    }

    int client_addr_length = sizeof(client_addr);

    bzero(&client_addr, sizeof(client_addr));

    for (int i = 0; i < clientSize; i++)
    {

        socket_client[i] = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_length);
        if (socket_client[i] < 0)
        {
            perror("Cannot establish connection with client");
            exit(0);
        }
        idx = i;

        bzero(buffer, 80);
        sprintf(buffer, "%d", i + 1);

        write(socket_client[i], buffer, strlen(buffer));
        pthread_create(&pid[i], NULL, readFunc, NULL);
    }

    printf("All clients Connected Successfully.\n");

    while (1)
    {
        bzero(buffer, 80);
        bzero(inp, 80);
        scanf("%[^\n]%*c", inp);
        strcpy(buffer, "Server : ");
        strcat(buffer, inp);

        for (int i = 0; i < clientSize; i++)
        {
            write(socket_client[i], buffer, strlen(buffer));
        }
        if (!(strcmp(buffer, "Server : exit")))
        {
            for (int i = 0; i < clientSize; i++)
            {
                pthread_cancel(pid[i]);
            }
            break;
        }
    }
    close(socket_fd);
    close(socket_client);
    printf("Chat Terminated...\n");
    return 0;
}