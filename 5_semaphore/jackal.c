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
#define ANIMAL JACKAL
#define MUTEX_FOOD JACKAL_FOOD_LOCK
#define MUTEX_COUNT JACKAL_COUNT_LOCK

key_t key=21;
int semid;
struct sembuf sb;

int genRandom()
{
    return rand()%3+1;
}

void init()
{
    semid = semget(key,50,0666|IPC_CREAT);
}

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

int main(void)
{
    init();
    int n,i,ate=1,pitid;
    printf("Enter the number of Jackals: ");
    scanf("%d",&n);
    for(i=0;i<n;i++)
    {
        if(fork())
            break;
    }
    srand(time(NULL));
    while(1)
    {
        if(ate) pitid = genRandom(),ate=0;
        else pitid = pitid%3+1;

        // increase animal inside
        down(MUTEX_COUNT,pitid);
        printf("Jackal: Count %d\n",getCount(ANIMAL,pitid));
        up(ANIMAL,pitid);
        if(getCount(ANIMAL,pitid)==1)
        {
            printf("Jackal: Peeing inside %d\n",pitid);
            down(PIT,pitid);
        }
        up(MUTEX_COUNT,pitid);

        // Eat
        printf("Jackal: Lock %d:%d\n",pitid,getCount(PIT,pitid));
        down(MUTEX_FOOD,pitid);
        if(getCount(FOOD,pitid)>0)
        {
            printf("Jackal: Eating Food\n");
            down(FOOD,pitid);
            ate=1;
        }
        printf("Jackal: Food left %d\n",semctl(semid,N*(pitid-1)+FOOD,GETVAL,0));
        up(MUTEX_FOOD,pitid);
        //getchar();

        // decrease animal inside
        down(MUTEX_COUNT,pitid);
        down(ANIMAL,pitid);
        if(getCount(ANIMAL,pitid)==0)
            up(PIT,pitid);
        up(MUTEX_COUNT,pitid);
        
        if(ate)
        {
            printf("Jackal: Sleeping\n");
            sleep(1);
        }
    }

    return 0;
}