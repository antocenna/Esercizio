#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include "header.h"


int main(){
	//ottenimento e attach shared memory
	printf("{CLIENT %d} Sono il client %d, e sono stato creato con successo\n", getpid(), getpid());;
	key_t key_shm;
	key_shm = ftok(".", 'a');

	printf("{CLIENT %d} Acquisizione shared memory\n", getpid());
	int ds_shm = shmget(key_shm, sizeof(Coda), IPC_CREAT | 0644);
	if(ds_shm < 0){
		perror("Errore acquisizione shared memory CLIENT");
		exit(1);
	}


	printf("{CLIENT %d} Attach shared memory\n", getpid());
	struct Coda * c;
	c = shmat(ds_shm, NULL, 0);
	if(c == (void *)-1){
		perror("{CLIENT} Errore attach shared memory CLIENT");
		exit(1);
	}


	//ottenimento semafori

	key_t key_sem;
	key_sem = ftok(".", 'b');

	printf("{CLIENT %d} Acquisizione semafori\n", getpid());
	int ds_sem = semget(key_sem, 4, IPC_CREAT | 0644); //Perche IPC_CREAT?
	if(ds_sem < 0){
		perror("{CLIENT} Errore acquisizione semafori");
		exit(1);
	}
    
	printf("{CLIENT %d} Invio richieste...\n", getpid());

	int i = 0;
	int num_prenotazioni;	


	while(i < NUM_REQ){
		num_prenotazioni = (rand() % 5) + 1;//TODO numero casuale

		//TODO invio richiesta tramite funzione "produci"
		printf("[CLIENT %d] - Invio richiesta (con num_prenotazioni = %d) al server...\n",getpid(), num_prenotazioni);
		produci(c, ds_sem, num_prenotazioni, getpid());

		sleep(1);
		i++;
	}
	//TODO invio richiesta di terminazione (-1)
	produci(c,ds_sem,-1, getpid());
	printf("[CLIENT %d] - Fine processo\n",getpid());

	return 0;
}
