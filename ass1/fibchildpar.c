#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
void main () {
	int n;
	scanf("%d", &n);
	int i, returnStatus;    
	int first = 0, second = 1, next;
	for (i=1; i<=n; i++) {
		pid_t  pid;
		pid = fork0();
		if (pid == 0) {
			if ( i==1 || i==2 )
				next = 1;
			else {
				next = first + second;
			}
			printf("%d\n",next);
			exit(next);
		}
		else {
			waitpid(pid, &returnStatus, 0);
			next = WEXITSTATUS(returnStatus);
			first = second;
			second = next;
		}
	}
}
 