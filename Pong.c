/* PONG - Ping utility implemented in the C programming language   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>         /* hostent */
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
//#include <linux/ip.h>
//#include <linux/icmp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef DEBUG
#define debug printf
#else
#define debug if(0) printf
#endif

#define perrif(CND, MSG) if (CND) perror(#MSG)
#define pexit(MSG) { fprintf(stderr, #MSG); exit(EXIT_FAILURE); } 
#define pexitif(CND, MSG) { if (CND) { fprintf(stderr, "%s\n", MSG); exit(EXIT_FAILURE);} }
#define _S struct
#define BUF_SIZE 1024

#define PVAR(VAR, TYPE) printf(#VAR " (" #TYPE ") : |%" #TYPE "|\n", VAR)
#define PARRVAR(ARR, IDX, TYPE) \
 printf(#ARR "[%d] (" #TYPE ") : |%" #TYPE "|\n", IDX, ARR[IDX])


void *BuildPacket(char *dst, struct iphdr **packet, int packetNum) {
   int pktSize = sizeof(struct iphdr) + sizeof(struct icmphdr);
   struct icmphdr *icmpHeader;
   char *srcIPAddress = "192.168.1.222";

   pktSize = sizeof(struct iphdr) + sizeof(struct icmphdr);  
   debug("pktSize: %d\n", pktSize);
   *packet = calloc(1, pktSize);

   //ipHeader = (struct iphdr *) packet; 
   icmpHeader = (struct icmphdr *) (*packet + sizeof(struct icmphdr));

   /* IP Fields to set:
    *    ihl      | Internet Header Length ; Length of entire IP header
    *    version  | Version 
    *    tos      | Type of service
    *    tot_len  | Length of entire packet
    *    ttl      | How many hops the packet will do 
    *    protocol | Define protocol
    *    check    | Checksum
    *    saddr    | Source Address
    *    daddr    | Destination Address
    *    */

   (*packet)->ihl = 5;
   (*packet)->version = 4;
   (*packet)->tot_len = pktSize;
   (*packet)->protocol = IPPROTO_ICMP;
   (*packet)->saddr = inet_addr(srcIPAddress);
   (*packet)->daddr = inet_addr(dst);
   //(*packet)->ttl = 225;
   //ipHeader->check = PacketChecksum();

   icmpHeader->type = ICMP_ECHO;
   debug("packet num: %d", packetNum);
   icmpHeader->un.echo.sequence = packetNum;

   //icmp->checksum

   return 0;
}

char *DNSLookup(char *name) {
   char *rtn;
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
   rtn = calloc(strlen(inet_ntoa(*addresses[0])), sizeof(char));
   strcpy(rtn, inet_ntoa(*addresses[0]));
      
   debug("DNS: %s was resolved to %s: \n", name, rtn);

   return rtn;
}

int EstablishConnection(char *dst, int *sockFD, struct sockaddr_in **sockAddr) {
   int sock, sockOpt;

   if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1 )
      perror("socket");

   /* Set IP_HDRINCL flag on socket to use MY IP header, 
    * rather than the kernals default IP header */
   setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &sockOpt, sizeof(int));

   *sockAddr = calloc(1, sizeof(struct sockaddr_in));
   (*sockAddr)->sin_family = AF_INET;
   (*sockAddr)->sin_addr.s_addr = inet_addr(dst);

   debug("socket fd: %d\n", sock);
   *sockFD = sock;

   return 1;

}

int SendPacket(int sockFD, struct sockaddr_in sockAddress, struct iphdr *packet, 
 struct timeval *sendTime, char *dst) {

   //struct iphdr *ipPacket;

   //ipPacket = (struct iphdr *) packet;

   debug("ipPacket->totLen : %d\n", packet->tot_len);

   if (sendto(sockFD, packet, packet->tot_len, 0, 
    (struct sockaddr *) &sockAddress, sizeof(struct sockaddr)) == -1)
      perror("sendto");

   gettimeofday(sendTime, NULL);

   debug("Sent %d byte packet to %s\n", packet->tot_len, dst);

   return 1;
}

