#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main () {
	// Create and access semaphore
	key_t key=(key_t)20;
	int nsem=1;	
	int semid=semget(key, nsem, IPC_CREAT|0666);
	int value = 0;
	int retval=semctl(semid, 0, GETVAL, 0);
	printf("%d\n", retval);
	semctl(semid, 0, SETVAL, 0);
	retval=semctl(semid, 0, GETVAL, 0);
	printf("%d\n", retval);
}