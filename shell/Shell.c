/* Author: Brady Shutt */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define pexit(msg) {\
   fprintf(stderr, #msg);\
   exit(EXIT_FAILURE); }

#define ARG_COUNT 01
#define ARG_INTERVAL 02
#define ARG_DEADLINE 04
#define ARG_TIMEOUT 017
#define ARG_TTL 027


ParseArgs(int argc, char *argv[], int *args) {


}

int main(int argc, char *argv[]) {
   int args;

   if (argc < 2)
      pexit("Usage: sudo ./Pong <ip-address>\n");


   ParseArgs(argc, argv, &args);





   return 0;
}

