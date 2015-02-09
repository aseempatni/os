#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <fcntl.h>
#define BUFFER_SIZE 1<<16
#define ARR_SIZE 1<<16

#define REQUEST 100
#define PIVOT 200
#define LARGE 300
#define SMALL 400
#define READY 500
#define ERROR -1
#define EXIT -100

void del_small(int* arr,int* valid,int n,int pivot)
{
    int i;
    for(i=0;i<n;i++)
    {
        if(arr[i]<=pivot)
            valid[i]=0;
    }
}

void del_large(int* arr,int* valid,int n,int pivot)
{
    int i;
    for(i=0;i<n;i++)
    {
        if(arr[i]>=pivot)
            valid[i]=0;
    }
}

void left(int* arr,int* valid,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
        // printf("(%d,%d) ",arr[i],valid[i]);
    }
}

int novalid(int* valid,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
        if(valid[i])
            return 0;
    }
    return 1;
}


int find_large(int* arr,int *valid,int n,int pivot)
{
    int i,count=0;
    for(i=0;i<n;i++)
    {
        if(arr[i]>pivot && valid[i])
            count++;
    }
    return count;
}

int main(int argc, char const *argv[])
{
    int     cp[2];
    int     pc[2];
    pc[0] = atoi(argv[0]);
    pc[1] = atoi(argv[1]);
    cp[0] = atoi(argv[2]);
    cp[1] = atoi(argv[3]);
    
    int code,n=25;
    int arr[5];
    int valid[5];
    pid_t   childpid;
    char    buffer[80];
    FILE *f;
    int j;
    for(j=0;j<5;j++)
        arr[j]=-1;

    int i=1;
    int id,pid,pivot;
    int cno;

    close(pc[1]); // close parent input
    close(cp[0]); // close child output
    read(pc[0], buffer, sizeof(buffer));
    //printf("pipe: %s\n",buffer);
    sscanf(buffer,"%d",&id);
    sprintf(buffer,"data_%d.txt",id+1);
    // printf("%s\n",buffer);
    cno = id+1;
    if(!(f = fopen(buffer,"r")))
    {
        strerror(errno);
        exit(0);
    }
    //printf("Reading file\n");
    j=0;
    while(!feof(f))
    {
        fscanf(f,"%d",&arr[j++]);
        valid[j-1]=1;
    }
    printf("--- Child %d sends READY\n", cno);
    sprintf(buffer,"%d",READY);
    write(cp[1], buffer, sizeof(buffer));
    srand((unsigned)time(NULL));
    int op;
    while(1)
    {
        //printf("Waiting for signal\n");
        read(pc[0], buffer, sizeof(buffer));
        sscanf(buffer,"%d",&op);
        
        // printf("opertion %d\n",op);
        switch(op)
        {
            case REQUEST:
            //printf("j: %d\n",j);
            if(novalid(valid,5))
            {
                sprintf(buffer,"%d",ERROR);
                write(cp[1], buffer, sizeof(buffer));
            }
            else
            {
                while(1)
                {
                    id= rand()%5;
                    if(valid[id])
                        break;
                }
                sprintf(buffer,"%d",arr[id]);
                write(cp[1], buffer, sizeof(buffer));
            }
            break;
            case PIVOT:
            read(pc[0], buffer, sizeof(buffer));
            sscanf(buffer,"%d",&pivot);
            printf("--- Child %d receives pivot and replies %d\n",cno,find_large(arr,valid,5,pivot));
            sprintf(buffer,"%d",find_large(arr,valid,5,pivot));
            write(cp[1], buffer, sizeof(buffer));

            break;
            case SMALL:
            // printf("small del\n");
            del_small(arr,valid,5,pivot);
            left(arr,valid,5);
            break;
            case LARGE:
            // printf("large del\n");
            del_large(arr,valid,5,pivot);
            left(arr,valid,5);
            break;
            case EXIT:
                printf("--- Child %d terminates\n", cno);
            fclose(f);
            exit(0);

        }
    }
}
