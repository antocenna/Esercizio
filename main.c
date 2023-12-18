#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "header.h"

#define NUM_CLIENT 3
#define NUM_SERVER 1

int main(){
	//ottenimento e attach shared memory
	key_t key_shm;
	key_shm = ftok(".", 'a');

	printf("{MAIN} Creazione shared memory\n");
	int ds_shm = shmget(key_shm, sizeof(Coda), IPC_CREAT | 0644);
	if(ds_shm < 0){
		perror("{MAIN} Errore inizializzazione shared memory");
		exit(1);
	}

	printf("{MAIN} Attach shared memory\n");
	struct Coda * c;
	c = shmat(ds_shm, NULL, 0);
	c->testa = 0;
	c->coda = 0;
	if(c == (void *)-1){
		perror("{MAIN} Errore shmat coda");
		exit(1);
	}
	
	//ottenimento array semafori

	key_t key_sem;
	key_sem = ftok(".", 'b');

	printf("{MAIN} Creazione semafori\n");
	int ds_sem = semget(key_sem, 4, IPC_CREAT | 0644);
	if(ds_sem < 0){
		perror("{MAIN} Errore inizializzazione semafori");
		exit(1);
	}
	
	//TODO inizializzazione coda e semafori
	printf("{MAIN} Inizializzazione semafori\n");
	int result = semctl(ds_sem, SPAZIO_DISP, SETVAL, DIM_BUFFER);
	if(result < 0){
		perror("{MAIN} Errore inizializzazione semaforo spazio disponibile");
		exit(1);
	}

	result = semctl(ds_sem, MSG_DISP, SETVAL, 0);
	if(result < 0){
		perror("{MAIN} Errore inizializzazione semaforo messaggio disponibile");
		exit(1);
	}

	result = semctl(ds_sem, MUTEXC, SETVAL, 1);
	if(result < 0){
		perror("{MAIN} Errore inizializzazione semaforo mutex consumatori");
		exit(1);
	}

	result = semctl(ds_sem, MUTEXP, SETVAL, 1);
	if(result < 0){
		perror("{MAIN} Errore inizializzazione semaforo mutex produttori");
		exit(1);
	}


	int i = 0;
	pid_t pid;
	int status;
	
	for(int i = 0; i < NUM_CLIENT; i++){
		
		pid = fork();

		if(pid > 0){
			//Padre
		}

		else if(pid == 0){
			//Figlio - Client
			printf("{MAIN} Creazione figlio client %d\n", i);
			execl("./client", "client", NULL);

			perror("Errore execl client");
			exit(1);
		}

		else{
			//Errore
			perror("Errore creazione fork client");
			exit(1);
		}
	}


	pid = fork();

	if(pid > 0){
		//Padre
	}

	else if(pid == 0){
		//Figlio - Client
		printf("{MAIN} Creazione figlio server\n");
		execl("./server", "server", NULL);

		perror("Errore execl server");
		exit(1);
	}

	else{
		//Errore
		perror("Errore creazione fork server");
		exit(1);
	}



	for(i = 0; i < NUM_CLIENT+NUM_SERVER;i++){

		wait(&status);

		if (WIFEXITED(status)) {
    			printf("{MAIN} [PADRE] - Figlio terminato con stato %d\n",status);
  		}
	}

	//TODO rimozione array semafori e memoria condivisa
	result = shmctl(ds_shm, IPC_RMID, NULL);
	if(result < 0){
		perror("{MAIN} Errore rimozione shared memory");
		exit(1);
	}

	result = semctl(ds_sem, 0, IPC_RMID);
	if(result < 0){
		perror("{MAIN} Errore rimozione semafori");
		exit(1);
	}

	printf("{MAIN} [PADRE] - Fine elaborazione...\n");

	return 0;	
}
