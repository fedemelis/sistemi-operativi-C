#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>

int main(int argc, char **argv){

	int fd;
	char buf;
	
	if(argc != 4){
		printf("Devo inserire esattamente tre parametri\n");
		exit(1);
	}
	
	if((fd = open(argv[1], O_RDWR)) < 0){
		printf("Il primo parametro deve essere un file, leggibile e scrivibile\n");
		exit(2);
	}
	
	if(argv[2][1] != 0){
		printf("Il secondo parametro deve essere un singolo carattere\n");
		exit(3);
	}
	
	if(argv[3][1] != 0){
		printf("Il terzo parametro deve essere un singolo carattere\n");
		exit(4);
	}
	
	while(read(fd, &buf, 1)){
		if(buf == argv[2][0]){
			lseek(fd, -1L, SEEK_CUR);
			write(fd, argv[3], 1);
			printf("%s", argv[3]);
		}
	}
	
	exit(0);
}


