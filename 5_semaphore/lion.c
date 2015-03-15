#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define N 4
#define PIT 0
#define LION 1
#define JACKAL 2
#define FOOD 3
#define LION_FOOD_LOCK 20
#define JACK_FOOD_LOCK 21
#define LION_COUNT_LOCK 21
#define JACK_COUNT_LOCK 21
#define ANIMAL LION
#define MUTEX_FOOD LION_FOOD_LOCK
#define MUTEX_COUNT LION_COUNT_LOCK

key_t pkey=20;
int semid;
struct sembuf sb;

int genRandom()
{
    srand(time(NULL));
    return rand()%3+1;
}

void init()
{
    semid = semget(pkey,3,0666|IPC_CREAT);
    semctl(semid,0,SETALL,0);
}

void up(int num,int pitid)
{
    sb.sem_op = 1;
    sb.sem_flg = 0;
    if(num==MUTEX_COUNT)
    {
        sb.sem_num = MUTEX_COUNT;
        semop(semid, &sb, 1);
    }
    else if(num==MUTEX_FOOD)
    {
        sb.sem_num = MUTEX_FOOD;
        semop(semid, &sb, 1);
    }
    else
    {
        sb.sem_num = N*(pitid-1)+num;
        semop(semid, &sb, 1);
    }
}

void down(int num,int pitid)
{
    sb.sem_op = -1;
    sb.sem_flg = 0;
    if(num==MUTEX_COUNT)
    {
        sb.sem_num = MUTEX_COUNT;
        semop(semid, &sb, 1);
    }
    else if(num==MUTEX_FOOD)
    {
        sb.sem_num = MUTEX_FOOD;
        semop(semid, &sb, 1);
    }
    else
    {
        sb.sem_num = N*(pitid-1)+num;
        semop(semid, &sb, 1);
    }
}

int getCount(int num,int pitid)
{
    return semctl(semid,N*(pitid-1)+num,GETVAL,0);
}

int main(void)
{
    init();

    while(1)
    {
        int ate=0;
        int pitid = genRandom();

        // increase animal inside
        down(MUTEX_COUNT,0);
        up(ANIMAL,pitid);
        if(getCount(ANIMAL,pitid)==1)
            down(PIT,pitid);
        up(MUTEX_COUNT,0);

        // Eat
        down(MUTEX_FOOD,pitid);
        if(getCount(FOOD,pitid)>0)
        {
            down(FOOD,pitid);
            ate=1;
        }
        up(MUTEX_FOOD,pitid);

        // decrease animal inside
        down(MUTEX_COUNT,0);
        down(ANIMAL,pitid);
        if(getCount(ANIMAL,pitid)==0)
            up(PIT,pitid);
        up(MUTEX_COUNT,0);
        
        if(ate)
        {
            printf("Sleeping\n");
            sleep(1);
        }
    }

    return 0;
}