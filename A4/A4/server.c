#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    int socket_fd;
    struct sockaddr_in server_addr, client_addr;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0)
    {
        perror("Socket creation failed");
        exit(0);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    int status = bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (status < 0)
    {
        perror("Bind Failed");
        exit(0);
    }

    int client_addr_length = sizeof(client_addr);
    char msg[1310];
    short *packetid;
    while (1)
    {
        int rv = recvfrom(socket_fd, msg, 1310, MSG_WAITALL, (struct sockaddr *)&client_addr, &client_addr_length);
        if (!rv)
        {
            break;
        }
        msg[6]--;
        packetid = (short *)msg;
        printf("packet_id %hu recieved\n", packetid[0]);
        int sv = sendto(socket_fd, msg, sizeof(msg), MSG_CONFIRM, (const struct sockaddr *)&client_addr, sizeof(client_addr));
        if (!sv)
        {
            break;
        }
    }
    close(socket_fd);
    return 0;
}