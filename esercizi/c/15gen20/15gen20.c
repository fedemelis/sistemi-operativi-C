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

typedef int pipe_t[2];

int main(int argc, char** argv) {


	int N, fd, nread, pidfiglio, ritorno, status, pid; //var
	char msg; //token
	pipe_t* pipedFN; //pipe
	pipe_t* pipedNF; //pipe
	int FOut;
	char nome[15];
	char ch;
	
	//controllo param	
	if (argc < 3) {
		printf("Errore nel numero di param\n");
		exit(1);
	}
	
	
	//pari
	if((argc % 2) != 1){
		printf("Devo passare un numero pari di file\n");
		exit(2);
	}

	//controllo file param
	for (int i = 1; i < argc; i++) {
		if ((fd = open(argv[i], O_RDONLY)) < 0) {
			printf("Devo passare N nomi di file\n");
			exit(2);
		}
	}
	
	//set variabile
	N = argc - 1;

	/* pipe */
	pipedFN = (pipe_t*)malloc(sizeof(pipe_t) * (N/2));
	if (pipedFN == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < N/2; i++) {
		if (pipe(pipedFN[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	
	pipedNF = (pipe_t*)malloc(sizeof(pipe_t) * (N/2));
	if (pipedNF == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < N/2; i++) {
		if (pipe(pipedNF[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	/* fine pipe*/
	

	//creo i figli
	for (int i = 0; i < N/2; i++) {
		//errore fork
		if ((pid = fork()) < 0) {
			printf("Errore in fork\n");
			exit(5);
		}
		//figlio
		if(pid == 0){
		
			sprintf(nome, "/tmp/merge%d", i);
			FOut = creat(nome, PERM);
			
			if((pidfiglio = fork()) < 0){
				printf("Errore in fork\n");
				exit(5);
			}
			//nipote
			if(pidfiglio == 0){
				if ((fd = open(argv[(N/2)+i+1], O_RDONLY)) < 0) {
					printf("Errore nell'apertura del file\n");
					exit(-1);
				}
				
				//chiudo pipe
				for(int p = 0; p < N/2; p++){
					close(pipedNF[p][0]);
					close(pipedFN[p][1]);
					if(p != i){
						close(pipedNF[p][1]);
						close(pipedFN[p][0]);
					}
				}
				
				//legge l'ok
				
				while((nread = read(fd, &ch, 1)) > 0){
					read(pipedFN[i][0], &msg, 1);
					write(FOut, &ch, 1);
					write(pipedNF[i][1], &msg, 1);
					//printf("Letto\n");
				}
				//printf("Nipote uscito\n");
				exit(ch);
				
			}
			
			//chiudo pipe figlio
			for(int p = 0; p < N/2; p++){
				close(pipedNF[p][1]);
				close(pipedFN[p][0]);
				if(p != i){
					close(pipedNF[p][0]);
					close(pipedFN[p][1]);
				}
			}
			
			//apro file corretto
			if ((fd = open(argv[i+1], O_RDONLY)) < 0) {
				printf("Errore nell'apertura del file\n");
				exit(-1);
			}
			
			//scrivo sul file
			while((nread = read(fd, &ch, 1)) > 0){
				write(FOut, &ch, 1);
				write(pipedFN[i][1], &msg, 1);
				read(pipedNF[i][0], &msg, 1);
				
			}
			
			printf("ASPETTO IL NIPOTE\n");
			if ((pidfiglio = wait(&status)) < 0) {
				printf("Errore nella wait\n");
				exit(6);
			}
	
			if ((status & 0xFF) != 0) {
				printf("Errore nella chiusura del figlio con PID %d\n", pidfiglio);
				exit(7);
			}
			else {
				ritorno = (int)status >> 8;
				ritorno &= 0xFF;
				printf("Il nipote con PID %d ha restituito: %d\n", pidfiglio, ritorno);
				exit(ritorno);
			}
		}
	}
	//chiudo pipe
	for (int i = 0; i < N/2; i++) {
		close(pipedNF[i][0]);
		close(pipedNF[i][1]);
		close(pipedFN[i][0]);
		close(pipedFN[i][1]);
	}

	//aspetto i figli (standard x N figli)
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < N/2; i++) {
		if ((pidfiglio = wait(&status)) < 0) {
			printf("Errore nella wait\n");
			exit(6);
		}

		if ((status & 0xFF) != 0) {
			printf("Errore nella chiusura del figlio con PID %d\n", pidfiglio);
			exit(7);
		}
		else {
			ritorno = (int)status >> 8;
			ritorno &= 0xFF;
			printf("Il filgio con PID %d ha restituito: %d\n", pidfiglio, ritorno);
		}
	}
	printf("DEBUG: Finito\n");
	exit(0);

}
