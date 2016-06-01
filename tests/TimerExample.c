#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
#include <limits.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

/* Main */   
int main(int argc, char *argv[]) {
   struct timeval tval_start, tval_end, tval_diff;


   printf("Press Enter to start the timer\n");
   fgetc(stdin);
   gettimeofday(&tval_start, NULL);
   printf("\tStart time: %ld.%0.6ld\n", (long int) tval_start.tv_sec, (long int) tval_start.tv_usec);

   printf("Press Enter to STOP the timer\n");
   fgetc(stdin);
   gettimeofday(&tval_end, NULL);
   printf("\tEnd time: %ld.%0.6ld\n", (long int) tval_end.tv_sec, (long int) tval_end.tv_usec);


   timersub(&tval_end, &tval_start, &tval_diff);
   printf("\n\n\nEnd time: %ld.%0.6ld\n", (long int) tval_diff.tv_sec, (long int) tval_diff.tv_usec);

   return 0;
}



//      void timersub(struct timeval *a, struct timeval *b,
//  	     struct timeval *res);
//





