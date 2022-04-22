#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char **argv){

	int fd;

	if(argc != 4){
		printf("Devo inserire 3 parametri\n");
		exit(1);	
	}
	
	if((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Il primo parametro deve essere il nome di un file\n");
		exit(2);
	}
	
	if(atoi(argv[2]) <= 0){
		printf("Il secondo parametro deve essere un numero strattamente positivo\n");
		exit(3);
	}
	
	if(argv[3][1] != 0){
		printf("Il terzo parametro deve essere un singolo carattere\n");
		exit(4);
	}
	
	
	
	printf("L'eseguibile %s è stato invocato con %d parametri\n", argv[0], argc-1);
	
	printf("Nome file: %s\n", argv[1]);
	printf("Numero: %s\n", argv[2]);
	printf("Carattrere: %s\n", argv[3]);
	
	exit(0);
}
