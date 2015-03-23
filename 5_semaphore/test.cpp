#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <unistd.h>
#include <stdlib.h>

#define WAITING 0
#define BARBER 1
#define MUTEX 2

key_t key = 30;
int semid;
int numChairs;

void init () {
	// Create and access semaphore
	semid=semget(key, 3, IPC_CREAT|0666);

	// Set initial values
	semctl(semid, WAITING, SETVAL, numChairs);
	semctl(semid, BARBER, SETVAL, 1);
	semctl(semid, MUTEX, SETVAL, 1);
}

int main (int argc, char *argv[]) {
	// Check that the arguments are passed
    if (argc != 2) {
	printf("Use: SleepBarber <Num Chairs>\n");
	exit(-1);
    }
    numChairs = atoi(argv[1]);
    
    init();

	int retval;
	retval=semctl(semid, 0, GETVAL, 0);
	printf("%d\n", retval);
	retval=semctl(semid, 1, GETVAL, 0);
	printf("%d\n", retval);
	retval=semctl(semid, 2, GETVAL, 0);
	printf("%d\n", retval);
	// semctl(semid, 0, SETVAL, 0);
	// retval=semctl(semid, 0, GETVAL, 0);
	// printf("%d\n", retval);
}