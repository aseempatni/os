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

int main()
{
	int     cp[5][2];
	int     pc[5][2];
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
	char arg[4][15];
	for(i=0;i<5;i++)
	{
		pipe(pc[i]); // parent -> child
		pipe(cp[i]); // child -> parent
		pid = fork();
		if(pid==0)
		{
			sprintf(arg[0],"%d",pc[i][0]);
			sprintf(arg[1],"%d",pc[i][1]);
			sprintf(arg[2],"%d",cp[i][0]);
			sprintf(arg[3],"%d",cp[i][1]);
			// printf("p %d %d %d %d\n",pc[i][0],pc[i][1],cp[i][0],cp[i][1]);
			execl("child",arg[0],arg[1],arg[2],arg[3],(char *)0);
			strerror(errno);
			exit(0);
		}
		else
		{
			// parent
			//printf("Parent\n");
			close(pc[i][0]); // close parent output
			close(cp[i][1]); // close child input
			sprintf(buffer,"%d",i);
			write(pc[i][1], buffer, sizeof(buffer));
		}
	}
	// printf("READY?\n");
	for(i=0;i<5;)
	{
		read(cp[i][0], buffer, sizeof(buffer));
		sscanf(buffer,"%d",&code);
		if(code==READY) {
			i++;
		}
	}
	int k=n/2;
	printf("--- Parent READY\n");
	while(1)
	{
		pivot=-1;
		while(pivot==-1)
		{
			id = rand()%5;
			printf("--- Parent sends REQUEST to child %d.\n",id+1);
			sprintf(buffer,"%d",REQUEST);
			write(pc[id][1], buffer, sizeof(buffer));
			read(cp[id][0],buffer,sizeof(buffer));
			sscanf(buffer,"%d",&pivot);
			printf("--- Child %d sends %d to parent.\n", id+1,pivot);
			// printf("Pivot %d\n",pivot);
			//getchar();
		}
		printf("--- Parent broadcasts pivot %d to all children.\n", pivot);
		for(i=0;i<5;i++)
		{
			sprintf(buffer,"%d",PIVOT);
			write(pc[i][1], buffer, sizeof(buffer));
			sprintf(buffer,"%d",pivot);
			write(pc[i][1], buffer, sizeof(buffer));
		}
		int clarge=0,temp;
		printf("--- Parent: m=");
		for(i=0;i<5;i++)
		{
			read(cp[i][0],buffer,sizeof(buffer));
			sscanf(buffer,"%d",&temp);
			clarge+=temp;
			printf("%d", temp);
			if (i!=4) printf("+");
		}
		printf("=%d \n",clarge);
		if(clarge==k)
		{
			printf("m=k. Median found!\n");
			printf("--- Parent sends kill signal to all the children.\n");
			for(i=0;i<5;i++)
			{
				sprintf(buffer,"%d",EXIT);
				write(pc[i][1], buffer, sizeof(buffer));
			}
			break;
		}
		else if(clarge > k)
		{
			printf("m>k. Median is greater than %d.\n", clarge);
			printf("--- Parent asks children to drop all emements smaller than pivot, %d\n",pivot);
			for(i=0;i<5;i++)
			{
				sprintf(buffer,"%d",SMALL);
				write(pc[i][1], buffer, sizeof(buffer));
			}
		}
		else
		{
			printf("m<k. Median is less than %d.\n",clarge);
			printf("--- Parent asks children to drop all emements larger than pivot, %d\n",pivot);
			for(i=0;i<5;i++)
			{
				sprintf(buffer,"%d",LARGE);
				write(pc[i][1], buffer, sizeof(buffer));
			}
			k = k - clarge - 1;
		}
		// printf("Left %d\n",k);
	}

	// printf("Waiting for Child\n");
	int status;
	for(i=0;i<5;i++)
		wait(&status);
	printf("The Median of data is %d.\n",pivot);
}
