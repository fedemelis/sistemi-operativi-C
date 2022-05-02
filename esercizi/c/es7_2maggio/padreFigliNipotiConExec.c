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
	
	int fd;
	int pidfiglio;
	int exitval;
	int pid;
	int status;
	char nomefile[BUFSIZ];
	int outfile;
	
	if(argc < 4){
		printf("numero sbagliato di param\n");
		exit(1);
	}
	
	for(int i = 1; i < argc; i++){
		if((fd = open(argv[i], O_RDONLY)) < 0){
			printf("Tutti i parametri devono essere percorsi ass\n");
			exit(2);
		}	
	}
	
	for(int i = 0; i < (argc - 1); i++){
		if((pid = fork()) < 0){
			printf("Errore nella fork");
			exit(4);
		}
		if(pid == 0){
			strcat(nomefile, argv[i+1]);
			strcat(nomefile, ".sort");
			printf("%s\n", nomefile);
			if((outfile = creat(nomefile, PERM)) < 0){
				printf("errore in fase di creazione\n");
				exit(9);
			}
			if((pidfiglio = fork()) < 0){
				printf("Errore nella fork");
				exit(4);
			}
			if(pidfiglio == 0){
				close(1);
				open(nomefile, O_WRONLY);
				execlp("sort", "sort", argv[i+1], (char *) 0);
				exit(-1);
			}
			if((pidfiglio = wait(&status)) < 0){
				printf("Errore nella wait\n");
				exit(5);
			}		
			if((status & 0xFF) != 0){
				printf("Errore nella chiusura del figlio\n");
				exit(6);
			}
			else{
				exitval = status >> 8;
				exitval &= 0xFF;
				exit(exitval);
			}
		}
		if((pidfiglio = wait(&status)) < 0){
			printf("Errore nella wait\n");
			exit(5);
		}
	
		if((status & 0xFF) != 0){
			printf("Errore nella chiusura del figlio\n");
			exit(6);
		}
		
		exitval = status >> 8;
		exitval &= 0xFF;
		printf("Il figlio con pid: %d ha ritornato %d\n", pidfiglio, exitval);
	
	}
	
}


