#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#define PERM 0644

typedef int pipe_t[2];

//struttura da passare
typedef struct{
	int index;
	int len;
}str;

//flag per
bool flag = false;

//funzione handler
void handler(int signo){
	//printf("DEBUG: ENTRATO");
	flag = true;	
}

int main(int argc, char **argv){

	//var
	int fd, H, N, quantiLetti, nRead;
	//var per leggere la linea
	char ch[255];
	pipe_t *pipedFTOP;
	str mystruct;
	str mystructBis;
	int* pid;
	int pidfiglio, status, ritorno;
	
	quantiLetti = 0;
	
	//aggancio il segnale alla funzione
	signal(SIGUSR1, handler);
	
	//controllo param	
	if(argc < 4){
		printf("Errore nel numero di param\n");
		exit(1);
	}
	
	//controllo file param
	for(int i = 1; i < argc - 1; i++){
		if((fd = open(argv[i], O_RDONLY)) < 0){
			printf("Devo passare N nomi di file\n");
			exit(2);
		}
	}
	
	//controllo intero
	if(atoi(argv[argc - 1]) > 0 && atoi(argv[argc - 1]) < 255){
		H = atoi(argv[argc - 1]);
	}
	else{
		printf("L'ultimo param deve essere un numero\n");
		exit(3);
	}
	
	//
	N = argc - 2;
	
	pid = (int *)malloc(sizeof(int) * N);
	
	/* pipe */
	//pipe figlio to padre
	pipedFTOP = (pipe_t *)malloc(sizeof(pipe_t) * N);
	
	if(pipedFTOP == NULL){
		printf("Errore nell'allocazione della memoria\n");
		exit(6);
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(pipedFTOP[i]) < 0){
			printf("Errore in pipe\n");
			exit(7);
		}
	}
	/* fine pipe*/

	//creo i figli
	for(int i = 0; i < N; i++){
		
		//errore fork
		if((pid[i] = fork()) < 0){
			printf("Errore in fork\n");
			exit(5);
		}
		
		//cod figlio
		if(pid[i]  == 0){
			
			//chiudo pipe
			for(int p = 0; p < N; p++){
				if(p != i){
					close(pipedFTOP[p][1]);
				}
				if(i == 0 || p != i-1){
					close(pipedFTOP[p][0]);
				}			
			}
			//apro file corrispondente
			if((fd = open(argv[i+1], O_RDONLY)) < 0){
				//printf("%s\n", argv[i+1]);
				printf("Errore nell'apertura del file\n");
				exit(2);
			}
			//leggo le righe
			for(int r = 0; r < H; r++){
				//leggo la linea
				quantiLetti = 0;
				nRead = read(fd, &ch[quantiLetti], 1);
				//printf("Letto: %s\n", ch);
				while(nRead > 0 && ch[quantiLetti] != '\n'){
					quantiLetti += nRead;
					nRead = read(fd, &ch[quantiLetti], 1);
				}
				//printf("STRINGA: %s, index: %d\n", ch, i);
				//codice differenziato per il primo figlio
				if(i == 0){
					mystruct.index = i;
					mystruct.len = quantiLetti;
				}
				//codice per gli altri figli
				else{
					read(pipedFTOP[i-1][0], &mystruct, sizeof(str));
					//printf("%d < %d\n", mystruct.len, quantiLetti);
					//aggiorno la struct
					if(mystruct.len < quantiLetti){
						mystruct.index = i;
						mystruct.len = quantiLetti;
					}
				}
				//scrivo la struct
				//printf("SCRIVO index: %d len: %d riga: %d inviato dalla pipe: %d\n", mystruct.index, mystruct.len, r, i);
				write(pipedFTOP[i][1], &mystruct, sizeof(str));
				sleep(3);
				//se il flag è true scrivo la linea
				if(flag == true){
					printf("%s", ch);
					flag = false;
				}
				//pulisco l'array
				memset(ch, 0, 255);
			}
			exit(0);
		}
		
	
	}
	//chiudo pipe
	
	for(int p = 0; p < N; p++){
		close(pipedFTOP[p][1]);
		if(p != N-1){
			close(pipedFTOP[p][0]);
		}
				
	}
	
	for(int i = 0; i < H; i++){
		//leggo la struct
		read(pipedFTOP[N-1][0], &mystructBis, sizeof(str));
		//printf("RICEVUTO index=%d dalla pipe %d iterazione %d\n", mystructBis.index, N-1, H);
		for(int ii = 0; ii < N; ii++){
			if(ii == mystructBis.index){
				//invio il segnale al figlio giusto
				printf("Inviato al figlio %d\n", ii);
				kill(pid[mystructBis.index], SIGUSR1);
			}
		}
		
	}
	
	//aspetto i figli - STANDARD N FIGLI
	printf("ASPETTO I FIGLI\n");
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
	printf("DEBUG: Finito\n");
	exit(0);
}
