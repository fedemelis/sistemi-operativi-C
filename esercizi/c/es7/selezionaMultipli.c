#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#define PERM 0644


int main(int argc, char **argv){
	
	int fd, nread;
	char buffer[BUFSIZ];
	char c[BUFSIZ];
	
	
	if(argc != 3){
		printf("Devo inserire esattamente due parametri\n");
		exit(1);
	}
	
	if((fd = open(argv[1], O_RDWR)) < 0){
		printf("Il primo parametro deve essere un file\n");
		exit(2);
	}
	
	if((atoi(argv[2])) <= 0){
		printf("Il primo parametro deve essere un numero stretamente positivo\n");
		exit(3);
	}
	
	while((nread = read(fd, buffer, BUFSIZ)) > 0){
		strcat(c, buffer);
	}
	
	for(int i = 0; i < strlen(c); i++){
		if((i % atoi(argv[2])) == 0){
			printf("Questo è il %d-esimo carattere del file passato: %c\n", i, c[i]);
		}
	}
	
	exit(0);	
}


