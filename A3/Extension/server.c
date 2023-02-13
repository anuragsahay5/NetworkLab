#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define cSize 4
const int clientSize = 4;
int socket_client[cSize];
fd_set readSock;

void sort(char *msg, short msgLen, char msgType)
{
    if (msgType == '1')
    {
        int j = 5;
        char arr[msgLen];
        for (int i = 0; i < msgLen; i++)
        {
            arr[i] = msg[j];
            j++;
        }

        char key;
        for (int i = 1; i < msgLen; i++)
        {
            key = arr[i];
            j = i - 1;

            while (j >= 0 && arr[j] > key)
            {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }
        j = 5;
        for (int i = 0; i < msgLen; i++)
        {
            msg[j] = arr[i];
            j++;
        }
    }

    else if (msgType == '2')
    {
        int j = 5;
        short arr[msgLen];
        for (int i = 0; i < msgLen; i++)
        {
            memcpy(&arr[i], &msg[j], sizeof(short));
            j += 2;
        }

        short key;
        for (int i = 1; i < msgLen; i++)
        {
            key = arr[i];
            j = i - 1;

            while (j >= 0 && arr[j] > key)
            {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }

        j = 5;
        for (int i = 0; i < msgLen; i++)
        {
            memcpy(&msg[j], &arr[i], sizeof(short));
            j += 2;
        }
    }

    else if (msgType == '4')
    {
        int j = 5;
        int arr[msgLen];
        for (int i = 0; i < msgLen; i++)
        {
            memcpy(&arr[i], &msg[j], sizeof(int));
            j += 4;
        }

        int key;
        for (int i = 1; i < msgLen; i++)
        {
            key = arr[i];
            j = i - 1;

            while (j >= 0 && arr[j] > key)
            {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }

        j = 5;
        for (int i = 0; i < msgLen; i++)
        {
            memcpy(&msg[j], &arr[i], sizeof(int));
            j += 4;
        }
    }
}

int main()
{
    int socket_fd;
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
    bzero(&readSock, sizeof(readSock));
    for (int i = 0; i < clientSize; i++)
    {
        bzero(&client_addr, sizeof(client_addr));
        socket_client[i] = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_length);
        FD_SET(socket_client[i], &readSock);
        if (socket_client[i] < 0)
        {
            perror("Cannot establish connection with client");
            exit(0);
        }
    }
    printf("All Clients connected successfully\n\n");

    char msg[600];
    while (1)
    {
        for (int i = 0; i < clientSize; i++)
        {
            if (FD_ISSET(socket_client[i], &readSock))
            {
                bzero(msg, 600);
                int rn = read(socket_client[i], msg, 600);
                if (rn)
                {
                    printf("Request recieved\n");
                    printf("Recieved data : ");
                }

                short msgLen;
                char msgType;

                memcpy(&msgLen, &msg[1], sizeof(short));
                msgType = msg[4];

                if (msgType == '1')
                {
                    int j = 5;
                    for (int i = 0; i < msgLen; i++)
                    {
                        printf("%c ", msg[j]);
                        j++;
                    }
                    sort(&msg[0], msgLen, msgType);
                }
                else if (msgType == '2')
                {
                    int j = 5;
                    for (int i = 0; i < msgLen; i++)
                    {
                        printf("%hi ", msg[j]);
                        j += 2;
                    }
                    sort(&msg[0], msgLen, msgType);
                }

                else if (msgType == '4')
                {
                    int j = 5;
                    for (int i = 0; i < msgLen; i++)
                    {
                        printf("%d ", msg[j]);
                        j += 4;
                    }
                    sort(&msg[0], msgLen, msgType);
                }
                int sn = write(socket_client[i], msg, 600);

                if (sn)
                {
                    printf("\nResponse sent Successfully\n\n");
                }
            }
        }
    }

    close(socket_fd);
    close(socket_client);
    return 0;
}