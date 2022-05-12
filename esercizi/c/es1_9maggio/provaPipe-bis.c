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

int main(int argc, char **argv){

	int fd1, fd2, fd3;
	int piped[2];
	
	if(argc != 4){
		printf("Errore nel numero di parametri\n");
		exit(1);
	}
	
	if((fd1 = open(argv[0], O_RDONLY)) < 0){
		printf("Errore nell'apertura del file\n");
		exit(2);
	}
	printf("File descriptor 1: %d\n", fd1);
	
	if((fd2 = open(argv[1], O_RDONLY)) < 0){
		printf("Errore nell'apertura del file\n");
		exit(3);
	}
	printf("File descriptor 2: %d\n", fd2);
	
	if((fd3 = open(argv[2], O_RDONLY)) < 0){
		printf("Errore nell'apertura del file\n");
		exit(4);
	}
	printf("File descriptor 3: %d\n", fd3);

	
	close(fd1);
	
	pipe(piped);

	printf("File descriptor piped[0]: %d\n", piped[0]);
	printf("File descriptor piped[1]: %d\n", piped[1]);



}


