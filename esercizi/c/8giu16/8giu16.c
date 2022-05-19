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

//genera numero random
int mia_random(int n) {
	int casuale;
	casuale = rand() % n;
	return casuale;
}

int main(int argc, char** argv) {

	//seme
	srand(time(NULL));

	//var
	int fd, H, fdCreat, N, pid, quantiLetti, nRead, count, rnd, index, fdTMP;
	char ch;
	pipe_t* pipedFTOP;
	pipe_t* pipedPTOF;
	int pidfiglio, status, ritorno;

	quantiLetti = 0;

	//controllo param	
	if (argc < 6) {
		printf("Errore nel numero di param\n");
		exit(1);
	}

	//controllo file param
	for (int i = 1; i < argc - 1; i++) {
		if ((fd = open(argv[i], O_RDONLY)) < 0) {
			printf("Devo passare N nomi di file\n");
			exit(2);
		}
	}

	//controllo intero
	if (atoi(argv[argc - 1]) > 0 && atoi(argv[argc - 1]) < 255) {
		H = atoi(argv[argc - 1]);
	}
	else {
		printf("L'ultimo param deve essere un numero\n");
		exit(3);
	}

	N = argc - 2;

	/* pipe */
	//pipe figlio to padre
	pipedFTOP = (pipe_t*)malloc(sizeof(pipe_t) * N);

	if (pipedFTOP == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(6);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(pipedFTOP[i]) < 0) {
			printf("Errore in pipe\n");
			exit(7);
		}
	}

	//pipe padre to figlio
	pipedPTOF = (pipe_t*)malloc(sizeof(pipe_t) * N);

	if (pipedPTOF == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(6);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(pipedPTOF[i]) < 0) {
			printf("Errore in pipe\n");
			exit(7);
		}
	}

	/* fine pipe*/

	//creo file
	if ((fdTMP = creat("/tmp/creato", PERM)) < 0) {
		printf("Errore durante la creazione del file\n");
		exit(4);
	}

	if ((fdCreat = open("/tmp/creato", O_WRONLY)) < 0) {
		printf("Errore nell'apertura del file\n");
		exit(2);
	}


	//creo i figli
	for (int i = 0; i < N; i++) {

		//errore fork
		if ((pid = fork()) < 0) {
			printf("Errore in fork\n");
			exit(5);
		}

		//cod figlio
		if (pid == 0) {

			//chiudo pipe 
			for (int j = 0; j < N; j++) {
				close(pipedFTOP[j][0]);
				close(pipedPTOF[j][1]);
				if (i != j) {
					close(pipedFTOP[j][1]);
					close(pipedPTOF[j][0]);
				}
			}

			if ((fd = open(argv[i + 1], O_RDONLY)) < 0) {
				printf("Errore nell'apertura del file\n");
				exit(-1);
			}

			for (int k = 0; k < H; k++) {
				//leggo fino alla fine della linea
				nRead = read(fd, &ch, 1);
				while (nRead > 0 && ch != '\n') {
					quantiLetti += nRead;
					nRead = read(fd, &ch, 1);
				}
				//scrivo su pipe
				quantiLetti += 1;
				write(pipedFTOP[i][1], &quantiLetti, sizeof(int));
				if ((read(pipedPTOF[i][0], &index, sizeof(index))) > 0) {

					if (index <= quantiLetti) {
						//printf("IDONEO max: %d index: %d file: %s\n", quantiLetti, index, argv[i+1]);
						//mi sposto indietro all'inizio della linea
						for (int t = 0; t < quantiLetti; t++) {
							lseek(fd, -1L, SEEK_CUR);
						}
						//leggo fino all'indice desiderato
						for (int t = 0; t < index; t++) {
							read(fd, &ch, 1);
						}
						printf("Scrivo sul file %c all'indice %d\n", ch, index);
						write(fdCreat, &ch, 1);
						//mi riporto alla posizione precedente
						for (int t = 0; t < quantiLetti - index; t++) {
							read(fd, &ch, 1);
						}
					}
					else {
						//printf("NON IDONEO max: %d index: %d\n", quantiLetti, index);
					}
				}
				quantiLetti = 0;
				nRead = 0;
			}
			exit(0);
		}

	}
	//chiudo pipe
	for (int i = 0; i < N; i++) {
		close(pipedFTOP[i][1]);
		close(pipedPTOF[i][0]);
	}
	for (int j = 0; j < H; j++) {
		rnd = mia_random(N);
		//printf("Scelgo il file numero: %d\n", rnd);
		for (int i = 0; i < N; i++) {
			read(pipedFTOP[i][0], &count, sizeof(count));
			if (i == rnd) {
				rnd = mia_random(count - 1);
			}
		}
		for (int k = 0; k < N; k++) {
			//scrivo l'index a tutti i figli
			write(pipedPTOF[k][1], &rnd, sizeof(rnd));
		}
	}



	//aspetto i figli
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < N; i++) {
		if ((pidfiglio = wait(&status)) < 0) {
			printf("Errore nella wait\n");
			exit(2);
		}

		if ((status & 0xFF) != 0) {
			printf("Errore nella chiusura del figlio\n");
			exit(3);
		}
		else {
			ritorno = (int)status >> 8;
			ritorno &= 0xFF;
			printf("Il filgio con PID %d ha restituito: %d\n", pidfiglio, ritorno);
		}
	}
	printf("DEBUG: Finito\n");
	exit(0);

}#include <stdlib.h>
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

