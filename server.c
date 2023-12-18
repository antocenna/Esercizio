#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <pthread.h>
#include "header.h"

#define NUM_WORKER 3

static struct Coda * coda;
static int ds_sem;
static int end = 0;

// struttura per passaggio parametri ai thread
typedef struct parametri{
	int id;
	Count* count;
} parametri;

void init_buffer(Count* c){
	//TODO aggiungere codice per inizializzare Count
	pthread_mutex_init(&(c->mutex), NULL);
	c->num_prenotazioni = 0;
	printf("{SERVER} Count inizializzato!\n");
}

void remove_buffer(Count* c){
	//TODO aggiungere codice per rimuovere Count
	pthread_mutex_destroy(&(c->mutex));
	printf("{SERVER} Count distrutto\n");
}


void *worker(void* arg){

	printf("[WORKER] Start del thread...\n");

	parametri* par = arg;
	Count *c = par->count;
	int consumazione = 0;
	//TODO: completare il passaggio dei parametri
	
	while(!end) {
		//TODO completare il codice del worker
		// worker preleva dalla coda con funzione "consuma"
		// aggiornamento (eventuale) variabile end se valore ricevuto = -1
		// aggiornamento Count in mutua esclusione	
		printf("[WORKER %d] In attesa di richiesta con id %d\n", par->id, par->id);
		consumazione = consuma(coda, ds_sem, par->id);
		printf("[WORKER %d] Ho ricevuto la richiesta di %d prenotazioni \n", par->id, consumazione);
		

		if(consumazione == -1){
			end = 1;
			break;
		}

		pthread_mutex_lock(&(c->mutex));
		c->num_prenotazioni = c->num_prenotazioni + consumazione;
		printf("[WORKER %d] Numero prenotazioni totali aggiornato a %d prenotazioni \n",par->id, c->num_prenotazioni);
		pthread_mutex_unlock(&(c->mutex));
	
	}
    //TODO uscita dal thread
	pthread_exit(0);
}



int main(){

	int i,k;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

	//ottenimento e attach shared memory
	key_t key_shm;
	key_shm = ftok(".", 'a');

	printf("{SERVER} Acquisizione shared memory\n");
	int ds_shm = shmget(key_shm, sizeof(Coda), IPC_CREAT | 0644);
	if(ds_shm < 0){
		perror("{SERVER} Errore acquisizione shared memory client");
		exit(1);
	}


	printf("{SERVER} attach shared memory\n");
	Coda * c;
	c = shmat(ds_shm, NULL, 0);
	if(c == (void *)-1){
		perror("{SERVER} Errore attach shared memory client");
		exit(1);
	}


	//ottenimento semafori

	key_t key_sem;
	key_sem = ftok(".", 'b');

	printf("{SERVER} Acquisizione semafori\n");
	ds_sem = semget(key_sem, 4, IPC_CREAT | 0644);
	if(ds_sem < 0){
		perror("{SERVER} Errore acquisizione semafori");
		exit(1);
	}

	//TODO Creazione e Inizializzazione struttura locale
	printf("{SERVER} Creazione e allocazione struttura locale ai worker, count...\n");
	Count* count = (Count *)malloc(sizeof(Count));
	init_buffer(count);
	

	printf("{SERVER} Creazione struttura locale ai worker, parametri...\n");
	parametri *param[NUM_WORKER];
	pthread_t id_worker[NUM_WORKER];
	int result;


	//Avvio dei thread
	for(i = 0; i < NUM_WORKER;i++){
		printf("[SERVER] Creo thread %d...\n",i);
		//TODO creazione threads
		printf("{SERVER} Allocazione struttura locale ai worker, parametri...\n");
		param[i] = (parametri *)malloc(sizeof(parametri)); //Va allocata?
		param[i]->count = count;
		param[i]->id = i; 

		printf("{SERVER} Creazione thread worker!\n");
		result = pthread_create(&id_worker[i], NULL, worker, (void *)param[i]);
		if(result != 0){
			perror("{SERVER} Errore creazione thread worker");
			exit(1);
		}
	}
	
	for(k=0; k < NUM_WORKER; k++) {
	  //TODO join 
	  printf("{SERVER} Join thread\n");
	  pthread_join(id_worker[k], NULL);
	}
	
	//TODO rimozione struttura locale
	printf("[SERVER] - Deallocazione struttura\n");
	pthread_attr_destroy(&attr);
	pthread_exit(NULL);
}

