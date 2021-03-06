#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define sem1 0
#define sem2 1

struct message
{
        long mtype;
        char mtext[BUFFER_SIZE];
};
struct sembuf *sb;

char buffer[BUFFER_SIZE];
struct message msgmq;
key_t broad_key=31,array_key=32,msg_key=33,sem_key=34;
int mqid,asid,msid,semid;
int* pidarr;
char *msgshm;
int pid;

void lock(int num)
{
    int nop;
    sb[0].sem_flg = 0;
    sb[0].sem_num = num;
    sb[1].sem_flg = 0;
    sb[1].sem_num = num;

    sb[0].sem_op = -1;
    nop = 1;
    if(num==sem2)
    {
        sb[0].sem_op = 0;
        sb[1].sem_op = 1;
        nop = 2;
    }
    semop(semid, sb, nop);
}

void unlock(int num)
{
    int nop;
    sb[0].sem_flg = 0;
    sb[0].sem_num = num;
    sb[0].sem_op = 1;
    nop=1;
    semop(semid, sb, nop);
}

void printsem(int p)
{
    int a = semctl(semid,sem1,GETVAL,0);
    int b = semctl(semid,sem2,GETVAL,0);
    //printf("%d sem1:%d sem2:%d\n",p,a,b);
}

void init()
{
    sb = (struct sembuf *)malloc(2*sizeof(sembuf));
    mqid = msgget(broad_key,IPC_CREAT|0666);
    asid = shmget(array_key,500,IPC_CREAT|0666);
    msid = shmget(msg_key,5000,IPC_CREAT|0666);
    semid = semget(sem_key,2,IPC_CREAT|0666);
    pidarr = (int*)shmat(asid,NULL,0);
    msgshm = (char*)shmat(msid,NULL,0);
    printf("Device (pid: %d)\n",getpid());
}

void tokenize(char *str,char** tok)
{
    char * pch;
    pch = strtok (str,"/:");
    int i=0;
    while (pch != NULL)
    {
        if(strlen(pch)>0)
            tok[i++] = pch;
        pch = strtok (NULL, "/:");
    }
    tok[i]=NULL;
}

void release()
{
    //printf("cpid %d\n",pid);
    printf("Client Exit\n");
    int status;
    
    sleep(1);
    int idx=0,count=0;
    lock(sem1);
    for(int i=0;i<50;i++)
    {
        if(pidarr[i]!=-1)
        {
            count++;
            printf("cc %d\n",pidarr[i]);
            if(pidarr[i]==getpid())
            {
                idx = i;
            }
        }
    }
    printf("idx %d\n",idx);
    pidarr[idx]=-1;
    shmdt(pidarr);
    unlock(sem1);
    strcpy(buffer,"*");
    printf("Count %d\n",count);
    if(count==1)
    {
        lock(sem2);
        strcpy(msgshm,buffer);
        unlock(sem2);
    }
    shmdt(msgshm);
    wait(&status);
    exit(0);
}

void check()
{
    if( access( "ser.txt", F_OK ) == -1 ) {
         printf("--- Server Absent\n");
         return;
    }
}

bool flag;
void sendHandle(int sig)
{
    flag = true;
    char temp[700];
    printf("<Ctrl+C is pressed>\n");
    printf("--- Enter a message:\n");
    fflush(stdin);
    scanf("\n%[^\n]s",temp);
    if(!strcmp(temp,"bye"))
        release();
    sprintf(buffer,"%s/%d:%s",getlogin(),getpid(),temp);
}

void send()
{
    signal(SIGINT,sendHandle);
    flag = false;
    while(1)
    {
        //printf("hey\n");
        sleep(1);
        if(!flag)
            sprintf(buffer,".");
        flag=false;
        //printf("Stuck at sem2\n");
        printsem(0);
        signal(SIGINT,SIG_IGN);
        lock(sem2);
        
        printsem(1);
        //printf("Copying to shared variable\n");
        strcpy(msgshm,buffer);
        //printf("Done\n");
        
        unlock(sem2);
        signal(SIGINT,sendHandle);
        printsem(2);
        //getchar();
    }
}

void addPPID()
{
    printsem(5);
    lock(sem1);
    for(int i=0;i<50;i++)
        if(pidarr[i]==-1)
        {
            printf("index %d\n",i);
            pidarr[i]=getppid();
            break;
        }
    unlock(sem1);
}

void receive()
{
    signal(SIGINT,SIG_IGN);
    addPPID();

    while(1)
    {
        msgrcv(mqid,&msgmq,sizeof(msgmq.mtext),getppid(),0);
        printf("--- Received message: %d a%sa\n",pid,msgmq.mtext);
    }
}

void exitall(int sig)
{
    //for(int i=0;i<50;i++)
    //    kill(SIGKILL,pidarr[i]);
    //release();
    //printf("hey kill");
    exit(0);
}

int main(int argc,char* argv[])
{
    signal(SIGTSTP,exitall);
    check();
    init();
    pid = fork();
    //printf("pid %d\n",pid);
    if(pid)
        send();
    else
    {
        receive();
    }
}
