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
	
	int pid, pidfiglio, exitval;
	int status;
	srand(time(NULL));
	
	printf("Pid del processo padre: %d\n", getpid());
	
	if((pid = fork()) < 0){
		printf("Errore nella fork\n");
		exit(1);
	}
	
	if(pid == 0){
		printf("Pid processo: %d\n Pid padre: %d\n", getpid(), getppid());
		int casuale;
		casuale = rand() % 100;
		exit(casuale);	
	}
	
	if((pidfiglio = wait(&status)) < 0){
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
		printf("Il figlio con pid %d ha ritornato: %d\n", pidfiglio, exitval);
	}
	
	
	
	

	
}


