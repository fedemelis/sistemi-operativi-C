#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#define PERM 0644


int main(int argc, char **argv){
	
	char buffer;
	int nread = 1;
	
	if(argc != 2){
		printf("Devo inserire esattamente un'opzione\n");
		exit(1);
	}
	
	if(argv[1][0] != '-'){
		printf("Devo inserire esattamente il parametro -n\n");
		exit(2);
	}
	
	int i = 0;
	while(i < atoi(&argv[1][1])){
		while(buffer != '\n'){	
			nread = read(0, &buffer, 1);
			if(buffer == '\n'){
				write(1, &buffer, 1);
			}
			else if(nread > 0){
				write(1, &buffer, 1);
			}
		}
		i+=1;
		buffer = ' ';
	}
	
	exit(0);	
}


