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
	
	int pid, pidfiglio, exitval, n;
	int status;
	srand(time(NULL));
	
	if(argc != 2){
		printf("Numero sbagliato di param\n");
		exit(1);
	}
	
	n = atoi(argv[1]);
	
	if(n < 0 || n > 155){
		printf("Il numero deve essere compreso fra 0 e 155\n");
		exit(2);
	}
	
	printf("Pid del processo corrente: %d\nNumero n: %d\n", getpid(), n);
	
	
	for(int i = 0; i < n; i++){
		if((pid = fork()) < 0){
			printf("Errore nella fork\n");
			exit(1);
		}
	
		if(pid == 0){
			printf("Sono il processo %d-esimo con pid: %d\n", i, getpid());
			int rnd;
			rnd = rand() % (100+i);
			exit(rnd);
		}
		else{
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
				printf("Il figlio %d-esimo con pid %d ha ritornato: %d\n", i, pidfiglio, exitval);
			}
		}
	}
	

	

	
}


