#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>
#define PERM 0644

int main(int argc, char **argv){
	
	
	int pid, pidFiglio, status, exitval;
	
	if(argc != 3){
		printf("Errore nel numero di parametri\n");
		exit(1);
	}
	
	if((pid = fork()) < 0){
		printf("Errore nella fork\n");
		exit(2);
	}
	
	if(pid == 0){
		printf("Pid processo: %d\nPid padre: %d\n", getpid(), getppid());
		close(1);
		open("/dev/null", O_WRONLY);
		close(2);
		open("/dev/null", O_WRONLY);
		close(0);
		open(argv[2], O_RDONLY);
		execlp("grep", "grep", argv[1], (char *) 0);
		exit(-1);
	}
	
	if((pidFiglio = wait(&status)) < 0){
		printf("Errore nella wait\n");
		exit(2);
	}
	
	if((status & 0xFF) != 0){
		printf("Errore nella chiusura del figlio\n");
		exit(3);
	}
	else{
		exitval = status >> 8;
		exitval &= 0xFF;
		printf("Il figlio con pid %d ha ritornato: %d\n", pidFiglio, exitval);
	}
	
	exit(0);
	
}