//genera numero random
int mia_random(int n) {
	int casuale;
	casuale = rand() % n;
	return casuale;
}

int main(int argc, char** argv) {

	//seme
	srand(time(NULL));

	//var
	int fd, H, fdCreat, N, pid, quantiLetti, nRead, count, rnd, index, fdTMP;
	char ch;
	pipe_t* pipedFTOP;
	pipe_t* pipedPTOF;
	int pidfiglio, status, ritorno;

	quantiLetti = 0;

	//controllo param	
	if (argc < 6) {
		printf("Errore nel numero di param\n");
		exit(1);
	}

	//controllo file param
	for (int i = 1; i < argc - 1; i++) {
		if ((fd = open(argv[i], O_RDONLY)) < 0) {
			printf("Devo passare N nomi di file\n");
			exit(2);
		}
	}

	//controllo intero
	if (atoi(argv[argc - 1]) > 0 && atoi(argv[argc - 1]) < 255) {
		H = atoi(argv[argc - 1]);
	}
	else {
		printf("L'ultimo param deve essere un numero\n");
		exit(3);
	}

	N = argc - 2;

	/* pipe */
	//pipe figlio to padre
	pipedFTOP = (pipe_t*)malloc(sizeof(pipe_t) * N);

	if (pipedFTOP == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(6);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(pipedFTOP[i]) < 0) {
			printf("Errore in pipe\n");
			exit(7);
		}
	}

	//pipe padre to figlio
	pipedPTOF = (pipe_t*)malloc(sizeof(pipe_t) * N);

	if (pipedPTOF == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(6);
	}

	for (int i = 0; i < N; i++) {
		if (pipe(pipedPTOF[i]) < 0) {
			printf("Errore in pipe\n");
			exit(7);
		}
	}

	/* fine pipe*/

	//creo file
	if ((fdTMP = creat("/tmp/creato", PERM)) < 0) {
		printf("Errore durante la creazione del file\n");
		exit(4);
	}

	if ((fdCreat = open("/tmp/creato", O_WRONLY)) < 0) {
		printf("Errore nell'apertura del file\n");
		exit(2);
	}


	//creo i figli
	for (int i = 0; i < N; i++) {

		//errore fork
		if ((pid = fork()) < 0) {
			printf("Errore in fork\n");
			exit(5);
		}

		//cod figlio
		if (pid == 0) {

			//chiudo pipe 
			for (int j = 0; j < N; j++) {
				close(pipedFTOP[j][0]);
				close(pipedPTOF[j][1]);
				if (i != j) {
					close(pipedFTOP[j][1]);
					close(pipedPTOF[j][0]);
				}
			}

			if ((fd = open(argv[i + 1], O_RDONLY)) < 0) {
				printf("Errore nell'apertura del file\n");
				exit(-1);
			}

			for (int k = 0; k < H; k++) {
				//leggo fino alla fine della linea
				nRead = read(fd, &ch, 1);
				while (nRead > 0 && ch != '\n') {
					quantiLetti += nRead;
					nRead = read(fd, &ch, 1);
				}
				//scrivo su pipe
				quantiLetti += 1;
				write(pipedFTOP[i][1], &quantiLetti, sizeof(int));
				if ((read(pipedPTOF[i][0], &index, sizeof(index))) > 0) {

					if (index <= quantiLetti) {
						//printf("IDONEO max: %d index: %d file: %s\n", quantiLetti, index, argv[i+1]);
						//mi sposto indietro all'inizio della linea
						for (int t = 0; t < quantiLetti; t++) {
							lseek(fd, -1L, SEEK_CUR);
						}
						//leggo fino all'indice desiderato
						for (int t = 0; t < index; t++) {
							read(fd, &ch, 1);
						}
						printf("Scrivo sul file %c all'indice %d\n", ch, index);
						write(fdCreat, &ch, 1);
						//mi riporto alla posizione precedente
						for (int t = 0; t < quantiLetti - index; t++) {
							read(fd, &ch, 1);
						}
					}
					else {
						//printf("NON IDONEO max: %d index: %d\n", quantiLetti, index);
					}
				}
				quantiLetti = 0;
				nRead = 0;
			}
			exit(0);
		}

	}
	//chiudo pipe
	for (int i = 0; i < N; i++) {
		close(pipedFTOP[i][1]);
		close(pipedPTOF[i][0]);
	}
	for (int j = 0; j < H; j++) {
		rnd = mia_random(N);
		//printf("Scelgo il file numero: %d\n", rnd);
		for (int i = 0; i < N; i++) {
			read(pipedFTOP[i][0], &count, sizeof(count));
			if (i == rnd) {
				rnd = mia_random(count - 1);
			}
		}
		for (int k = 0; k < N; k++) {
			//scrivo l'index a tutti i figli
			write(pipedPTOF[k][1], &rnd, sizeof(rnd));
		}
	}



	//aspetto i figli
	printf("ASPETTO I FIGLI\n");
	for (int i = 0; i < N; i++) {
		if ((pidfiglio = wait(&status)) < 0) {
			printf("Errore nella wait\n");
			exit(2);
		}

		if ((status & 0xFF) != 0) {
			printf("Errore nella chiusura del figlio\n");
			exit(3);
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