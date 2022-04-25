#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#define PERM 0644


int main(int argc, char **argv){
	
	char buffer;
	int i = 0;
	int fd;
	
	if(argc < 2 || argc > 3){
		printf("Usage is:[-n] filepath\n");
		exit(1);
	}
	
	if(argc == 2){
		if((fd = open(argv[1], O_RDONLY)) < 0){
			printf("Il parametro deve essere un file leggibile\n");
		}
		while((read(fd, &buffer, 1)) > 0){
			write(1, &buffer, 1);
			if(buffer == '\n'){
				i+=1;
			}
			if(i == 10){
				exit(0);
			}
		}
		exit(0);
	}
	else if(argv[1][0] != '-'){
		printf("Devo inserire esattamente l'opzione -n\n");
		exit(2);
	}
	
	int n = atoi(&argv[1][1]);
	if(n <= 0){
		printf("Errore nell'opzione");
		exit(3);
	}
	
	if((fd = open(argv[2], O_RDONLY)) < 0){
		printf("Il parametro deve essere un file leggibile\n");
	}
	
	i = 0;
	while((read(fd, &buffer, 1)) > 0){
		write(1, &buffer, 1);
		if(buffer == '\n'){
			i+=1;
		}
		if(i == n){
			exit(0);
		}
	}
	exit(0);	
}


