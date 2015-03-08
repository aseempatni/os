#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
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
int chatid;
int idlist[CLIENT_SIZE];
struct message buff;



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

    *nargs=j;
    args[j]=NULL;
}

void displayList()
{
    printf("CHAT IDs:\n");
    for(int i=0;i<CLIENT_SIZE;i++)
        if(idlist[i]!=-1)
            printf("%d\n",idlist[i]);
}

void printargs(char *args[ARR_SIZE]) {
    printf("arg 0 = %s\n", args[0]);
    printf("arg 1 = %s\n", args[1]);
    printf("arg 2 = %s\n", args[2]);
    printf("arg 3 = %s\n", args[3]);
}

int analyse_msg()
{
    char *args[ARR_SIZE];
    size_t nargs;
    parse_msg(buff.mtext,args,ARR_SIZE,&nargs);
    
    if(!strcmp(args[0],"LIST"))
    { // Update client list
        for(int i=1;i<nargs;i++)
            idlist[i-1]=atoi(args[i]);
        printf("Client list updated.\n");
        displayList();
        return 0;
    }
    if(!strcmp(args[0],"MSG"))
    { // f it is a chat message, show the message, sender and time of message
        printf("Got new message. Hurray!!\n");
        printf(" %s: \t%s \n %s\n", args[3], args[1],args[2]);
        // printargs(args);
        return 1;
    }
}

void sendMsg(int recid)
{
    sprintf(buff.mtext,"MSG<%s><%d>",temp,recid);
    if(msgsnd(idup,&buff,strlen(buff.mtext),0)==-1)
    {
        printf("error\n");
        exit(1);
    }
}

void init_chat()
{
    idup=msgget(up,IPC_CREAT|0666);
    iddown=msgget(down,IPC_CREAT|0666);
    for(int i=0;i<CLIENT_SIZE;i++)
        idlist[i]=-1;
    printf("Register with ChatID: ");
    scanf("%d",&chatid);
    strcpy(buff.mtext,"");
    sprintf(buff.mtext,"NEW<%d>",chatid);
    buff.mtype=chatid;
    if(msgsnd(idup,&buff,strlen(buff.mtext),0)==-1)
    {
        printf("error\n");
        exit(1);
    }
    if(msgrcv(iddown,&buff,BUFFER_SIZE,chatid,0)==-1)     //Kernel to user memory space 
    {   
        perror("msgrv failed\n");
        exit(1);
    }
    // printf("Mq:%s\n",buff.mtext);
    analyse_msg();
}

int main()
{
    init_chat();
    char op;
    int recid;
    while(1)
    {
        printf("Entered\n");
        // show list of clients
        displayList();
        printf("Send Message(y/n): ");
        scanf("%*c%c",&op);
        if(op=='y')
        {
            displayList();
            // ask user to pick a client
            printf("Pick Client: ");
            scanf("%d",&recid);
            // ask user to type a message
            printf("Enter Message:\n");
            scanf("%s",temp);
            // send the message using communication protocol
            sendMsg(recid);
        }
        // retrieve a message from message queue
        if(msgrcv(iddown,&buff,BUFFER_SIZE,chatid,0)==-1)     //Kernel to user memory space 
        {   
            perror("msgrv failed\n");
            exit(1);
        }
        printf("Msg: %s\n",buff.mtext);
        analyse_msg();
    }
}