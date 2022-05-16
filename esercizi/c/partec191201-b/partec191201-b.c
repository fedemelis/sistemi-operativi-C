#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>


int main(int argc, char **argv){

	int fd, N, pid, nRead;
	int piped[2];
	long int count = 0;
	char ch;
	int pidfiglio, ritorno, status;
	
	struct {
	
	char ch;
	long int count;
	
	
	} retval;
	
	if(argc < 4){
		printf("Devo inserire almeno 3 param\n");
		exit(1);
	}
	
	if((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Il primo parametro deve essere un file\n");
		exit(2);
	}
	
	N = argc - 2;
	
	for(int i = 0; i < N; i++){
		if(argv[i+2][1] != 0){
			printf("Devo passare singoli caratteri\n");
			exit(3);
		}
	}
	
	//creo la pipe
	if((pipe(piped)) < 0){
		printf("Errore in pipe\n");
		exit(3);
	}
	
	//creo i figli
	for(int i = 0; i < N; i++){
		if((pid = fork()) < 0){
			printf("Errore in fork\n");
			exit(5);
		}
		
		//codice figli
		if(pid == 0){
		
		if((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Errore nell'apertura del file\n");
		exit(-1);
		}
		
		//chiudo il lato di lettura
		close(piped[0]);
		
		while((read(fd, &ch, 1)) > 0){
			if(argv[i + 2][0] == ch){
				count += 1;
			}
		}
		retval.ch = argv[i + 2][0];
		retval.count = count;
		write(piped[1], &retval, sizeof(retval));
		exit(0);
		}
	}

	//chiudo pipe
	close(piped[1]);
	
	while((nRead = read(piped[0], &retval, sizeof(retval))) > 0){
		printf("Ho trovato %ld occorrenze del carattere %c\n", retval.count, retval.ch);
	}
	
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
	exit(0);
}



