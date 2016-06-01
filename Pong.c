/* PONG - Ping utility implemented in the C programming language   */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>         /* hostent */
//#include <netinet/ip.h>
//#include <netinet/ip_icmp.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PVAR(VAR, TYPE) printf(#VAR " (" #TYPE ") : |%" #TYPE "|\n", VAR)
#define PARRVAR(ARR, IDX, TYPE) \
 printf(#ARR "[%d] (" #TYPE ") : |%" #TYPE "|\n", IDX, ARR[IDX])

void *BuildPacket(char *dst, void *packet) {
   int pktSize = sizeof(struct iphdr) + sizeof(struct icmphdr);
   struct iphdr *ip;
   struct icmphdr *icmp;
   char *srcIPAddress = "192.168.1.222";

   ip = (struct iphdr *) packet; 
   icmp = (struct icmphdr *) (ip + sizeof(struct icmphdr));

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

   ip->ihl = 5;
   ip->version = 4;
   ip->tot_len = pktSize;
   ip->protocol = IPPROTO_ICMP;
   ip->saddr = inet_addr(srcIPAddress);
   ip->daddr = inet_addr(dst);
   //ip->check = PacketChecksum();

   icmp->type = ICMP_ECHO;
   //icmp->checksum

   memcpy(packet, ip, sizeof(struct iphdr));
   return 0;
}

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

   return inet_ntoa(*addresses[0]);
}

int main(int argc, char *argv[]) {
   char *dstIP, *buf, *pktSrcAddr;
   int sock, sockOpt, adrLen, pktSize;
   struct iphdr *packet, *replyPacket;
   struct sockaddr_in connection;

   dstIP = GetIPAddress("www.google.com");
   printf("DNS: %s was resolved to %s: \n", "www.google.com", dstIP);

   pktSize = sizeof(struct iphdr) + sizeof(struct icmphdr);  
   packet = malloc(pktSize);
   buf = malloc(pktSize);
   
   BuildPacket(dstIP, packet);
   
   if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1 );
      perror("socket");

   /* Set IP_HDRINCL flag on socket to use MY IP header, 
    * rather than the kernals default IP header */
   setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &sockOpt, sizeof(int));

   connection.sin_family = AF_INET;
   connection.sin_addr.s_addr = packet->daddr;

   printf("%d\n", sock);
   sendto(sock, packet, packet->tot_len, 0, (struct sockaddr *) &connection, sizeof(struct sockaddr));

   printf("Sent %d byte packet to %s\n", packet->tot_len, dstIP);

   adrLen = sizeof(connection);
   PVAR(adrLen, d);

    printf("REC VARS:\n\
          sockfd: %d\n\
          s/o(buffer): %d\n\
          buffer: %s\n\
          3rd size: %d\n\
          &connection: %d\n\
          &addrlen: %d\n\
          addrlen: %d\n",
          sock, sizeof(buf), buf, 
          sizeof(struct iphdr) + sizeof(struct icmphdr), &connection, &adrLen, adrLen);
   //if (recvfrom(sock, buf, pktSize, 0, (struct sockaddr *) &connection, &addrlen) == -1)
   if (recvfrom(sock, buf, pktSize, 0, (struct sockaddr *) &connection, (socklen_t *) &adrLen) == -1)
      perror("recvfrom");

   
   replyPacket = (struct iphdr*) buf;
   pktSrcAddr = (char *)&replyPacket->saddr;
   printf("hi\n");
   printf("Received %d byte reply from %2u.%u.%u.%u:\n", 
    ntohs(replyPacket->tot_len), pktSrcAddr[0],pktSrcAddr[1]&0xff,pktSrcAddr[2]&0xff,pktSrcAddr[3]&0xff);




   return 0;
}

