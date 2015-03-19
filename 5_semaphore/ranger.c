#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define N 8
#define PIT 0
#define LION 1
#define JACKAL 2
#define FOOD 3
#define LION_FOOD_LOCK 4
#define LION_COUNT_LOCK 5
#define JACKAL_FOOD_LOCK 6
#define JACKAL_COUNT_LOCK 7
#define ANIMAL LION
#define MUTEX_FOOD LION_FOOD_LOCK
#define MUTEX_COUNT LION_COUNT_LOCK

key_t key=21;
int semid,pitid,val;
struct sembuf sb;

int genRandom()
{
    return rand()%3+1;
}

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

void up(int num,int pitid)
{
    sb.sem_op = 1;
    sb.sem_flg = 0;
    sb.sem_num = N*(pitid-1)+num;
    semop(semid, &sb, 1);
}

void down(int num,int pitid)
{
    sb.sem_op = -1;
    sb.sem_flg = 0;
    sb.sem_num = N*(pitid-1)+num;
    semop(semid, &sb, 1);
}

int getCount(int num,int pitid)
{
    return semctl(semid,N*(pitid-1)+num,GETVAL,0);
}

void init()
{
    srand(time(NULL));
    semid = semget(key,50,0666|IPC_CREAT);
    int i;
    for(i=0;i<50;i++)
        semctl(semid,i,SETVAL,1);
    for(i=0;i<3;i++)
    {
        semctl(semid,N*i+FOOD,SETVAL,0);
        semctl(semid,N*i+LION,SETVAL,0);
        semctl(semid,N*i+JACKAL,SETVAL,0);
    }
}

int main()
{
    init();
    while(1)
    {
        /* check if pit not occupied */
        pitid = genRandom();
        printf("Ranger: Entering Pit %d\n",pitid);
        down(PIT,pitid);
        printf("Ranger: Lock %d:%d\n",pitid,getCount(PIT,pitid));
        if((val = semctl(semid,N*(pitid-1)+FOOD,GETVAL,0))<=40)
        {
            printf("Ranger: Filling %d pit.Total %d food\n",pitid,val+10);
            semctl(semid,N*(pitid-1)+FOOD,SETVAL,val+10);
        }

        up(PIT,pitid);
        //getchar();
    }

    printf("Unlocked\n");
    return 0;
}