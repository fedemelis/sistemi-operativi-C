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

	int H, fd, pid, N, fdNew, lung, nRead, nWrite, pidfiglio, status, ritorno, count;
	pipe_t* piped;
	char nomefile[255];
	char blocco[255];
	
	if(argc < 3){
		printf("Errore nel numero di param\n");
		exit(1);
	}
	
	//dispari
	if((atoi(argv[1]) % 2) != 1){
		printf("Il primo param deve essere un num dispari\n");
		exit(2);
	}
	
	//set delle var
	H = atoi(argv[1]);
	N = argc - 2;
	count = 0;
	
	//controllo nomi file
	for(int i = 2; i < argc; i++){
		if((fd = open(argv[i], O_RDONLY)) < 0){
			printf("Devo passare nomi di file\n");
			exit(3);
		}
	}
	
	/*creo pipe*/
	piped = (pipe_t*)malloc(sizeof(pipe_t) * N);

	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(4);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(piped[i]) < 0) {
			printf("Errore in pipe\n");
			exit(5);
		}
	}
	/*fine pipe*/
	
	//creo figli
	for(int i = 0; i < N*2; i++){
		printf("I: %d\n", i);
		//creo 
		if((pid = fork()) < 0){
			printf("Errore in fork\n");
			exit(6);
		}
		if(pid == 0){
			//primo figlio della coppia
			if(i < N){
				//chiudo pipe
				for(int p = 0; p < N; p++){
					close(piped[p][1]);
					if(p != i){
						close(piped[p][1]);	
					}
				}
				//CODICE FIGLIO 1
				//apro file corrispondente
				if((fd = open(argv[i+2], O_RDONLY)) < 0){
					printf("Errore nell'apertura del file\n");
					exit(0);
				}
				
				//preparo il nome del file
				strcpy(nomefile, argv[i+2]);
				strcat(nomefile, ".mescolato");
				
				//printf("%s\n", nomefile);
				
				//creo nuovo file
				if((fdNew = creat(nomefile, PERM)) < 0){
					printf("Errore durante la creazione del nuovo file\n");
					exit(0);	
				}
				
				//calcolo la lunghezza del file in blocchi
				lung = (lseek(fd, 0L, SEEK_END) / H);
				lseek(fd, 0L, SEEK_SET);
				
				for(int j = 0; j < lung/2; j++){
					//leggo il blocco
					nRead = read(fd, blocco, H);
					if(nRead != H){
						printf("Errore durante la lettura del file\n");
						exit(0);
					}
					//scrivo il blocco
					nWrite = write(fdNew, blocco, H);
					//printf("%s\n", blocco);
					if(nWrite != H){
						printf("Errore durante la scrittura sul nuovo file\n");
						exit(0);
					}
					count += 1;
					//printf("%s\n", blocco);
					
					//leggo il blocco dell'altro figlio
					nRead = read(piped[i][0], blocco, H);
					if(nRead != H){
						printf("Errore durante la lettura del file\n");
						exit(0);
					}
					//scrivo il blocco dell'altro figlio
					nWrite = write(fdNew, blocco, H);
					//printf("%s\n", blocco);
					if(nWrite != H){
						printf("Errore durante la scrittura sul nuovo file\n");
						exit(0);
					}
				}
				exit(count);
			}
			//CODICE FIGLIO 2
			else{
				//chiudo pipe
				for(int p = 0; p < N; p++){
					close(piped[p][0]);
					if(p != i-N){
						close(piped[p][1]);	
					}
				}
				
				//apro file corrispondente alla coppia
				if((fd = open(argv[i-N+2], O_RDONLY)) < 0){
					printf("Errore nell'apertura del file\n");
					exit(0);
				}
				
				
				//calcolo la lunghezza del file in blocchi
				lung = (lseek(fd, 0L, SEEK_END) / H);
				lseek(fd, ((long)H*(lung/2)), SEEK_SET);
				
				for(int j = 0; j < lung/2; j++){
					//leggo blocco
					nRead = read(fd, blocco, H);
					//printf("BLOCCO 2:%s\n", blocco);
					if(nRead != H){
						printf("Errore durante la lettura del file\n");
						exit(0);
					}
					//scrivo bloccos
					nWrite = write(piped[i-N][1], blocco, H);
					if(nWrite != H){
						printf("Errore durante la scrittura sul nuovo file\n");
						exit(0);
					}
					count += 1;
				}
				exit(count);
			}
		}
	}
	
	//chiudo tutte le pipe
	for(int i = 0; i < N; i++){
		close(piped[i][0]);
		close(piped[i][1]);
	}
	
	//aspetto i figli
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < N*2; i++) {
		//printf("ASPETTO\n");
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
				printf("Il filgio con PID %d ha restituito: %d (se 0 problemi!)\n", pidfiglio, ritorno);
			}
	}
	printf("DEBUG: Finito\n");
	exit(0);	
}

