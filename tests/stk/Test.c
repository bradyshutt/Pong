
/* send icmp packet example */
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <arpa/inet.h>
#include <sys/socket.h>


//unsigned short in_cksum(unsigned short *addr, int len);

char *GetIPAddress(char *name) {
   struct hostent *he;
   struct in_addr **addresses;

   he = gethostbyname(name); 
   if (he == NULL) {
      perror("gethostbyname error");
      return NULL;
   }

   addresses = (struct in_addr**) he->h_addr_list;

   if (addresses[0] == NULL) {
      perror("addresses = NULL");
      return NULL;
   }

   return (char *) inet_ntoa(*addresses[0]);
}

int main(int argc, char* argv[]) {
    struct iphdr *ip, *ip_reply;
    struct icmphdr* icmp;
    struct sockaddr_in connection;
    char *dst_addr;
    char *src_addr="192.168.1.222";
    char *packet, *buffer;
    int sockfd, optval, addrlen;

    dst_addr = GetIPAddress("www.google.com");
    printf("dst_addr: %s\n", dst_addr);

    packet = malloc(sizeof(struct iphdr) + sizeof(struct icmphdr));
    buffer = malloc(sizeof(struct iphdr) + sizeof(struct icmphdr));
    ip = (struct iphdr*) packet;
    icmp = (struct icmphdr*) (packet + sizeof(struct iphdr));

    ip->ihl         = 5;
    ip->version     = 4;
    ip->tot_len     = sizeof(struct iphdr) + sizeof(struct icmphdr);
    ip->protocol    = IPPROTO_ICMP;
    ip->saddr       = inet_addr(src_addr);
    ip->daddr       = inet_addr(dst_addr);
    //ip->check = in_cksum((unsigned short *)ip, sizeof(struct iphdr)); 

    icmp->type      = ICMP_ECHO;
    //icmp->checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr));

    /* open ICMP socket */
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
     /* IP_HDRINCL must be set on the socket so that the kernel does not attempt 
     *  to automatically add a default ip header to the packet*/
    setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    connection.sin_family       = AF_INET;
    connection.sin_addr.s_addr  = ip->daddr;
    printf("ip->tot_len: %d\n", ip->tot_len);
    sendto(sockfd, packet, ip->tot_len, 0, (struct sockaddr *)&connection, sizeof(struct sockaddr));
    perror("sendto");
    printf("Sent %d byte packet to %s\n", ip->tot_len, dst_addr);

    addrlen = sizeof(connection);
    printf("REC VARS:\n\
          sockfd: %d\n\
          s/o(buffer): %d\n\
          buffer: %s\n\
          3rd size: %d\n\
          &connection: %d\n\
          &addrlen: %d\n\
          addrlen: %d\n",
          sockfd, sizeof(buffer), buffer, 
          sizeof(struct iphdr) + sizeof(struct icmphdr), &connection, &addrlen, addrlen);

    if (recvfrom(sockfd, buffer, sizeof(struct iphdr) + sizeof(struct icmphdr), 
             0, (struct sockaddr *)&connection, &addrlen) == -1)
        {
        perror("recv");
        }
    else
        {
        char *cp;
        ip_reply = (struct iphdr*) buffer;
        cp = (char *)&ip_reply->saddr;
        printf("Received %d byte reply from %u.%u.%u.%u:\n", ntohs(ip_reply->tot_len), cp[0]&0xff,cp[1]&0xff,cp[2]&0xff,cp[3]&0xff);
        printf("ID: %d\n", ntohs(ip_reply->id));
        printf("TTL: %d\n", ip_reply->ttl);
        }

}

//unsigned short in_cksum(unsigned short *addr, int len) {
//    register int sum = 0;
//    u_short answer = 0;
//    register u_short *w = addr;
//    register int nleft = len;
//    /*
//     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
//     * sequential 16 bit words to it, and at the end, fold back all the
//     * carry bits from the top 16 bits into the lower 16 bits.
//     */
//    while (nleft > 1)
//    {
//      sum += *w++;
//      nleft -= 2;
//    }
//    /* mop up an odd byte, if necessary */
//    if (nleft == 1)
//    {
//      *(u_char *) (&answer) = *(u_char *) w;
//      sum += answer;
//    }
//    /* add back carry outs from top 16 bits to low 16 bits */
//    sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
//    sum += (sum >> 16);             /* add carry */
//    answer = ~sum;              /* truncate to 16 bits */
//    return (answer);
//}
