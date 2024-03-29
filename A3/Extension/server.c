#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define cSize 30
int clientSize;
int socket_client[cSize];
fd_set readSock, tempreadSock;
short msgLen;
char msgType;
int mxn;

void sort(char *msg, short msgLen, char msgType)
{
    int j;
    if (msgType == '1')
    {
        j = 5;
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
        j = 5;
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
        j = 5;
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
        int oup;
        for (int i = 0; i < msgLen; i++)
        {
            memcpy(&msg[j], &arr[i], sizeof(int));
            j += 4;
        }
    }
}

int main(int argc, char *argv[])
{
    int socket_fd;
    clientSize = atoi(argv[2]);
    struct sockaddr_in server_addr, client_addr;
    struct timeval tv;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("Socket creation failed");
        exit(0);
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int status = bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (status < 0)
    {
        perror("Bind Failed");
        exit(0);
    }

    status = listen(socket_fd, cSize);
    if (status < 0)
    {
        perror("Listening Failed");
        exit(0);
    }

    int client_addr_length = sizeof(client_addr);
    FD_ZERO(&readSock);

    for (int i = 0; i < clientSize; i++)
    {
        bzero(&client_addr, sizeof(client_addr));
        socket_client[i] = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_length);
        FD_SET(socket_client[i], &readSock);
        mxn = mxn > socket_client[i] ? mxn : socket_client[i];
        if (socket_client[i] < 0)
        {
            perror("Cannot establish connection with client");
            exit(0);
        }
    }
    printf("All Clients connected successfully\n\n");

    char msg[600];
    tempreadSock = readSock;
    while (1)
    {
        if (select(mxn + 1, &readSock, NULL, NULL, NULL) > 0)
        {
            for (int i = 0; i < clientSize; i++)
            {
                if (FD_ISSET(socket_client[i], &readSock))
                {
                    bzero(msg, 600);
                    int j = 5;
                    int rn = read(socket_client[i], msg, 600);
                    if (rn)
                    {
                        printf("Request recieved\n");
                        printf("Recieved data : ");
                    }

                    memcpy(&msgLen, &msg[1], sizeof(short));
                    msgType = msg[4];

                    if (msgType == '1')
                    {
                        for (int i = 0; i < msgLen; i++)
                        {
                            printf("%c ", msg[j]);
                            j++;
                        }
                        sort(&msg[0], msgLen, msgType);
                    }
                    else if (msgType == '2')
                    {
                        short oup;
                        for (int i = 0; i < msgLen; i++)
                        {
                            memcpy(&oup, &msg[j], sizeof(short));
                            printf("%hi ", oup);
                            j += 2;
                        }
                        sort(&msg[0], msgLen, msgType);
                    }

                    else if (msgType == '4')
                    {
                        int oup;
                        for (int i = 0; i < msgLen; i++)
                        {
                            memcpy(&oup, &msg[j], sizeof(int));
                            printf("%d ", oup);
                            j += 4;
                        }
                        sort(&msg[0], msgLen, msgType);
                    }
                    int sn = write(socket_client[i], msg, 600);

                    if (sn)
                    {
                        printf("\nResponse sent Successfully\n\n");
                    }
                    fflush(stdout);
                }
            }
        }
        readSock = tempreadSock;
    }
    close(socket_fd);
    close(socket_client);
    return 0;
}