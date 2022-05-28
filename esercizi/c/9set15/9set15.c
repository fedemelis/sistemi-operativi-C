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

	int fd, N, nread, pidfiglio, ritorno, status, index;
	pipe_t* pipedFTOP; //figlio > padre
	pipe_t* pipedPTOF; //padre > figlio
	int* pid;
	bool* isValid;
	char ch, chFiglio, go, ricevuto; //go = token
	go = 'x';

	if(argc < 4){
		printf("Errore nel nuemro di param\n");
		exit(1);
	}
	
	N = argc - 2;
	
	for(int i = 1; i < argc; i++){
		if((fd = open(argv[i], O_RDONLY))<0){
			printf("Devo passare solo file\n");
			exit(2);
		}
	}
	
	pipedFTOP = (pipe_t*)malloc(sizeof(pipe_t) * N);

	if (pipedFTOP == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(pipedFTOP[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}

	//pipe padre to figlio
	pipedPTOF = (pipe_t*)malloc(sizeof(pipe_t) * N);

	if (pipedPTOF == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(5);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(pipedPTOF[i]) < 0) {
			printf("Errore in pipe\n");
			exit(6);
		}
	}
	
	pid = (int*)malloc(sizeof(int) * N);
	isValid = (bool*)malloc(sizeof(bool) * N);
	
	for(int i = 0; i < N; i++){
		isValid[i] = true;
	}
	
	for(int i = 0; i < N; i++){
		if((pid[i] = fork()) < 0){
			printf("Errore in fork\n");
			exit(7);
		}
		
		if(pid[i] == 0){
			//printf("Creato figlio: N\n");
			//chiudo pipe
			for(int ii = 0; ii < N; ii++){
				close(pipedFTOP[ii][0]);
				close(pipedPTOF[ii][1]);
				if(ii != i){
					close(pipedFTOP[ii][1]);
					close(pipedPTOF[ii][0]);
				}
				//printf("Chiudo pipe ii=%d N=%d\n", ii, N);
			}
			//printf("Provo ad aprire il file\n");
			
			if((fd = open(argv[i+1], O_RDONLY)) < 0){
				printf("Errore nell'apertura del file\n");
				exit(9);
			}
			nread = 1;
			//printf("Entro nel while\n");
			/*do{
				nread = read(pipedPTOF[i][0], &ricevuto, 1);
				//read(pipedPTOF[i][0], &ricevuto, 1);
				//printf("DEBUG: ho letto %c\n", ricevuto);
				//printf("Sto provando a leggere dal file %s\n", argv[i+1]);
				read(fd, &ch, 1);
				//printf("DEBUG: ho scritto %c", rd);
				write(pipedFTOP[i][1], &ch, 1);
			
			}while(nread > 0);*/
			while(nread > 0){
				nread = read(pipedPTOF[i][0], &ricevuto, 1);
				if(ricevuto == 's'){
					break;
				}
				//printf("DEBUG: ho letto %c\n", ricevuto);
				//printf("Sto provando a leggere dal file %s\n", argv[i+1]);
				read(fd, &ch, 1);
				//printf("DEBUG: ho scritto %c", rd);
				write(pipedFTOP[i][1], &ch, 1);
			}
			printf("DEBUG: SONO USCITO SONO USCITO SONO USCITO %d\n", pid[i]);
			exit(0);	
		}

	}
	
	//padre
	
	for(int i = 0; i < N; i++){
		close(pipedFTOP[i][1]);
		close(pipedPTOF[i][0]);
	}
	
	if((fd = open(argv[argc-1], O_RDONLY)) < 0){
		printf("Errore nell'apertura del file\n");
		exit(8);
	}
	
	while((nread = read(fd, &ch, 1)) > 0){
		//printf("Ho letto %c\n", ch);
		for(int i = 0; i < N; i++){
			if(isValid[i]){
				//printf("Mando %c\n", go);
				write(pipedPTOF[i][1], &go, 1);
				read(pipedFTOP[i][0], &chFiglio, 1);
				printf("%c é uguale a %c?\n", chFiglio, ch);
				if(chFiglio != ch){
					printf("DEBUG: NON VALIDO. pid: %d\n", pid[i]);
					isValid[i] = false;
				}
				else{
					printf("VALIDO\n");
				}
			}
		}
	}
	
	
	for(int i = 0; i < N; i++){
		if(isValid[i] == false){
			kill(pid[i], SIGKILL);
		}
		else{
			go = 's';
			write(pipedPTOF[i][1], &go, 1);
		}
	}
	
	//aspetto i figli (standard x N figli)
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < N; i++) {
		printf("Aspetto %d\n", i);
		if ((pidfiglio = wait(&status)) < 0) {
			printf("Errore nella wait\n");
			exit(2);
		}
		printf("Wait corretta\n");
		if ((status & 0xFF) != 0) {
			printf("Il filgio con PID %d è terminato in modo anomalo\n", pidfiglio);
		}
		else {
			ritorno = (int)status >> 8;
			ritorno &= 0xFF;
			for(int j = 0; j < N; j++){
				if(pidfiglio == pid[j]){
					index = j;
				}
			}
			printf("Il filgio con PID %d ha restituito: %d. Il file corretto è: %s\n", pidfiglio, ritorno, argv[index+1]);
		}
	}
	printf("DEBUG: finito");
	exit(0);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}
