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
	char c;
	int nread;
	char buffer;
	int pidfiglio;
	int exitval;
	int pid;
	int status;
	
	if(argc < 4){
		printf("numero sbagliato di param\n");
		exit(1);
	}
	
	for(int i = 1; i < (argc-1); i++){
		if((fd = open(argv[i], O_RDONLY)) < 0){
			printf("Tutti i parametri tranne l'ultimo devono essere nomi assoluti di directory\n");
			exit(2);
		}	
	}
	
	if(argv[argc-1][1] != 0){
		printf("L'ultimo param deve essere un singolo carattere\n");
		exit(3);
	}
	
	c = argv[argc-1][0];
	
	for(int i = 0; i < (argc - 2); i++){
		if((pid = fork()) < 0){
			printf("Errore nella fork");
			exit(4);
		}
		if(pid == 0){
			int occorrenze = 0;
			fd = open(argv[i+1], O_RDONLY);
			while((nread = read(fd, &buffer, 1)) > 0){
				if(c == buffer){
					occorrenze += 1;
				}
			}
			printf("%d\n", occorrenze);
			exit(occorrenze);
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
			printf("Il figlio con pid %d ha trovato %d occorrenze\n", pidfiglio, exitval);
		}
	}
	
}


