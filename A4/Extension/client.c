#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

char randomString[] = "nrVEbdeqaHgbjRNBrgtMzWKXCEVLiFAAeVjiSgwwzRLTDyJViMzKvJJMdBBfzyNTzGYMmubFnhmHADLQpXgBRStabqSgRtkqiMkVyyBDRnVDqDedretgJpckHHnRnHcPXEMFiTjrVcRCipkMQCXLSKxmDHSFAApgfniVcUUfHvyrdHuVyxpmwVAPLhDuFCGuLRfKCzWvXJZFbbwRRZBbwzeNmcReuiucUJzhLtKrTauerCJAKmLqQFzfPTQxqAZJXzaNLVWrmpCBKReVjUXWXNnCngGzxBgQVNKvdjuzGxizncUEWHaJZqHybeFWvhQDPDFggfLqHZDSxTtUGmrBwyKDcYqSGgGfaECPnHAGXYtMfFTUbiBRvyXUxprfmbaDpKdwbguXeiPAXEXibGJfhAyacNLKuBXDNSVmdAYzqrVdyxnnfvtnCHLnKDVdxNFnfrVXCPhjwUpbEYghNfyDSwEhMFmKvSxqhxXZeRqNBEiWXpCLxbGcuvbBBXqwhkMjXbaKahrpYHpzSfZryEHjCydvpiYwbEBebRbXkLEifjRJtpvTChaZfSBYHTWPwTvAwixZTBpZRTbVSGRZYNHWtiwyhFMSJKTmAyhvFnVMmLpeKkeegvaLRTjaTgriQLgzhHWhJESvYYeDrRxFJgtuEmJHXrvGhDuEJbpzJikQDCNyDpfCmvBEfdvwUuDBaLzyxmASiRcLHexHLMcQZLpetEatPmaCqFKCUaKWaVRbJGTEqDnDXeQdFEStDTSCxLLirUFwwJRJNBjQnCMAjhrvRcUCjpAnAevCkuSWZYTCuEtqTVTzkXtVgPfHcAgkSRTaZBWwZgktiPUdHdWeLDEEJLkyGMgXfJRQeHfKZJLZYtBiUHNefWcYWXBABwrrXvuHtvfCBTfLWtzSLiaYqWJWRtkCdvAYYFXUuDBTHipXtCytgXprATypFnPTPwJJDgzrDqKArGAXenYaynGVyxVQrvPaDGYcfXjYwkbMbDaEZARwPRckQGGNkDxHwgzWxxDMpynivcJCGUMiCdciKkxeTKXWvhDmAvMpADHnkAKqXQWfQPHkXmKgwhgLPPPMXBKXNckHdDMRftFvTUmgtMiPBgXqmHMchCBMRMhdPaaJfkvCbTyQcENdhNekYxmApqQAnYLkPmUQDcdtiVCycbcrUPcPaETcdFNSqCHAPtgekcXvfffJmQMWnYRWfnEurYkBxjrmxuJMiKwEWWRgfkxFpzChyUdLKAGrhyLqquYxfLFkLGJeCvvdKHPmedkNfMvpFCyaVXeheFuzKySAmSWEBkxVVwaSwt";
FILE *fptr;

// argument desc - ./client ipaddr port payload TTL numofPack filename
int main(int argc, char *argv[])
{
    int payloadByte = atoi(argv[3]);
    int numPacket = atoi(argv[5]);
    char TTL = atoi(argv[4]);
    fptr = fopen(argv[6], "w");

    // int payloadByte = 100;
    // int numPacket = 5;
    // char TTL = 98;
    clock_t tsend, trecv;
    float sentTime, recvTime;
    float RTT, AVGRTT = 0;

    short *st;
    int *it;
    float *ft;

    int socket_fd;
    struct sockaddr_in server_addr;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0)
    {
        perror("Socket creation failed");
        exit(0);
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    // server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int server_addr_length = sizeof(server_addr);

    char msg[7 + payloadByte];

    for (int i = 1; i <= numPacket; i++)
    {
        bzero(msg, sizeof(msg));
        st = (short *)msg;
        st[0] = i;
        msg[6] = TTL;

        memcpy(&msg[7], &randomString[0], payloadByte);

        ft = (float *)&msg[2];
        ft[0] = (float)clock();

        while (msg[6])
        {
            sendto(socket_fd, msg, sizeof(msg), MSG_CONFIRM, (const struct sockaddr *)&server_addr, sizeof(server_addr));

            recvfrom(socket_fd, msg, sizeof(msg), MSG_WAITALL, (struct sockaddr *)&server_addr, &server_addr_length);
        }

        ft = (float *)&msg[2];
        sentTime = ft[0];

        recvTime = clock();

        RTT = ((recvTime - sentTime) / CLOCKS_PER_SEC);
        RTT *= 1e3;
        AVGRTT += RTT;
        st = (short *)msg;
        fprintf(fptr, "seq_no %hu  time : %0.3f ms\n", st[0], RTT);
        printf("%d bytes  seq_no %hu  time : %0.3f ms\n", (int)sizeof(msg), st[0], RTT);
        sleep(1);
    }
    printf("  Average Round Trip Time : %f ms\n", AVGRTT / numPacket);
    fclose(fptr);
    close(socket_fd);
    return 0;
}