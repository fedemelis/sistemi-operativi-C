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


	int N, fd, nread, pidfiglio, ritorno, status, pid, index, quanteOcc; //var
	char stop, go, token; //token
	pipe_t* pipedFP; //pipe
	pipe_t* pipedPF; //pipe
	char linea[250]; //array di char per la linea da leggere
	bool * finito;
	bool fin = false;
	char chMax = '0';
	char ch;
	int winnerIndex;
	
	stop = 's';
	go = 'g';
	quanteOcc = 0;
	
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
	N = argc - 1;
	
	//array di bool
	finito = (bool *)malloc(sizeof(bool) * N);
	if (finito == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	
	for(int i = 0; i < N; i++){
		finito[i] = false;
	}


	/* pipe */
	pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * N);
	if (pipedFP == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(pipedFP[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	
	pipedPF = (pipe_t*)malloc(sizeof(pipe_t) * N);
	if (pipedPF == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(pipedPF[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	/* fine pipe*/
	

	//creo i figli
	for (int i = 0; i < N; i++) {
		//errore fork
		if ((pid = fork()) < 0) {
			printf("Errore in fork\n");
			exit(5);
		}
		if(pid == 0){
			
			if ((fd = open(argv[i+1], O_RDONLY)) < 0) {
				printf("Errore nell'apertura del file\n");
				exit(-1);
			}
			
			//chiudo le pipe
			for(int p = 0; p < N; p++){
				if(i != p){
					close(pipedFP[p][1]);
					close(pipedPF[p][0]);
				}
				close(pipedFP[p][0]);
				close(pipedPF[p][1]);
			}
			
			index = 0;
			
			while((nread = read(fd, &linea[index], 1)) > 0){
				if(linea[index] == '\n'){
					linea[index] = '\0';
					write(pipedFP[i][1], &linea[0], 1);
					nread = read(pipedPF[i][0], &token, 1);
					if(nread != 1){
						printf("Errore durante la lettura del token\n");
						exit(-1);
					}
					//token corretto
					if(token == 'g'){
						printf("Sono il figlio %d-esimo con PID: %d, ho inviato al padre il carattere '%c'\n", i, getpid(), linea[0]);
						printf("LINEA: %s\n", linea);
						quanteOcc += 1;
					}
					index = 0;
				}
				else{
					index += 1;
				}
				
			}
			//printf("DEBUG: FIGLIO USCITO\n");
			exit(quanteOcc);
		}
	}
	
	//chiudo pipe
	for (int i = 0; i < N; i++) {
		close(pipedFP[i][1]);
		close(pipedPF[i][0]);
	}
	
	while(fin == false){
		chMax = 0;
		fin = true;
		//leggo da tutti i figli e calcolo il massimo
		for(int i = 0; i < N; i++){
			if(finito[i] == false){
				//printf("Provo a leggere\n");
				if((nread =(read(pipedFP[i][0], &ch, 1))) != 1){
					printf("DEBUG: IL FIGLIO %d è finito\n", i);
					finito[i] = true;
				}
				//calcolo massimo
				else if(ch > chMax){
					chMax = ch;
					winnerIndex = i;
				}
			}
		}
		//do le indicazioni ai figli
		for(int i = 0; i < N; i++){
			if(i == winnerIndex){
				write(pipedPF[i][1], &go, 1);
			}
			else if (finito[i] == false){
				write(pipedPF[i][1], &stop, 1);
			}
		}
		
		//verifico se ci sono ancora figli che non hanno terminato
		for(int i = 0; i < N; i++){
			if(finito[i] == false){
				printf("FIGLIO %d ancora non ha finito\n", i);
				fin = false;
			}
		}
	}
	printf("ASPETTO I FIGLI\n");
	//aspetto i figli (standard x N figli)
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < N; i++) {
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





