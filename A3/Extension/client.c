#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

short msgLen;
char temp;
char msgType;
int msgSize;
char msg[600];
int socket_fd;
pthread_t pid;

void readResponse()
{
    int j;
    while (1)
    {
        int rn = read(socket_fd, msg, 600);
        if (!rn)
        {
            pthread_exit(0);
            close(socket_fd);
            printf("Server is Down\n");
            exit(0);
        }

        printf("Recieved data : ");
        j = 5;
        if (msgType == '1')
        {
            for (int i = 0; i < msgLen; i++)
            {
                printf("%c ", msg[j]);
                j++;
            }
        }
        else if (msgType == '2')
        {
            for (int i = 0; i < msgLen; i++)
            {
                printf("%hi ", msg[j]);
                j += 2;
            }
        }

        else if (msgType == '4')
        {
            for (int i = 0; i < msgLen; i++)
            {
                printf("%d ", msg[j]);
                j += 4;
            }
        }
        printf("\n\n\n");
    }
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

    printf("Connected to Server Successfully\n\n");

    pthread_create(&pid, NULL, readResponse, NULL);

    printf("Message Types\n");
    printf("char : 1\n");
    printf("short : 2\n");
    printf("int : 4\n\n");

    while (1)
    {

        /* Sending Response to Server*/
        printf("Length : ");
        scanf("%hi", &msgLen);
        if (msgLen == 0)
        {
            pthread_cancel(pid);
            break;
        }
        printf("Type : ");
        scanf(" %c", &msgType);

        if (msgType == '1')
        {
            msgSize = sizeof(char);
        }
        else if (msgType == '2')
        {
            msgSize = sizeof(short);
        }
        else if (msgType == '4')
        {
            msgSize = sizeof(int);
        }

        bzero(msg, 600);
        msg[0] = '0';
        memcpy(&msg[1], &msgLen, sizeof(short));
        msg[3] = '1';
        msg[4] = msgType;

        int j = 5;
        printf("Data : ");
        if (msgType == '1')
        {
            for (int i = 0; i < msgLen; i++)
            {
                scanf(" %c", &msg[j]);
                j++;
            }
        }
        else if (msgType == '2')
        {
            short inp;
            for (int i = 0; i < msgLen; i++)
            {
                scanf("%hi", &inp);
                memcpy(&msg[j], &inp, sizeof(short));
                j += 2;
            }
        }
        else if (msgType == '4')
        {
            int inp;
            for (int i = 0; i < msgLen; i++)
            {
                scanf("%d", &inp);
                memcpy(&msg[j], &inp, sizeof(int));
                j += 4;
            }
        }

        int sn = write(socket_fd, msg, 600);
        if (sn)
        {
            printf("Request Sent\n");
        }
        sleep(1);
    }

    close(socket_fd);
    printf("Disconnected Succesfully\n");
    return 0;
}