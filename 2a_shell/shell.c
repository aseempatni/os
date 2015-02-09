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

/**
*mkdir - mkdir - done
*rmdir - rmdir - done
*cp - 
*ls - done
*cd - chdir - done
*pwd - getcwd - done
*exit - exit - done
**/

static char perms_buff[30];

const char *get_perms(mode_t mode)
{
    char ftype = '?';

    if (S_ISREG(mode)) ftype = '-';
    if (S_ISLNK(mode)) ftype = 'l';
    if (S_ISDIR(mode)) ftype = 'd';
    if (S_ISBLK(mode)) ftype = 'b';
    if (S_ISCHR(mode)) ftype = 'c';
    if (S_ISFIFO(mode)) ftype = '|';

    sprintf(perms_buff, "%c%c%c%c%c%c%c%c%c%c %c%c%c", ftype,
        mode & S_IRUSR ? 'r' : '-',
        mode & S_IWUSR ? 'w' : '-',
        mode & S_IXUSR ? 'x' : '-',
        mode & S_IRGRP ? 'r' : '-',
        mode & S_IWGRP ? 'w' : '-',
        mode & S_IXGRP ? 'x' : '-',
        mode & S_IROTH ? 'r' : '-',
        mode & S_IWOTH ? 'w' : '-',
        mode & S_IXOTH ? 'x' : '-',
        mode & S_ISUID ? 'U' : '-',
        mode & S_ISGID ? 'G' : '-',
        mode & S_ISVTX ? 'S' : '-');

    return (const char *)perms_buff;
}

char pathname[MAXPATHLEN];

void die(char *msg)
{
    perror(msg);
    exit(0);
}

static int one (const struct dirent *unused)
{
    if(unused->d_name[0]=='.')
        return 0;
    else return 1;
}

