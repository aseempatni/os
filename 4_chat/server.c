#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 1000
#define ARR_SIZE 100
#define CLIENT_SIZE 10
char temp[BUFFER_SIZE];
struct message
{   
        long mtype;
        char mtext[BUFFER_SIZE];
};
key_t up = 131;
key_t down = 132;
int idup;
int iddown;
int map[100][2];
char chatID[100][20];

struct message msg;

int nclient() {
    for(int i=0;i<CLIENT_SIZE;i++)
    {
        if(map[i][0]==-1)
        {
            return i;
        }
    }
}

void init_chat()
{
    idup=msgget(up,IPC_CREAT|0666);
    iddown=msgget(down,IPC_CREAT|0666);
    msgctl(up, IPC_RMID, NULL);
    msgctl(down, IPC_RMID, NULL);
    for(int i=0;i<CLIENT_SIZE;i++)
        map[i][0]=-1;
}

int sender_pid()
{
    struct msqid_ds qstat;
    if(msgctl(idup,IPC_STAT,&qstat)<0)
    {
        perror("msgctl failed");
        exit(1);
    }
    return qstat.msg_lspid;
}

char* sender_chatid ()
{
    int pid = sender_pid();
    for(int i=0;i<CLIENT_SIZE;i++)
    {
        if(map[i][0]==pid)
        {
            return chatID[i];
        }
    }
}
void get_msg_time(char* buff)
{
    struct msqid_ds qstat;
    if(msgctl(idup,IPC_STAT,&qstat)<0)
    {
        perror("msgctl failed");
        exit(1);
    }
    time_t now = qstat.msg_stime;
    strftime(buff, 20, "%H:%M:%S", localtime(&now));
    // strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    // return buff;
}

//* registration
int addClient(int pid,char* chatid)
{
    for(int i=0;i<CLIENT_SIZE;i++)
    {
        if(map[i][0]==-1 || strcmp(chatID[i],chatid)==0 )
        {
            map[i][0]=pid;
            map[i][1]=atoi(chatid);
            strcpy (chatID[i],chatid);
            printf(" Chat ID: %s \n PID: %d\n", chatid,pid);
            return 1;
        }
    }
    return -1;
}

void disp_clients() {
    printf("Chat IDs: \n");
    for(int i=0;i<CLIENT_SIZE;i++)
    {
        printf("%s\t",chatID[i] ); 
        if(map[i][0]==-1)
        {
            break;
        }
    }
    printf("\n");
}

void sendList()
{
    strcpy(msg.mtext,"");
    sprintf(msg.mtext,"LIST");
    for(int i=0;i<CLIENT_SIZE;i++)
        if(map[i][0]!=-1)
        {
            sprintf(temp,"<%s>",chatID[i]);
            strcat(msg.mtext,temp);
        }
    for(int i=0;i<CLIENT_SIZE;i++)
    {
        if(map[i][0]!=-1)
        {
            msg.mtype=map[i][0];
            if(msgsnd(iddown,&msg,strlen(msg.mtext),0)==-1)
            {
                printf("error\n");
                exit(1);
            }
        }
    }
}

void parse_msg(char *buffer,char** args,size_t args_size, size_t *nargs)
{
    char *buf_args[args_size]; /* You need C99 */
    char **cp;
    char *wbuf;
    size_t i, j;

    wbuf=buffer;
    buf_args[0]=buffer; 
    args[0] =buffer;

    for(cp=buf_args; (*cp=strsep(&wbuf, "<>")) != NULL ;){
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))
            break;
    }

    for (j=i=0; buf_args[i]!=NULL; i++){
        if(strlen(buf_args[i])>0)
        {
            args[j++]=buf_args[i];
        }
    }
    // printf("buffer:%s\n",buffer);
    *nargs=j;
    args[j]=NULL;
}

void addTime()
{

}

int qsize () {
    struct msqid_ds buf;
    int rc = msgctl(iddown, IPC_STAT, &buf);
    uint msg = (uint)(buf.msg_qnum);
    printf("# messages in queue: %u\n", msg);
}

void printargs(char *args[ARR_SIZE]) {
    printf("arg 0 = %s\n", args[0]);
    printf("arg 1 = %s\n", args[1]);
    printf("arg 2 = %s\n", args[2]);
    printf("arg 3 = %s\n", args[3]);
}
void relayMsg(int pid)
{
    msg.mtype=pid;
    if(msgsnd(iddown,&msg,strlen(msg.mtext),0)==-1)
    {
        printf("error\n");
        exit(1);
    }
    // printf("sent to %d\n", chatid);
}

int analyse_msg()
{
    char *args[ARR_SIZE];
    size_t nargs;
    parse_msg(msg.mtext,args,ARR_SIZE,&nargs);
    if(!strcmp(args[0],"NEW"))
    { // when a client joins, display new client's chat ID (described below) and process ID, total number of clients
        printf("=== New Client\n");
        addClient(sender_pid(),args[1]);
        printf(" Total clients: %d\n", nclient());
        sendList();
        return 0;
    }
    if(!strcmp(args[0],"MSG"))
    {
        printf("=== New Message\n");
        printf(" From: %s \n To: %s \n Message: %s\n", sender_chatid(), args[2], args[1]);
        char time_msg[20];
        get_msg_time(time_msg);
        // printargs(args);
        addTime();
        int i=0;
        int to = atoi(args[2]);
        while(map[i][0]!=-1 && strcmp(chatID[i],args[2])!=0) i++;
        if ( map[i][0]==-1) 
            printf("Reciever not found.\n");
        else {

        }

        // Add message
        sprintf(msg.mtext,"MSG<%s>",args[1]);

        // Add time and sender chatID
        sprintf(temp,"<%s><%s>",time_msg, sender_chatid());
        strcat(msg.mtext,temp);
        printf("=> %s to %d\n", msg.mtext,to);
        relayMsg(map[i][0]);
        qsize();
        return 1;
    }
}

int main()
{
    init_chat();
    int chatid;
    while(1)
    {
        if(msgrcv(idup,&msg,BUFFER_SIZE,0,0)==-1)     //Kernel to user memory space 
        {
            perror("msgrv failed\n");
            exit(1);
        }
        printf("\n<= %s\n",msg.mtext);
        analyse_msg();
        disp_clients();
    }
}