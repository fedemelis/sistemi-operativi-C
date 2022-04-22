#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#define PERM 0644

int appendFile(char* f){

	int fd, nread;
	char buffer[BUFSIZ];
	
	if((fd = open(f, O_APPEND | O_RDWR | O_CREAT)) < 0){
		return 1;
	}
	
	while((nread = read(0, buffer, BUFSIZ)) > 0){
		write(fd, buffer, nread);
	}
	
	return 0;

}

int main(int argc, char **argv){
	
	if(argc != 2){
		printf("Devo inserire esattamente un parametro\n");
		exit(1);
	}
	
	int res = appendFile(argv[1]);
	
	if(res > 0){
		printf("Errore durante la scrittura in append\n");
		exit(2);	
	}
	
	exit(0);	
}


