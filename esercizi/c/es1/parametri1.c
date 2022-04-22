#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){

	if(argc < 2){
		printf("Devo inserire almeno un parametro\n");
		exit(1);	
	}
	
	printf("L'eseguibile %s è stato invocato con %d parametri\n", argv[0], argc-1);
	
	for(int i = 1; i < argc; i++){
		printf("%d) %s\n", i, argv[i]);
	}
	
	exit(0);
}
