#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#define PERM 0644


int main(int argc, char **argv){
	
	char buffer;
	int i = 1;
	int fd;
	bool scritto = false;
	
	if(argc != 3){
		printf("Usage is:filepath : numero\n");
		exit(1);
	}
	

	if((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Il parametro deve essere un file leggibile\n");
	}
	
	int n = atoi(argv[2]);
	if(n <= 0){
		printf("Il secondo parametro deve essere un numero");
		exit(3);
	}
	
	while((read(fd, &buffer, 1)) > 0){
		if(i == n){
			scritto = true;
			write(1, &buffer, 1);
		}
		if(buffer == '\n'){
			i+=1;
		}
		if(i == 10){
			exit(0);
		}
	}
	
	if(!scritto){
		printf("La linea richiesa non esiste\n");
	}
	exit(0);
	
	
}


