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

typedef struct{
	long int occorrenze; //c1 nel testo
	int index; //c2 nel testo
	long int sumOcc; //c3 nel testo
}retStruct;

int main(int argc, char** argv) {


	int *pid; //array di pid per gestire i figli
	int N, fd, nread, quantiLetti, quanteOcc, pidfiglio, ritorno, status;
	char Cx; //carattere passato come param
	char ch; //per leggere i char della linea
	pipe_t* piped;
	retStruct str; //struttura
	
	quantiLetti = 0;
	quanteOcc = 0;
	
	//controllo param	
	if (argc < 4) {
		printf("Errore nel numero di param\n");
		exit(1);
	}

	//controllo file param
	for (int i = 1; i < argc - 2; i++) {
		if ((fd = open(argv[i], O_RDONLY)) < 0) {
			printf("Devo passare N nomi di file\n");
			exit(2);
		}
	}
	
	//controllo il singolo carattere
	if(strlen(argv[argc-1]) != 1){
		printf("Devo passare un singolo carattere come ultimo param\n");
		exit(2);
	}
	
	Cx = argv[argc-1][0];
	N = argc - 2;

	/* pipe */
	piped = (pipe_t*)malloc(sizeof(pipe_t) * N);

	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(6);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(piped[i]) < 0) {
			printf("Errore in pipe\n");
			exit(7);
		}
	}


	/* fine pipe*/
	
	//alloco la mem per l'array di pid
	pid = (int*)malloc(sizeof(int) * N);
	
	if (pid == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(6);
	}

	//creo i figli
	for (int i = 0; i < N; i++) {
		//printf("Figlio %d", i);
		//errore fork
		if ((pid[i] = fork()) < 0) {
			printf("Errore in fork\n");
			exit(5);
		}
		if(pid[i] == 0){
			if ((fd = open(argv[i+1], O_RDONLY)) < 0) {
				printf("Devo passare N nomi di file\n");
				exit(2);
			}
			
			//chiudo pipe
			for(int p = 0; p < N; p++){
				if(p != i){
					close(piped[p][1]);
				}
				if(i == 0 || p != i-1){
					close(piped[p][0]);
				}
			}
			
			//leggo il file e conto le occorrenze
			while((nread = read(fd, &ch, 1)) > 0){
				//printf("Controllo\n");
				if(ch == Cx){
					quanteOcc+=1;
				}
				quantiLetti += 1;
			}
			//per il primo figlio
			if(i == 0){
				str.index = getpid();
				str.occorrenze = quanteOcc;
				str.sumOcc = quanteOcc;
			}
			//per gli altri figli
			else{
				read(piped[i-1][0], &str, sizeof(str));
				if(str.occorrenze < quanteOcc){
					str.index = getpid();
					str.occorrenze = quanteOcc;
				}
				str.sumOcc += quanteOcc;
			}
			//printf("Scrivo %d\n", i);
			write(piped[i][1], &str, sizeof(str));
			exit(i);
		}
	}
	//chiudo pipe
	for (int i = 0; i < N; i++) {
		if(i != N-1){
			close(piped[i][0]);
		}
		close(piped[i][1]);
	}
	//printf("Provo a leggere\n");
	//leggo la struct
	read(piped[N-1][0], &str, sizeof(str));
	
	printf("Il figlio con pid:%d ha trovato %ld occorrenze del carattere.\n", str.index, str.occorrenze);
	printf("Il totale delle occerrenze è: %ld\n", str.sumOcc);
	
	//

	//aspetto i figli (standard x N figli)
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < N; i++) {
		if ((pidfiglio = wait(&status)) < 0) {
			printf("Errore nella wait\n");
			exit(2);
		}

		if ((status & 0xFF) != 0) {
			printf("Errore nella chiusura del figlio\n");
			exit(3);
		}
		else {
			ritorno = (int)status >> 8;
			ritorno &= 0xFF;
			printf("Il filgio con PID %d ha restituito: %d\n", pid[ritorno], ritorno);
		}
	}
	printf("DEBUG: Finito\n");
	exit(0);

}
