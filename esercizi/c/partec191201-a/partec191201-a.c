#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char **argv){

	int fd, N, pid, nRead;
	pipe_t *piped;
	long int count = 0;
	char ch;
	int pidfiglio, ritorno, status;
	
	if(argc < 4){
		printf("Devo inserire almeno 3 param\n");
		exit(1);
	}
	
	if((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Il primo parametro deve essere un file\n");
		exit(2);
	}
	
	N = argc - 2;
	
	for(int i = 0; i < N; i++){
		if(argv[i+2][1] != 0){
			printf("Devo passare singoli caratteri\n");
			exit(3);
		}
	}
	
	//alloco mem per le N pipe
	piped = (pipe_t *) malloc(N * sizeof(pipe_t));
	
	//creo le pipe
	for(int i = 0; i < N; i++){
		if((pipe(piped[i])) < 0){
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	
	//creo i figli
	for(int i = 0; i < N; i++){
		if((pid = fork()) < 0){
			printf("Errore in fork\n");
			exit(5);
		}
		
		//codice figli
		if(pid == 0){
		
		if((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Errore nell'apertura del file\n");
		exit(-1);
		}
		
		//chiudo pipe che non servono, tenendo aperta solo la scrittura del figlio interessato
		for(int j = 0; j < N; j++){
			close(piped[j][0]);
			if(i != j){
				close(piped[j][1]);	
			}
		}
		
		while((read(fd, &ch, 1)) > 0){
			if(argv[i + 2][0] == ch){
				count += 1;
			}
		}
		write(piped[i][1], &count, sizeof(long int));
		exit(0);
		}
	}

	//TODO chiudere pipe
	for(int i = 0; i < N; i++){
		close(piped[i][1]);
	}
	
	for(int i = 0; i < N; i++){
		if((nRead = read(piped[i][0], &ch, sizeof(long int))) > 0){
			printf("Ci sono %d occorrenze del carattere %s\n", ch, argv[i+2]);
		}
	
	}
	
	for(int i = 0; i < N; i++){
		if((pidfiglio = wait(&status)) < 0){
			printf("Errore nella wait\n");
			exit(2);
		}	
		
		if((status & 0xFF) != 0){
			printf("Errore nella chiusura del figlio\n");
			exit(3);
		}
		else{
			ritorno =(int)status >> 8;
			ritorno &= 0xFF;
			printf("Il filgio con PID %d ha restituito: %d\n", pidfiglio, ritorno);
		}
	}
	exit(0);
}



