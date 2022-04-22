#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>

int main(int argc, char **argv){
	
	int fd = 0;
	int nread;
	char buffer[BUFSIZ];
	
	if(argc == 1){
		while((nread = read(fd, buffer, BUFSIZ)) > 0){
			write(1, buffer, nread);
		}
		
	}
	else{
		for(int i = 1; i < argc; i++){
			if((fd = open(argv[i], O_RDONLY)) < 0){
				printf("errore durantre l'apertura del file %s\n", argv[i]);
				exit(2);
			}
			while((nread = read(fd, buffer, BUFSIZ)) > 0){
				write(1, buffer, nread);
			}
		
		}
	}
	exit(0);	
}


