#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    struct sockaddr_in server_addr;
    int socket_fd;
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

    char buffer[80] = "Hello, This is  Anurag Sahay";

    
        bzero(buffer, 80);
        write(socket_fd,buffer,strlen(buffer));
        printf("Sent to server : %s\n", buffer);
        read(socket_fd,buffer,80);
        printf("Recieved from server : %s\n", buffer);

    close(socket_fd);
    return 0;
}