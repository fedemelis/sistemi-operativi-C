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
	
	int pid, fd, tot; 
	pipe_t *piped;
	pipe_t *pipedNipote;
	char buf[BUFSIZ];
	int status, pidfiglio, ritorno, pidNipote; //per i valori di ritorno dei figl
	tot = 0;
	long int totF = 0;
	long int tmp;
	
	//almeno 2 param
	if(argc < 3){
		printf("Inserisci almeno un parametro\n");
		exit(1);
	}
	
	for(int i = 1; i < argc; i++){
		if((fd = open(argv[i],  O_RDONLY)) < 0){
			printf("Devo passare nomi di file\n");
			exit(2);
		}
	}
	
	//alloco mem//////////////////////////////////////////
	
	piped = (pipe_t *)malloc(sizeof(pipe_t) * (argc - 1));
	
	if(piped == NULL){
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	
	//creo pipe
	for(int i = 0; i < argc-1; i++){
		if(pipe(piped[i]) < 0){
			printf("Errore in pipe\n");
			exit(3);
		}
	}
	
	////////////////////////////////////////////////////////
	
	//alloco mem
	pipedNipote = (pipe_t *)malloc(sizeof(pipe_t) * (argc - 1));
	
	if(pipedNipote == NULL){
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	
	//creo pipe
	for(int i = 0; i < argc-1; i++){
		if(pipe(pipedNipote[i]) < 0){
			printf("Errore in pipe\n");
			exit(3);
		}
	}
	
	
	//creo i figli//////////////////////////////////////////
	for(int i = 0; i < argc-1; i++){
		if((pid = fork()) < 0){
			printf("Errore nella fork\n");
			exit(4);
		}
		//figlio
		if(pid == 0){
			//printf("DEBUG: creato\n");
			//chiudo pipe non necessarie
			for(int p = 0; p < argc - 1; p++){
				close(piped[p][0]);
				if(p != i){
					close(piped[p][1]);
				}
			}
			
			for(int p = 0; p < argc - 1; p++){
				if(p != i){
					close(pipedNipote[p][1]);
					close(pipedNipote[p][0]);
				}
			}
			
			if((pid = fork()) < 0){
				printf("Errore nella fork\n");
				exit(4);
			}
			if(pid == 0){
				//printf("DEBUG: CHIUDO STDOUT\n");
				//chiudo stdout
				close(1);
				//al posto di stdout ho la pipe
				dup(pipedNipote[i][1]);
				close(pipedNipote[i][1]);
				
				//chiudo lo stdin e apro il file da leggere
				close(0);
				open(argv[i + 1], O_RDONLY);
				close(pipedNipote[i][0]);
			
				execlp("wc", "wc", "-l", (char *)0);
				exit(-1);
			}
			
			read(pipedNipote[i][0], buf, BUFSIZ);
			//printf("nread = %d - ho letto: %s", nread, buf);
			tot = atoi(buf);
			write(piped[i][1], &tot, sizeof(tot));	
			//printf("Debug: SCRITTO\n");
			if((pidNipote = wait(&status)) < 0){
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
				//printf("Il filgio con PID %d ha restituito: %d\n", pidNipote, ritorno);
				exit(ritorno);
			}
		}
	}
	
	for(int p = 0; p < argc - 1; p++){
		close(piped[p][1]);
	}
	
	for(int i = 0; i < argc - 1; i++){
		while(read(piped[i][0], &tmp, sizeof(tmp)) > 0){
			printf("DEBUG: lette %ld\n", tmp);
			totF += tmp;
		}
	}
	
	//aspetto i figli
	printf("ASPETTO I FIGLI\n");
	for(int i = 0; i < argc - 1; i++){
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
	printf("Totale delle linee lette: %ld\n", totF);
	exit(0);
}

