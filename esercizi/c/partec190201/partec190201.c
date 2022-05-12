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
	
	int nFile, pid, fd, nread0, nread1; 
	int piped[2][2]; //le due pipe
	char msgP, msgD, ch;
	int status, pidfiglio, ritorno; //per i valori di ritorno dei figli
	
	//almeno 2 param
	if(argc < 3){
		printf("Inserisci almeno un parametro\n");
		exit(1);
	}
	
	//quanti file ho
	nFile = argc - 1;
	
	//creo pipe
	if(pipe(piped[0]) < 0){
		printf("Errore in pipe\n");
		exit(2);
	}
	
	if(pipe(piped[0]) < 0){
		printf("Errore in pipe\n");
		exit(1);
	}
	
	
	//creo i figli
	for(int i = 0; i < nFile; i++){
		if((pid = fork()) < 0){
			printf("Errore nella fork\n");
			exit(4);
		}
		//figlio
		printf("DEBUG %d\n", i);
		if(pid == 0){
			//chiudo pipe
			close(piped[0][0]);
			close(piped[1][0]);
			//i figli chiudono quella che non serve
			close(piped[i % 2][1]);
			//apro il file
			if((fd = open(argv[i+1],  O_RDONLY)) < 0){
				printf("Errore nell'apertura del file\n");
				exit(5);
			}
			//leggo e verifico i casi
			while(read(fd, &ch, 1) > 0){
			//se è un file pari e il carattere letto è alfanumerico scrivo
			//se è un file dispari e il carattere è numerico scrivo
				if ((((i % 2) == 0) && isalpha(ch)) || 	
            				(((i % 2) == 1) && isdigit(ch)))  { 	
          					write(piped[(i+1) % 2][1], &ch, 1);
				  }
			}
			exit(0);
		}
	}
	
	//chiudo la pipe del padre
	close(piped[0][1]);
	close(piped[1][1]);
	
	//leggo e conto gli elementi da stampare
	nread0 = read(piped[0][0], &msgP, 1);
	nread1 = read(piped[1][0], &msgD, 1);
	
	/*
	CON "nreadX != 0" IL CICLO VA AVANTI ALL'INFINITO
	*/
	while((nread0 > 0) || (nread1 > 0)){
		//scrivo su stdin
		write(1, &msgD, nread1); 
    		write(1, &msgP, nread0);
    		//leggo di nuovo aggiornando i contatori
		nread0 = read(piped[0][0], &msgP, 1);
		nread1 = read(piped[1][0], &msgD, 1); 
	}
	
	//aspetto i figlii
	for(int i = 0; i < nFile; i++){
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
		}
	}
	exit(0);
}

