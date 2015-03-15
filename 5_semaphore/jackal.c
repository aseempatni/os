#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define MAX_RETRIES 10

int genRandom()
{
    srand(time(NULL));
    return rand()%3+1;
}

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

int main(void)
{
    key_t key=20;
    int semid;
    struct sembuf sb;

    semid = semget(key,3,0666|IPC_CREAT);

    while(1)
    {
        /* check if pit not occupied */
        int pitid = genRandom();
        sb.sem_op = -1;
        sb.sem_flg = 0;
        sb.sem_num = 2*(pitid-1)+1;
        // check if pit occupied
        sb.sem_op = 1;
        semop(semid, &sb, 1);


        /* CR */
        // ranger in

        if(semctl(semid,2*(pitid-1)+2,GETVAL,0)<10)
        {
            printf("Filling %d pit with food\n",sb.sem_num);
            semctl(semid,2*(pitid-1)+2,SETVAL,10);
        }

        // unoccupy pit
        sb.sem_num = 2*(pitid-1)+1;
        semop(semid, &sb, 1);
    }

    printf("Unlocked\n");

    return 0;
}