void do_ls(char* args[],int nargs)
{
    int count,i;
    struct direct **files;
    struct stat statbuf;
    char datestring[256];
    struct passwd pwent;
    struct passwd *pwentp;
    struct group grp;
    struct group *grpt;
    struct tm time;
    char buf[1024];

    if(!getcwd(pathname, sizeof(pathname)))
        printf("Error: %s",strerror(errno));

    count = scandir(pathname, &files, one, alphasort);
    if(count > 0)
    {
        printf("total %d\n",count);

        for (i=0; i<count; ++i)
        {
            if (stat(files[i]->d_name, &statbuf) == 0)
            {
                if(nargs>1 && !strcmp(args[1],"-l"))
                {
                    /* Print out type, permissions, and number of links. */
                    printf("%10.10s", get_perms(statbuf.st_mode));
                    printf(" %u", statbuf.st_nlink);

                    if (!getpwuid_r(statbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
                        printf(" %s", pwent.pw_name);
                    else
                        printf(" %d", statbuf.st_uid);

                    if (!getgrgid_r (statbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
                        printf(" %s", grp.gr_name);
                    else
                        printf(" %d", statbuf.st_gid);

                    /* Print size of file. */
                    printf(" %5d", (int)statbuf.st_size);

                    localtime_r(&statbuf.st_mtime, &time);
                    /* Get localized date string. */
                    strftime(datestring, sizeof(datestring), "%I:%M%p : %x", &time);

                    printf(" %s %s\n", datestring, files[i]->d_name);
                }
                else
                {
                    printf("%s\t", files[i]->d_name);
                }
            }

            free (files[i]);
        }

        free(files);
        printf("\n");
    }
}

void parse_args(char *buffer, char** args,size_t args_size, size_t *nargs,int *bgp)
{
    char *buf_args[args_size]; /* You need C99 */
    char **cp;
    char *wbuf;
    size_t i, j;

    wbuf=buffer;
    buf_args[0]=buffer; 
    args[0] =buffer;

    for(cp=buf_args; (*cp=strsep(&wbuf, " \n\t")) != NULL ;){
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))
            break;
    }
    *bgp = 0;
    for (j=i=0; buf_args[i]!=NULL; i++){
        if(strlen(buf_args[i])>0)
        {
            if(!strcmp(buf_args[i],"&"))
            {
                *bgp = 1;
                continue;
            }
            args[j++]=buf_args[i];
        }
    }

    *nargs=j;
    args[j]=NULL;
}

void parse_path(char** path,int *npath)
{
    char *buf_args[10]; /* You need C99 */
    char **cp;
    char *wbuf;
    size_t i, j;
    
    wbuf=getenv("PATH");
    
    buf_args[0]=wbuf;
    
    path[0] =strcat(getenv("PWD"),"/");
    
    for(cp=buf_args; (*cp=strsep(&wbuf, ":\n")) != NULL ;){
        if ((*cp != '\0') && (++cp >= &buf_args[10]))
            break;
    }
    
    for (j=1,i=0; buf_args[i]!=NULL; i++){
        if(strlen(buf_args[i])>0)
        {
            path[j++]=strcat(buf_args[i],"/");
        }
    }
    *npath=j;
    path[j]=NULL;
}

int path_given(char* prog)
{
    int i=0,result=0;
    while(prog[i]!='\0')
    {
        if(prog[i]=='\\')
        {
            result=1;
        }
        i++;
    }
    return result;
}

void execute(char* prog,char** args)
{
    int ret_status;
    int i;
    char *path[10];
    char* temp;
    int npath;
    if(!fork())
    {
        if(path_given(prog))
            execv(prog, args);
        else
        {
            parse_path(path,&npath);
            for(i=0;i<npath;i++)
            {
                temp = strcat(path[i],prog);
                execv(temp, args);
            }
        }
        puts(strerror(errno));
        exit(0);
    }
    else
        wait(&ret_status);
}

void copy(char* file1,char* file2)
{
    char ch;
    FILE* f1,*f2;
    f1 = fopen(file1,"r");
    f2 = fopen(file2,"w");
    if(f2==NULL)
	printf("File doesn't exist\n");
    while((ch=getc(f1))!=EOF)
    {
        putc(ch,f2);
    }
    fclose(f1);
    fclose(f2);
}

void do_cp(char** args)
{
    char* file1,*file2;
    struct stat at1;
    struct stat at2;
    file1 = args[1];
    file2 = args[2];
    if( access( file1, F_OK ) == -1 ) {
         printf("%s doesn't exists\n",file1);
         return;
    }
    else if( access( file1, R_OK ) == -1 )
    {
         printf("No read permission on %s",file1);
         return;
    }
    else if(access( file2, F_OK ) == -1 )
    {
         copy(file1,file2);
    }
    else if( access( file2, W_OK ) == -1 )
    {
         printf("No write permission on %s",file2);
         return;
    }
    else
    {
        stat(file1, &at1);
        stat(file2, &at2);
         // Time stamp comparison
        if(difftime(at1.st_mtime,at2.st_mtime)<0)
            printf("Error: %s is newer than %s\n",file1,file2);
        copy(file1,file2);
    }
}

int main(){
    char buffer[BUFFER_SIZE];
    char *args[ARR_SIZE];
    int bgp;

    int *ret_status;
    size_t nargs;
    pid_t pid;
    int i;

    while(1){
        getcwd(buffer, sizeof(buffer));
        printf("%s> ", buffer);
        fgets(buffer, BUFFER_SIZE, stdin);
        parse_args(buffer, args, ARR_SIZE, &nargs,&bgp); 

        if (nargs==0) continue;

        if (bgp && fork()){
            continue;
        } else {
            if (!strcmp(args[0], "exit" ))
            {
                exit(0);
            }
            else if(!strcmp(args[0],"pwd"))
            {
                getcwd(buffer, sizeof(buffer));
                printf("%s\n", buffer);
            }
            else if(!strcmp(args[0],"cd"))
            {
                if(chdir(args[1]))
                {
                    printf("Error: %s\n",strerror(errno));
                }

            }
            else if(!strcmp(args[0],"ls"))
            {
                do_ls(args,nargs);
            }
            else if(!strcmp(args[0],"mkdir"))
            {
                if(mkdir(args[1],0777))
                {
                    printf("Error: %s\n",strerror(errno));
                }
            }
            else if(!strcmp(args[0],"rmdir"))
            {
                if(rmdir(args[1]))
                {
                    printf("Error: %s\n",strerror(errno));
                }
            }
            else if(!strcmp(args[0],"cp"))
            {
                if(nargs==3)
                    do_cp(args);
                else if(nargs<3)
                    printf("Error: Less arguement passed\n");
                else printf("Error: More arguement passed\n");
            }
            else{
                execute(args[0],args);
            }
            if(bgp)
                exit(0);
        }
    }
    return 0;
}
