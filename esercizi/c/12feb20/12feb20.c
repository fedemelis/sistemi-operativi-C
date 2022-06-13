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

	long int asciiP;
	long int asciiD;
	
}conteggio;

int main(int argc, char** argv) {


	int N, fd, nread, pidfiglio, ritorno, status, pid, index, nr; //var
	pipe_t* piped; //pipe
	char ch;
	conteggio ret; //struttura da resituire al padre
	long int pari, dispari;
	
	pari = 0;
	dispari = 0;

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
	piped = (pipe_t*)malloc(sizeof(pipe_t) * N);
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(piped[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	
	/*fine pipe*/
	

	//creo i figli
	for (int i = 0; i < N; i++) {
		//errore fork
		if ((pid = fork()) < 0) {
			printf("Errore in fork\n");
			exit(5);
		}
		//figlio
		if(pid == 0){
			//chiudo pipe
			for(int p = 0; p < N; p++){
				close(piped[p][0]);
				if(i != p){
					close(piped[p][1]);
				}
			}
			//apro file corretto
			if ((fd = open(argv[i+1], O_RDONLY)) < 0) {
				printf("Errore nell'apertura del file\n");
				exit(-1);
			}
			//figli pari
			if((i % 2) == 0){	
				index = 0;
				while((nread = read(fd, &ch, 1)) > 0){
					if((index % 2) == 0){
						if((ch % 2) == 0){
							pari++;
						}
						else{
							dispari++;
						}
					}
					index+=1;
				}
				ret.asciiP = pari;
				ret.asciiD = dispari;
				write(piped[i][1], &ret, sizeof(ret));
				if(pari > dispari){
					exit(0);
				}
				else{
					exit(1);
				}
			}
			else{
				index = 0;
				while((nread = read(fd, &ch, 1)) > 0){
					if((index % 2) == 1){
						if((ch % 2) == 0){
							pari++;
						}
						else{
							dispari++;
						}
					}
					index+=1;
				}
				ret.asciiP = pari;
				ret.asciiD = dispari;
				write(piped[i][1], &ret, sizeof(ret));
				if(pari > dispari){
					exit(0);
				}
				else{
					exit(1);
				}
			}
		}
	}	
	//chiudo pipe
	for (int i = 0; i < N; i++) {
		close(piped[i][1]);
	}
	
	for(int i = 0; i < N; i+=2){
		nr = read(piped[i][0], &ret, sizeof(ret));
		if(nr != sizeof(ret)){
			printf("Errore in lettura della struct\n");
			exit(6);
		}
		printf("Il figlio con indice %d ha calcolato: c1=%ld e c2=%ld\n", i, ret.asciiP, ret.asciiD);
	}
	
	for(int i = 1; i < N; i+=2){
		nr = read(piped[i][0], &ret, sizeof(ret));
		if(nr != sizeof(ret)){
			printf("Errore in lettura della struct\n");
			exit(6);
		}
		printf("Il figlio con indice %d ha calcolato: c1=%ld e c2=%ld\n", i, ret.asciiP, ret.asciiD);
	}

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
