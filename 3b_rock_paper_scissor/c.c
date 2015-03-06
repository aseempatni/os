#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

char buffer[80];
int p[2];
char name[10];

void send(int x) { // write to pipe
    sprintf(buffer,"%d",x);
    // printf("sending %d\n", x);
    write(p[1], buffer, sizeof(buffer));
}

void childSigHandler ( int sig )
{
   if (sig == SIGUSR1) {
      // printf("+++ Child :  Received signal READY from parent...\n");
      int no = 1 + rand() % 3;
      send(no);
   } else if (sig == SIGUSR2) {
      printf("+++ Child : Received signal EXIT from parent...\n");
      sleep(1);
      exit(0);
   }
}

int main (int argc, char const *argv[])
{
   // srand((unsigned int)time(NULL));

   // Handle signal
   signal(SIGUSR1, childSigHandler);           /* Register SIGUSR1 handler */
   signal(SIGUSR2, childSigHandler);           /* Register SIGUSR2 handler */
   
   // Get pipes
   p[0] = atoi(argv[0]);
   p[1] = atoi(argv[1]);

  srand(time(NULL)+p[0]);
   // Close read end
   close(p[0]);
/*
   printf("%d\n",p[0]);
   printf("%d\n",p[1]);*/

   // Busy waiting
   while(1) {}

   exit(0);
}