void ReadPacket(int sockFD, char *destIP, struct sockaddr_in sockAddr,
 struct timeval *sendTime, struct timeval *recTime) {

   struct timeval timeDiff;
   struct iphdr *replyPacket;
   struct icmphdr *replyIcmpHeader;
   int addressLength, bufSize;
   char *buf;//, *packetSrcAddr;
   char *output = "%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%2.1lf ms\n";

   bufSize = sizeof(struct iphdr) + sizeof(struct icmphdr);
   buf = malloc(bufSize);
   addressLength = sizeof(sockAddr);
   debug("Address Length: %d", addressLength);

   if (recvfrom(sockFD, buf, bufSize, 0, (struct sockaddr *) &sockAddr, 
    (socklen_t *) &addressLength) == -1)
      perror("recvfrom");
   gettimeofday(recTime, NULL);

   debug("REC VARS:\n\
      sockfd: %d\n\
      s/o(buffer): %ld\n\
      buffer: %s\n\
      3rd size: %ld\n\
      addrlen: %d\n",
      sockFD, sizeof(buf), buf, sizeof(struct iphdr) + sizeof(struct icmphdr),
      addressLength);

   replyPacket = (struct iphdr *) buf;
//jjk   printf("internal pnum: %d\n", 
//jjk    (
//jjk     (struct icmphdr *) (replyPacket + sizeof(struct icmphdr))
//jjk    )->un.echo.sequence
//jjk   );
//jjk   //replyIcmpHeader = (struct icmphdr *) (*packet + sizeof(struct icmphdr));
   replyIcmpHeader = (struct icmphdr *) (replyPacket + sizeof(struct icmphdr));
//jjk   printf("replyIcmpHeader seq: %d\n", replyIcmpHeader->un.echo.sequence);
   //packetSrcAddr = (char *)&replyPacket->saddr;
 /*  printf("Received %d bytes from %u.%u.%u.%u:\n", 
    ntohs(replyPacket->tot_len), 
    packetSrcAddr[0] & 0xff, 
    packetSrcAddr[1] & 0xff, 
    packetSrcAddr[2] & 0xff,
    packetSrcAddr[3] & 0xff);
 */
   timersub(recTime, sendTime, &timeDiff);
   if (replyIcmpHeader);
      

   printf(output, ntohs(replyPacket->tot_len), DNSLookup(destIP), destIP, 
    replyIcmpHeader->un.echo.sequence, replyPacket->ttl, (double) timeDiff.tv_usec / 100 );
    //fk, 
    

   //printf("Took %2.1lf\n", (double) timeDiff->tv_usec / 100 ); 
   //printf("Took %ld.%.6ld\n", (long int) timeDiff.tv_sec, (long int) timeDiff.tv_usec); 
   //printf("TTL: %d\n", ip_reply->ttl);

}

int main(int argc, char *argv[]) {
   char *dstIP, *dst;
   int sockFD, packetNum = 0;
   struct iphdr *packet;
   struct sockaddr_in *sockAddress;
   struct timeval recTime, sendTime;

   pexitif(argc < 2, "Missing destination argument\n");
   dst = argv[1];
   dstIP = DNSLookup(dst);
   debug("dst: %s\n", dst);
   debug("dstIP: %s\n", dstIP);
   BuildPacket(dstIP, &packet, packetNum);

   if (!EstablishConnection(dstIP, &sockFD, &sockAddress))
      pexit("Could not connect to host");
   
   while (1) { 
      //BuildPacket(dstIP, &packet, packetNum++);
      ((struct icmphdr *) (packet + sizeof(struct icmphdr)))->un.echo.sequence = packetNum++;
      //printf("internal pnum: %d\n", ((struct icmphdr *) (packet + sizeof(struct icmphdr)))->un.echo.sequence);
      //printf("packet Num: %d\n", packetNum);
      SendPacket(sockFD, *sockAddress, packet, &sendTime, dstIP);
      ReadPacket(sockFD, dst, *sockAddress, &sendTime, &recTime);
      sleep(1);
   }

   return 0;
}

