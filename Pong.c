/* PONG - Ping utility implemented in the C programming language   */

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/time.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <linux/icmp.h>
//#include <linux/ip.h>
//#include <time.h>
//#include <unistd.h>
//#include <netdb.h>         /* hostent */
////#include <netinet/in.h>
////#include <netinet/ip_icmp.h>
//#include <arpa/inet.h>

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
#define pexit(MSG) { fprintf(stderr, #MSG "\n"); exit(EXIT_FAILURE); } 
//#define pexitif(CND, MSG) { if (CND) { fprintf(stderr, "%s\n", MSG); exit(EXIT_FAILURE);} }
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

//int EstablishConnection(char *dst, int *sockFD, struct sockaddr_in **sockAddr) {
//   int sock, sockOpt;
//
//   if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1 )
//      perror("socket");
//
//   /* Set IP_HDRINCL flag on socket to use MY IP header, 
//    * rather than the kernals default IP header */
//   setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &sockOpt, sizeof(int));
//
//   *sockAddr = calloc(1, sizeof(struct sockaddr_in));
//   (*sockAddr)->sin_family = AF_INET;
//   (*sockAddr)->sin_addr.s_addr = inet_addr(dst);
//
//   debug("socket fd: %d\n", sock);
//   *sockFD = sock;
//
//   return 1;
//}

//int SendPacket(int sockFD, struct sockaddr_in sockAddress, struct iphdr *packet, char *dst) {
//
//   //struct iphdr *ipPacket;
//
//   //ipPacket = (struct iphdr *) packet;
//
//   debug("ipPacket->totLen : %d\n", packet->tot_len);
//
//   if (sendto(sockFD, packet, packet->tot_len, 0, 
//    (struct sockaddr *) &sockAddress, sizeof(struct sockaddr)) == -1)
//      perror("sendto");
//
//   //gettimeofday(sendTime, NULL);
//
//   debug("Sent %d byte packet to %s\n", packet->tot_len, dst);
//
//   return 1;
//}
//


//sockFd, destIp, sockaddr_in sockAddr, 

void ReadPacket(int sockFD, char *dst) {

   struct iphdr *replyPacket;
   struct icmphdr *replyIcmpHeader;
   struct sockaddr_in recSock;
   int addrLen, bufSize;
   char *buf;//, *packetSrcAddr;

   recSock.sin_family = AF_INET;
   recSock.sin_addr.s_addr = inet_addr(dst);

   addrLen = sizeof(recSock);
   bufSize = sizeof(struct iphdr) + sizeof(struct icmphdr);
   buf = calloc(1, bufSize);
   debug("Address Length: %d", addrLen);

   //fprintf(stderr, "Reading from socked: %d\n", sockFD);
   printf("Reading a packet into a %d byte butter through sockedFD %d\n", bufSize ,  sockFD);
   //if ((sockFD = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1 )
   //   perror("Read create socket");

   recvfrom(sockFD, buf, bufSize, 0, (struct sockaddr *) &recSock, 
    (socklen_t *) &addrLen);
   perror("recvfrom");

   fprintf(stderr, "done\n" );
//socklen_t *addrlen
   //gettimeofday(recTime, NULL);

   debug("REC VARS:\n\
      sockfd: %d\n\
      s/o(buffer): %ld\n\
      buffer: %s\n\
      3rd size: %ld\n\
      addrlen: %d\n\
      \tID: %d\n",
      sockFD, sizeof(buf), buf, sizeof(struct iphdr) + sizeof(struct icmphdr),
      addrLen, ntohs(((struct icmphdr *) (buf + sizeof(struct icmphdr)))->un.echo.id));

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
   //timersub(recTime, sendTime, &timeDiff);
   if (replyIcmpHeader);
      
//   struct in_addr srcAddr, dstAddr;
//   srcAddr.s_addr = replyPacket->saddr;
//   dstAddr.s_addr = replyPacket->daddr;
//
//   printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%2.1lf ms\n", 
//         ntohs(replyPacket->tot_len),
//         inet_ntoa(srcAddr),
//         inet_ntoa(dstAddr),
//         //char *inet_ntoa(struct in_addr in);
//         replyIcmpHeader->un.echo.sequence, 
//         replyPacket->ttl, 
//         (double) timeDiff.tv_usec / 100);

   //printf("Took %2.1lf\n", (double) timeDiff->tv_usec / 100 ); 
   //printf("Took %ld.%.6ld\n", (long int) timeDiff.tv_sec, (long int) timeDiff.tv_usec); 
   //printf("TTL: %d\n", ip_reply->ttl);

}

