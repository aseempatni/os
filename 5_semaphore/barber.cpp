#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

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
int numChairs;
void randwait(int secs) {
	int len;
	
    // Generate a random number...
	len = (int) ((drand48() * secs) + 1);
	sleep(len);
}

void sleep_barber() {
	// Sleep until someone arrives and wakes you..
	int temp;
	temp=semctl(semid, WAITING, GETVAL, 0);
	if (temp==numChairs) {
		cout << "Barber is going to sleep" << endl;
	}
}

void open_shop () {
	// Create and access semaphore
	semid=semget(key, 3, IPC_CREAT|0666);

	// Set initial values

	// All chairs available
	semctl(semid, WAITING, SETVAL, numChairs);
	// Make the barber available
	semctl(semid, BARBER, SETVAL, 1);
	// Seatbelt tied, haircut not complete
	semctl(semid, MUTEX, SETVAL, 0);

	cout << "Barber opened the shop." << endl;

	// printf("The barber is sleeping\n");
    srand48(time(NULL));

    sleep_barber();
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
		printf("Use: SleepBarber <Num Chairs>\n");
		exit(-1);
	}
	numChairs = atoi(argv[1]);

	open_shop();

	struct sembuf sop;
	int temp;
	while (1) {
		printall();

    	// Wait for someone to arrive
		sop.sem_num=BARBER;
		sop.sem_op=0;
		sop.sem_flg=0;
		semop(semid, &sop, 1);

	    // Take a random amount of time to cut the
	    // customer's hair.
		printf("The barber is cutting hair.\n");
		randwait(3);
		printf("The barber has finished cutting hair.\n");

	    // Tell the customer that the haircut is complete
		sop.sem_num=MUTEX;
		sop.sem_op=1;
		sop.sem_flg=0;
		semop(semid, &sop, 1);

		// If waiting room is empty, go to sleep
		sleep_barber();

	    // Signal the waiting customers
		sop.sem_num=BARBER;
		sop.sem_op=1;
		sop.sem_flg=0;
		semop(semid, &sop, 1);
	}
}