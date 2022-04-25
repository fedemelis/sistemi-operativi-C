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
	int fd;
	int counter = 0;
	char c[BUFSIZ];
	bool scritto = false;
	
	if(argc != 3){
		printf("Usage is filepath ; numero\n");
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
		counter += 1;
		strcat(c, &buffer);
		if(buffer == '\n'){
			if(counter == n){
			scritto = true;
			write(1, &c, strlen(c));
			}
			counter = 0;
			memset(c, 0, strlen(c));
		}
	}
	
	if(!scritto){
		printf("Non esistono linee che rispettino la condizione\n");
	}
	
	exit(0);
}


