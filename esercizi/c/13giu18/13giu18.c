#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>
#define PERM 0644

typedef int pipe_t[2];

typedef struct {
	int pid;
	long int lunghezza;
}linea;

int main(int argc, char** argv) {


	int fd, nread, pidfiglio, ritorno, status, pid, count, nr; //var
	pipe_t* piped; //pipe
	//VA ASSOLUTAMENTE CHIARITO COME SI USANO LE PIPE
	//SI SEGNALA CHE IL CODICE DI CHIUSURA E DI SCRITTURA RISULTA MOLTO MOLTO COMPLICATO
	//SE SI DECIDE DI SCRIVERE SULLA PIPE i+1 E LEGGERE DALLA PIPE i, ALMENO SI POTREBBE USARE L'OPERATORE %N IN MODO DA SEMPLIFICARE IL CODICE!!!
	char ch;
	int N;
	int Y;
	linea* ret;

	//controllo param	
	if (argc < 4) {
		printf("Errore nel numero di param\n");
		exit(1);
	}

	//controllo file param
	for (int i = 1; i < argc - 1; i++) {
		if ((fd = open(argv[i], O_RDONLY)) < 0) {
			printf("Devo passare nomi di file\n");
			exit(2);
		}
	}

	if (atoi(argv[argc - 1]) <= 0) {
		printf("Devo passare un numero positivo\n");
		exit(2);
	}

	Y = atoi(argv[argc - 1]);
	N = argc - 2;

	/* pipe */
	piped = (pipe_t*)malloc(sizeof(pipe_t) * N);
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(piped[i]) < 0) {
			printf("Errore in pipe\n");
			exit(4);
		}
	}

	/*fine pipe*/


	//creo i figli
	for (int i = 0; i < N; i++) {
		//errore fork
		if ((pid = fork()) < 0) {
			printf("Errore in fork\n");
			exit(-1);
		}
		//figlio
		if (pid == 0) {
			//chiudo pipe
			for (int p = 0; p < N; p++) {
				if (i != N - 1) {
					if ((i + 1) != p) {
						close(piped[p][1]);
					}
					if (i == 0 || i != p) {
						close(piped[p][0]);
					}
				}
				else {
					if (i != p) {
						close(piped[p][0]);
					}
					if (p != 0) {
						close(piped[p][1]);
					}
				}
			}
			if ((fd = open(argv[i + 1], O_RDONLY)) < 0) {
				printf("Errore nell'apertura del file\n");
				exit(-1);
			}

			//BASTA FARLO UNA VOLTA PER TUTTI QUINDI UGUALE PER OGNI FIGLIO E NON PER OGNI LINEA!!!
			ret = (linea*)malloc(sizeof(linea) * (i + 1));
			//ANDREBBE CONTROLLATO SE LA MALLOC NON FALLISCE

			count = 0;
			//leggo finchè posso
			while ((nread = read(fd, &ch, 1)) > 0) {
				count += 1;
				//sono a fine linea
				if (ch == '\n') {
					//printf("Finelina\n");
					if (i == 0) {
						//ret = (linea*)malloc(sizeof(linea) * (i + 1));
						ret[i].pid = getpid();
						//printf("%d", ret[i].pid);
						ret[i].lunghezza = count;
						//SBAGLIATA DIMENSIONE write(piped[i + 1][1], ret, sizeof(ret) * (i + 1));
						write(piped[i + 1][1], ret, sizeof(linea) * (i + 1));
					}
					else {
						//preparo la struct
						//ret = (linea*)malloc(sizeof(linea) * (i + 1));
						//SBAGLIATA DIMENSIONE nr = read(piped[i][0], ret, sizeof(ret) * (i));
						nr = read(piped[i][0], ret, sizeof(linea) * (i));
						//IDEM if (nr != (sizeof(ret) * (i))) {
						if (nr != (sizeof(linea) * (i))) {
							//printf("nr=%d size=%ld", nr, sizeof(ret) * (i));
							printf("Errore nella lettura della struct\n");
							exit(-1);
						}
						//printf("Corretto\n");
						ret[i].pid = getpid();
						//printf("%d", ret[i].pid);
						ret[i].lunghezza = count;
						if (i == N - 1) {
							//printf("Ultimo figlio: %d", i);
							//IDEM write(piped[0][1], ret, sizeof(ret) * (i + 1));
							write(piped[0][1], ret, sizeof(linea) * (i + 1));
						}
						else {
							//write(piped[i + 1][1], ret, sizeof(ret) * (i + 1));
							write(piped[i + 1][1], ret, sizeof(linea) * (i + 1));
						}
					}
					count = 0;
				}
			}
			exit(i);

		}
	}
	//chiudo pipe
	for (int p = 0; p < N; p++) {
		close(piped[p][1]);
		if (p != 0) {
			close(piped[p][0]);
		}
	}

	ret = (linea*)malloc(sizeof(linea) * N);

	//SBAGLIATO RANGE for (int i = 0; i < N; i++) {
	for (int i = 0; i < Y; i++) {
		//SBAGLIATA DIMENSIONE nr = read(piped[0][0], ret, sizeof(ret) * N);
		nr = read(piped[0][0], ret, sizeof(linea) * N);
		//IDEM if (nr != (sizeof(ret) * N)) {
		if (nr != (sizeof(linea) * N)) {
			//printf("nr=%d size=%ld", nr, sizeof(ret) * (N));
			printf("Errore in lettura della linea\n");
			exit(8);
		}
		//SBAGLIATO RANDE  for (int l = 0; l < Y; l++) 
		for (int l = 0; l < N; l++) {
			printf("Il figlio con PID %d trova lunghezza=%ld nella linea %d del suo file\n", ret[l].pid, ret[l].lunghezza, l + 1);
		}
	}



	//aspetto i figli (standard x N figli)
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < N; i++) {
		if ((pidfiglio = wait(&status)) < 0) {
			printf("Errore nella wait\n");
			exit(6);
		}

		if ((status & 0xFF) != 0) {
			printf("Errore nella chiusura del figlio con PID %d\n", pidfiglio);
			exit(7);
		}
		else {
			ritorno = (int)status >> 8;
			ritorno &= 0xFF;
			printf("Il filgio con PID %d ha restituito: %d\n", pidfiglio, ritorno);
		}
	}
	printf("DEBUG: Finito\n");
	exit(0);

}
