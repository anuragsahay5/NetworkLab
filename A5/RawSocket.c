#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

int main()
{
    // Create a raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    // Prepare the destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Prepare the IP header
    char packet[4096];
    char packet2[4096];
    memset(packet, 0, sizeof(packet));
    memset(packet2, 0, sizeof(packet2));
    struct iphdr *ip_header = (struct iphdr *)packet;
    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tot_len = sizeof(struct iphdr);
    ip_header->id = htons(1234);
    ip_header->ttl = 64;
    ip_header->protocol = IPPROTO_RAW;
    ip_header->saddr = inet_addr("195.0.0.2");
    ip_header->daddr = dest_addr.sin_addr.s_addr;
    ip_header->check = 0; // Set to zero for now

    // Send the packet
    if (sendto(sockfd, packet, sizeof(struct iphdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto");
        exit(1);
    }
    printf("Packet sent\n");

    if (recvfrom(sockfd, packet2, sizeof(struct iphdr), 0, (struct sockaddr *)&dest_addr, &dest_addr) < 0)
        {
            perror("recvfrom");
            exit(1);
        }
    printf("Packet Recieved\n");
    struct iphdr *ipp = (struct iphdr *)packet2;
    struct in_addr paddr;
    paddr.s_addr = ipp->saddr;
    char stradd[100] = {0};

    inet_ntop(AF_INET, &(paddr), stradd, INET_ADDRSTRLEN);
    printf("%s\n",stradd);

    // Close the socket
    close(sockfd);

    return 0;
}
