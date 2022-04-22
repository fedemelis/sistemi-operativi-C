#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>

int main(int argc, char **argv){

	int fd, rdControl;
	int counter = 0;
	char buffer[1];

	if(argc != 3){
		printf("Devo inserire 2 parametri\n");
		exit(1);	
	}
	
	if((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Il primo parametro deve essere il nome di un file\n");
		exit(2);
	}
	
	if(argv[2][1] != 0){
		printf("Il secondo parametro deve essere un singolo carattere\n");
		exit(4);
	}
	
	
	printf("L'eseguibile %s è stato invocato con %d parametri\n", argv[0], argc-1);
	
	while((rdControl= read(fd, buffer, 1)) > 0){
		if(buffer[0] == argv[2][0]){
			counter+=1;
		}
	}
	printf("Ho trovato %d volte il carattere specificato\n", counter);
	exit(0);
}
