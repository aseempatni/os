#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>
#include <stdlib.h>

#include <iostream>

#define WAITING 0
#define BARBER 1
#define MUTEX 2

#define DEBUG false

using namespace std;

key_t key = 30;
int semid;
int numCustomers;

void randwait(int secs) {
    int len;
	
    // Generate a random number...
    len = (int) ((drand48() * secs) + 1);
    sleep(len);
}

void init () {
	// Create and access semaphore
	semid=semget(key, 3, IPC_CREAT|0666);
}

void printall () {
	if (DEBUG) {
		int retval;
		retval=semctl(semid, 0, GETVAL, 0);
		printf("waiting\t%d\n", retval);
		retval=semctl(semid, 1, GETVAL, 0);
		printf("barber \t%d\n", retval);
		retval=semctl(semid, 2, GETVAL, 0);
		printf("mutex \t%d\n", retval);
	}
}
int main (int argc, char *argv[]) {
	// Check that the arguments are passed
    if (argc != 2) {
	printf("Use: SleepBarber <Num Customers>\n");
	exit(-1);
    }
    numCustomers = atoi(argv[1]);
    
    init();
	struct sembuf sop;

    // Make multiple customers
    for (int i=0; i < numCustomers; i++) {
    	if (fork() == 0) {

			int num = i;
		    srand48(time(NULL) + num);

		    // Leave for the shop and take some random amount of
		    // time to arrive.
		    printf("Customer %d leaving for barber shop.\n", num);
		    randwait(5);
		    printf("Customer %d arrived at barber shop.\n", num);
			
			sop.sem_num=WAITING;
			sop.sem_op=-1;
			sop.sem_flg=IPC_NOWAIT;

		    // Wait for space to open up in the waiting room...
		    if (semop(semid, &sop, 1) == -1) {
		        printf("Waiting room full. Customer %d is leaving without haircut.\n",num);
		        exit(0);
		    }

		    printf("Customer %d entering waiting room.\n", num);

		    // Wait for the barber chair to become free.
		    sop.sem_num=BARBER;
		    sop.sem_op=-1;
		    sop.sem_flg=0;
		    semop(semid, &sop, 1);
			    
		    // The chair is free so give up your spot in the
		    // waiting room.
		    sop.sem_num=WAITING;
		    sop.sem_op=1;
		    sop.sem_flg=0;
		    semop(semid, &sop, 1);

		    // Wake up the barber...
		    printf("Customer %d having haircut.\n", num);
		    printall();

		    // Wait for the barber to finish cutting your hair.
			sop.sem_num=MUTEX;
			sop.sem_op=-1;
			sop.sem_flg=0;
			semop(semid, &sop, 1);

		    // Give up the chair.
		    printf("Customer %d leaving barber shop.\n", num);
		    exit(0);
    	}
    }
	while (wait(NULL) >0);
	cout << "All customers done." << endl;
}