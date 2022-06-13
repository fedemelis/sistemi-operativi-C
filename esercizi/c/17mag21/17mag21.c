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


	int Q, fd, nread, quanteOcc, pidfiglio, ritorno, status, pid, index, count; //var
	char msg; //token
	pipe_t* piped; //pipe
	char linea[255]; //array di char per la linea da leggere e scrivere
	
	
	//controllo param	
	if (argc < 3) {
		printf("Errore nel numero di param\n");
		exit(1);
	}

	//controllo file param
	for (int i = 1; i < argc; i++) {
		if ((fd = open(argv[i], O_RDONLY)) < 0) {
			printf("Devo passare N nomi di file\n");
			exit(2);
		}
	}
	
	//set variabile
	Q = argc - 1;

	/* pipe */
	piped = (pipe_t*)malloc(sizeof(pipe_t) * Q);
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < Q; i++) {
		if (pipe(piped[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	/* fine pipe*/
	

	//creo i figli
	for (int i = 0; i < Q; i++) {
		//printf("Figlio %d", i);
		//errore fork
		if ((pid = fork()) < 0) {
			printf("Errore in fork\n");
			exit(5);
		}
		if(pid == 0){
			//chiudo le pipe
			for(int p = 0; p < Q; p++){
				if(p != i){
					close(piped[p][0]);
				}
				if(p != (i+1)%Q){
					close(piped[p][1]);
				}
			}
			//apro file corretto
			if ((fd = open(argv[i+1], O_RDONLY)) < 0) {
				printf("Errore nell'apertura del file\n");
				exit(-1);
			}
			//set variabili
			index = 0;
			count = 0;
			//leggo finché si può
			while((nread = read(fd, &linea[index], 1)) > 0){
				//se è numerico incremento count
				if(isdigit(linea[index])){
					count++;
				}
				//se è linea aspetto il messaggio e stampo
				if(linea[index] == '\n'){
					nread = read(piped[i][0], &msg, 1);
					if(nread != 1){
						printf("Errore in sincronizzazione\n");
						exit(-1);
					}
					linea[index+1] = '\0';
					printf("Il processo di ordine %d e di PID: %d, ha trovato %d occorrenze numeriche nella linea che segue:\n", i, getpid(), count);
					printf("LINEA: %s\n", linea);
					quanteOcc = count;
					count = 0;
					index = 0;
					//do l'ok all'altro figlio
					write(piped[((i+1)%Q)][1], &msg, 1);
				}
				index++;
			}
			//restituisco il numero delle occ
			exit(quanteOcc);
		}
	}
	//non devo chiudere il lato di lettura della 0-esima pipe
	//close(piped[0][0]);
	//chiudo pipe
	for (int i = 1; i < Q; i++) {
		close(piped[i][0]);
		close(piped[i][1]);
	}
	
	//faccio partire
	write(piped[0][1], &msg, 1);

	//aspetto i figli (standard x N figli)
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < Q; i++) {
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
