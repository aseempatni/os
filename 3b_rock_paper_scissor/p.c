#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

char arg[2][15];
char name[10];
char buffer[80];

#define PAPER 1
#define SCISSOR 2
#define ROCK 3

int readfrompipe (int p) { // read from pipe
   int x;
   // printf("%d\n", p);
   read(p,buffer,sizeof(buffer));
   sscanf(buffer,"%d",&x);
   return x;
}

int main () {
   int pidc, pidd; // Childs id
   int pc[2];  // Communication to C
   pipe (pc);
   int pd[2];  // Communication to D
   pipe (pd);
   system("gcc c.c -o c");
   pidc = fork();                                   /* Spawn the child process */
   if (pidc) {
      pidd = fork();                                   /* Spawn the child process */
      if (pidd) {
         close(pc[1]);
         close(pd[1]);
      }
      else { // Child 1
         sprintf(arg[0],"%d",pd[0]);
         sprintf(arg[1],"%d",pd[1]);
         execl("c",arg[0],arg[1],(char *)0);
         strerror(errno);
         exit(0);
      }
   } else { // Child 2
      sprintf(arg[0],"%d",pc[0]);
      sprintf(arg[1],"%d",pc[1]);
      execl("c",arg[0],arg[1],(char *)0);
      strerror(errno);
      exit(0);
   }

   /* Parent process */
   int t;
   srand((unsigned int)time(NULL));

   t = 1 + rand() % 1;
   printf("+++ Parent: Going to sleep for %d seconds\n", t);
         sleep(t);       /* Sleep for some time before sending a signal to child */

   int read_c, read_d;
   float cscore=0, dscore=0;
   while (cscore<=10 && dscore<=10) {
      printf("+++ Parent: Sending ready to both child\n");

      kill(pidc,SIGUSR1);
      read_c = readfrompipe(pc[0]);
      printf("C got %d\n",read_c);

      kill(pidd,SIGUSR1);
      read_d = readfrompipe(pd[0]);
      printf("D got %d\n",read_d);

      if (read_d== read_c) {
         cscore+=0.5;
         dscore+=0.5;
      }
      else {
         switch(read_c) {
            case SCISSOR:
            switch (read_d) {
               case PAPER: cscore++; break;
               case ROCK: dscore++; break;
            }
            break;
            case PAPER: 
            switch (read_d) {
               case SCISSOR: dscore++; break;
               case ROCK: cscore++; break;
            }
            break;
            case ROCK: 
            switch (read_d) {
               case SCISSOR: cscore++;break;
               case PAPER: dscore++; break;
            }
            break;
         }
      }

      printf("+++ Parent: C's score +> %f\n", cscore);
      printf("+++ Parent: D's score => %f\n", dscore);
      // dscore++;
            // sleep(1);
   }
   int winner;
   if (cscore>10 && dscore>10) {
      read_d = rand();
      read_c = rand();
      if (read_c> read_d) winner = 1;
      else winner= 2;
   }

   else if (cscore>10)  winner = 1;
   else if (dscore>10)  winner = 2;
   if(winner==1)   printf("+++ Parent: C won the game\n");
   if(winner==2)   printf("+++ Parent: D won the game\n");
   printf("+++ Parent: sending exit to both child\n");
   kill(pidd,SIGUSR2);
   kill(pidc,SIGUSR2);
   printf("+++ Parent: waiting for children to end\n");
   wait(NULL);
   wait(NULL);
         // waitpid(pidc,NULL,0);                                   /* Wait for child to exit */
         // waitpid(pidd,NULL,0);                                   /* Wait for child to exit */
   printf("+++ Parent: Children died\n");
   exit(0);
}
