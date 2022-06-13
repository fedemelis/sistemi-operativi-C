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


	int fd, nread, pidfiglio, ritorno, status, pid; //var
	pipe_t* piped; //pipe
	char ch;
	int B, L;
	char buff[255];
	int fdCreat;
	char *nome;

	//controllo param	
	if (argc != 4) {
		printf("Errore nel numero di param\n");
		exit(1);
	}

	//controllo file param
	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		printf("Devo passare un nome di file\n");
		exit(2);
	}
	
	B = atoi(argv[2]);
	L = atoi(argv[3]);

	//nomefile
	nome = (char*)malloc(sizeof(char) * (strlen(argv[1]) + 8));
	
	strcat(nome, argv[1]);
	strcat(nome, ".Chiara");
	strcat(nome, "\0");
	
	/* pipe */
	piped = (pipe_t*)malloc(sizeof(pipe_t) * B);
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < B; i++) {
		if (pipe(piped[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}
	
	/*fine pipe*/
	

	//creo i figli
	for (int i = 0; i < B; i++) {
		//errore fork
		if ((pid = fork()) < 0) {
			printf("Errore in fork\n");
			exit(5);
		}
		//figlio
		if(pid == 0){
			for(int p = 0; p < B; p++){
				close(piped[p][0]);
				if(i != p){
					close(piped[p][1]);
				}
			}
			
			if ((fd = open(argv[1], O_RDONLY)) < 0) {
				printf("Errore nell'apertura del file\n");
				exit(2);
			}
			
			lseek(fd, (long int)((L/B)*i), SEEK_SET);
			
			nread = read(fd, buff, (L/B));
			
			write(piped[i][1], &buff[nread-1], 1);
			
			exit(nread);
			
			
		}
	}	
	//chiudo pipe
	for (int i = 0; i < B; i++) {
		close(piped[i][1]);
	}
	
	printf("%s\n", nome);
	
	
	if((fdCreat = creat(nome, PERM)) < 0){
		printf("Errore durante la creazione del file\n");
		exit(2);
	} 
	
	if ((fdCreat = open(nome, O_RDWR)) < 0) {
		printf("Errore nell'apertura del nuovo file\n");
		exit(2);
	}
	
	for(int i = 0; i < B; i++){
		read(piped[i][0], &ch, 1);
		write(fdCreat, &ch, 1);
	}
	

	//aspetto i figli (standard x N figli)
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < B; i++) {
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
