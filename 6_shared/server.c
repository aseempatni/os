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
#include <time.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>

#define BUFFER_SIZE 1024
#define sem1 0
#define sem2 1


void perror(char* s)
{
    printf("%s",s);
    exit(0);
}

struct message
{
        long mtype;
        char mtext[BUFFER_SIZE];
};
struct sembuf sb;

char buffer[BUFFER_SIZE];
struct message msgmq;
key_t broad_key=31,array_key=32,msg_key=33,sem_key=34;
int mqid,asid,msid,semid;
int* pidarr;
char *msgshm;

void lock(int num)
{
    sb.sem_op = -1;
    sb.sem_flg = 0;
    sb.sem_num = num;
    if(num==sem2)
        sb.sem_op = -2;
    semop(semid, &sb, 1);
}

void unlock(int num)
{
    sb.sem_op = 1;
    sb.sem_flg = 0;
    sb.sem_num = num;
    if(num==sem2)
        sb.sem_op = 2;
    semop(semid, &sb, 1);
}

void writepid()
{
    int fd;
    // |O_EXCL
    if((fd=open("ser.txt",O_CREAT|O_WRONLY,0666))==-1)
        perror("Only one server can run.\n");
    printf("Opening File\n");
    sprintf(buffer,"%d",getpid());
    printf("Server PID: %s\n",buffer);
    write(fd,buffer,strlen(buffer));
    close(fd);
}

int present(char *u,char * user[])
{
    int i=0;
    while(user[i]!=NULL)
    {
        if(!strcmp(u,user[i]))
        {
            printf("Allowed: %s\n",user[i]);
            return i;
        }
        i++;
    }
    return -1;
}

void verifyUser(int *n,char* user[])
{
    char* tempUser[50];
    FILE *fuser  = fopen("/var/run/utmp","r");
    struct utmp u;
    int i=0,idx;
    while(user[i]!=NULL)
    {
        tempUser[i]=user[i];
        i++;
    }
    i=0;
    while(!feof(fuser))
    {
        fread(&u, sizeof(u), 1, fuser);
        if(getpwnam(u.ut_user)!=NULL)
        {
            if((idx=present(u.ut_user,tempUser)) > -1)
            {
                printf("%s\n",u.ut_line);
                sprintf(buffer, "./commence > /dev/%s", u.ut_line);
                system(buffer);
                user[i++]=strdup(tempUser[idx]);
                //sprintf(tempUser[idx],"\n");
            }
        }
    }
    user[i]=NULL;
    *n = i;
}

void init()
{
    mqid = msgget(broad_key,IPC_CREAT|0666);
    asid = shmget(array_key,500,IPC_CREAT|0666);
    msid = shmget(msg_key,5000,IPC_CREAT|0666);
    semid = semget(sem_key,2,IPC_CREAT|0666);
    pidarr = (int*)shmat(asid,NULL,0);
    msgshm = (char*)shmat(msid,NULL,0);
    semctl(semid,sem2,SETVAL,0);
    writepid();
    for(int i=0;i<50;i++)
        pidarr[i]=-1;
    printf("--- Initialization complete\n");
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

char* msg[5];
int read_message()
{
    lock(sem2);
    strcpy(buffer,msgshm);
    unlock(sem2);
    tokenize(buffer,msg);
    if(!strcmp(msg[2],"."))
        return 0;
    if(!strcmp(msg[2],"*"))
        return -1;
    return 1;
}

void broadcast(int expid)
{
    strcpy(msgmq.mtext,buffer);
    for(int i=0;i<50;i++)
    {
        if(pidarr[i]!=-1 && pidarr[i]!=expid)
        {
            msgmq.mtype = pidarr[i];
            msgsnd(mqid,&msgmq,strlen(msgmq.mtext),0);
        }
    }
}

void release()
{
    msgctl(mqid,IPC_RMID,NULL);
    shmctl(asid,IPC_RMID,NULL);
    shmctl(msid,IPC_RMID,NULL);
    semctl(semid,0,IPC_RMID,0);
    exit(0);
}

int main(int argc,char* argv[])
{
    init();
    pidarr[0]=2;
    int nuser=argc;
    int result;
    char **user = &argv[1];
    verifyUser(&nuser,user);

    while(1)
    {
        result = read_message();
        if(result==1)
            broadcast(atoi(msg[1]));
        if(result==-1)
            release();
    }
}