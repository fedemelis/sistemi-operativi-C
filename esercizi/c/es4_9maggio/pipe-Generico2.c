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
#define MESGSIZE 5


int main(int argc, char **argv){

	int piped[2];
	int pid;
	char mess[512];
	
	int pidfiglio, status, ritorno;
	
	if(argc != 2){
		printf("Errore nel numero di param\n");
		exit(1);
	}
	
	
	if(pipe(piped) < 0){
		printf("Errore in fase di pipe\n");
		exit(2);
	}
	
	if((pid = fork()) < 0){
		printf("Errore durante la creazione del figlio\n");
		exit(3);
	}
	
	if(pid == 0){
		int fd;
		close(piped[0]);
		
		if((fd = open(argv[1], O_RDONLY)) < 0){
			printf("Errore durante l'apertura del file\n");
			exit(-1);	
		}
		
		while(read(fd, &mess[0], 1)){
				write(piped[1], mess, 1);
				//printf("%s / %d\n", mess, quanti);
		}
		exit(0);
	}
	
	close(piped[1]);
	
	int index = 0;
	
	while(read(piped[0], &mess[index], 1)){
		index += 1;
		if(mess[index -1] == '\n'){
			mess[index -1] = '\0';
			printf("%s\n", mess);
			index = 0;
		}
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
		ritorno = status >> 8;
		ritorno &= 0xFF;
	}
	exit(0);
	
	
}


