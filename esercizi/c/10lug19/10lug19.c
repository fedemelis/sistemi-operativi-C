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
	pipe_t* piped1; //pipe
	pipe_t* piped2; //pipe
	char ch, c;
	long int pos = 0;
	int count = 0;
	int nw, nr;
	
	//controllo param	
	if (argc < 4) {
		printf("Errore nel numero di param\n");
		exit(1);
	}

	//controllo file param
	for (int i = 1; i < argc-1; i++) {
		if ((fd = open(argv[i], O_RDONLY)) < 0) {
			printf("Devo passare N nomi di file\n");
			exit(2);
		}
	}
	//controllo carattere
	if(strlen(argv[argc-1]) != 1){
		printf("L'ultimo param deve essere un singolo carattere\n");
		exit(2);
	}
	//set variabile
	N = argc - 2;
	ch = argv[argc-1][0];
	
	//pipe
	piped1 = (pipe_t*)malloc(sizeof(pipe_t) * N * 2);
	if (piped1 == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	for (int i = 0; i < N * 2; i++) {
		if (pipe(piped1[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	
	piped2 = (pipe_t*)malloc(sizeof(pipe_t) * N * 2);
	if (piped2 == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < N *2; i++) {
		if (pipe(piped2[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	//fine pipe

	//creo i figli
	for (int i = 0; i < N*2; i++) {
		//errore fork
		if ((pid = fork()) < 0) {
			printf("Errore in fork\n");
			exit(5);
		}
		if(pid == 0){
			if(i < N){
				//chiudo le pipe
				for(int p = 0; p < N*2; p++){
					close(piped1[p][0]);
					close(piped2[p][1]);
					if(p != i){
						//printf("CHIUDO 1: %d %d\n", p, i);
						close(piped1[p][1]);
					}
					//printf("%d != %d\n", p, ((2*N)-i)-2);
					if(p != (((N*2)-i)-1)){
						//printf("CHIUDO 2: piped[%d][%d]\n", p, i);
						close(piped2[p][0]);
					}
				}
				//apro file corretto
				if ((fd = open(argv[i+1], O_RDONLY)) < 0) {
					printf("Errore nell'apertura del file\n");
					exit(-1);
				}
				pos = 0;
				count = 0;
				while((nread = read(fd, &c, 1)) > 0){
					pos += 1;
					if(c == ch){
						//printf("Trovato in posizione %ld dal processo di indice %d\n", pos, i);
						count+=1;
						nw = write(piped1[i][1], &pos, sizeof(pos));
						//printf("PRIMO: scrivo su piped[%d]\n", i);
						if(nw != sizeof(pos)){
							printf("Errore nell'invio della posizione\n");
							exit(-1);
						}
						nr = read(piped2[((2*N)-i)-1][0], &pos, sizeof(pos));
						//printf("PRIMO: leggo da piped[%d]\n", ((2*N)-i)-1);
						//printf("PRIMO: Leggo la posizione %ld dal figlio %d e ho letto in totale: %d\n", pos, ((2*N)-i)-1, nr);
						if(nr == 0){
							exit(count);
						}
						if(nr != sizeof(pos)){
							printf("Errore nel ricevimento della posizione\n");
							exit(-1);
							//finito = true;
						}
						lseek(fd, pos, SEEK_SET);
					}
				}
			}
			else{
				for(int p = 0; p < N*2; p++){
					close(piped1[p][1]);
					close(piped2[p][0]);
					if(p != ((2*N)-i)-1){
						close(piped1[p][0]);
					}
					if(p != i){
						close(piped2[p][1]);
					}
				}
				
				if ((fd = open(argv[(N*2)-i], O_RDONLY)) < 0) {
					printf("Errore nell'apertura del file\n");
					exit(-1);
				}
				
				nr = read(piped1[((2*N)-i)-1][0], &pos, sizeof(pos));
				//printf("SECONDO: Leggo la posizione %ld dal figlio %d e ho letto in totale: %d\n", pos, ((2*N)-i)-1, nr);
				if(nr == 0){
					exit(count);
				}
				if(nr != sizeof(pos)){
					printf("Errore nel ricevimento della posizione\n");
					exit(-1);
				}
				count = 0;
				while((nread = read(fd, &c, 1)) > 0){
					//printf("Vado\n");
					pos += 1;
					if(c == ch){
						//printf("Trovato in posizione %ld dal processo di indice %d\n", pos, i);
						//printf("TROVATO\n");
						count+=1;
						nw = write(piped2[i][1], &pos, sizeof(pos));
						//printf("SECONDO: Scrivo su piped[%d]\n", i);
						if(nw != sizeof(pos)){
							printf("Errore nell'invio della posizione\n");
							exit(count);
						}
						nr = read(piped1[((2*N)-i)-1][0], &pos, sizeof(pos));
						if(nr == 0){
							exit(count);
						}
						//printf("SECONDO: leggo da piped[%d]\n", ((2*N)-i)-1);
						//printf("SECONDO: Leggo la posizione %ld dal figlio %d e ho letto in totale: %d\n", pos, ((2*N)-i)-1, nr);
						if(nr != sizeof(pos)){
							printf("Errore nel ricevimento della posizione\n");
							exit(-1);
							//finito = true;
						}
						lseek(fd, pos, SEEK_SET);
					}
				}
				
			}	
			exit(count);
		}
	}
	//chiudo pipe
	for (int i = 0; i < N*2; i++) {
		close(piped1[i][0]);
		close(piped1[i][1]);
		close(piped2[i][0]);
		close(piped2[i][1]);
	}
	
	//aspetto i figli (standard x N figli)
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < N*2; i++) {
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