void PingHost(char *dst, int interval, int *sockFD) {
   int sockOpt, pktSize, pktNum = 1, pid;
   char *pkt, *srcIPAddress = "192.168.1.222";
   struct sockaddr_in sockAddr;
   struct icmphdr *icmpHdr;
   struct iphdr *ipHdr;

   if (((*sockFD) = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
      perror("Socket");
   debug("socketFD: %d\n", *sockFD);

   /* Use MY ip_header, not the kernal's default */
   setsockopt(*sockFD, IPPROTO_IP, IP_HDRINCL, &sockOpt, sizeof(int));

   sockAddr.sin_family = AF_INET; 
   sockAddr.sin_addr.s_addr = inet_addr(dst);

   pktSize = sizeof(struct iphdr) + sizeof(struct icmphdr);  
   pkt = calloc(1, sizeof(struct iphdr) + sizeof(struct icmphdr));

   ipHdr = (struct iphdr *) pkt; 
   icmpHdr = (struct icmphdr *) (pkt + sizeof(struct icmphdr));  

   ipHdr->ihl = 5;
   ipHdr->version = 4;
   ipHdr->tot_len = pktSize;
   ipHdr->protocol = IPPROTO_ICMP;
   ipHdr->saddr = inet_addr(srcIPAddress);
   ipHdr->daddr = inet_addr(dst);
   icmpHdr->type = ICMP_ECHO;
   icmpHdr->un.echo.sequence = htons(pktNum++);
   icmpHdr->un.echo.id = htons(pid = getpid());

   debug("pktSize: %d\n", pktSize);
   debug("pid: %d\n", pid);
   debug("icmp ID: %d\n", ntohs(icmpHdr->un.echo.id));
   debug("packet num: %d\n", icmpHdr->un.echo.sequence);
   debug("ipPacket->totLen : %d\n", ipHdr->tot_len);

   printf("Sending a %d byte packet to %s, through sockedFD %d\n", ipHdr->tot_len ,dst, *sockFD);

   if (sendto(*sockFD, pkt, ipHdr->tot_len, 0, (struct sockaddr*) &sockAddr, 
    (socklen_t) sizeof(struct sockaddr_in)));
    perror("sendto");
    //sizeof(sockAddr)) == -1)
    //(socklen_t *) &addressLength) == -1)
//
    //  sizeof(struct sockaddr_in);
    //(socklen_t *) &addressLength) == -1)

   //gettimeofday(sendTime, NULL);

   debug("Sent %d byte packet to %s\n", ((struct iphdr *) pkt)->tot_len, dst);
   //icmp->checksum
   //(*packet)->ttl = 225;
   //ipHeader->check = PacketChecksum();
   //pexit("Could not connect to host");
}

int main(int argc, char *argv[]) {
   char *dstIP, *dst;
   int interval = 1, sockFD;

   if (argc < 2)
      pexit("Missing destination argument");
   dst = argv[1];
   dstIP = DNSLookup(dst);
   debug("dst: %s\n", dst);
   debug("dstIP: %s\n", dstIP);
   //BuildPacket(dstIP, &packet, packetNum);

   //if (!EstablishConnection(dstIP, &sockFD, &sockAddress))
   //   pexit("Could not connect to host");
   

//   if (0) {//!fork()) {
 //     //receiver
  // }
   //else {
      PingHost(dstIP, interval, &sockFD);
      ReadPacket(sockFD, dstIP);

   //}

   //exit(0);

  // while (1) { 
  //    //BuildPacket(dstIP, &packet, packetNum++);
  //    ((struct icmphdr *) (packet + sizeof(struct icmphdr)))->un.echo.sequence = packetNum++;
  //    //printf("internal pnum: %d\n", ((struct icmphdr *) (packet + sizeof(struct icmphdr)))->un.echo.sequence);
  //    //printf("packet Num: %d\n", packetNum);
  //    SendPacket(sockFD, *sockAddress, packet, dstIP);
  //    ReadPacket(*sockAddress);
  //    sleep(1);
  // }

   return 0;
}

