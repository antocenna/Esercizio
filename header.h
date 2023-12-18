#ifndef __HEADER
#define __HEADER

#include <pthread.h>

#define DIM_BUFFER 5
#define NUM_REQ 10


//TODO definizione eventuale di altre costanti
#define SPAZIO_DISP 0
#define MSG_DISP    1
#define MUTEXP      2
#define MUTEXC      3

typedef struct Coda {
    int buffer[DIM_BUFFER];
    int testa;
    int coda;
} Coda;

void produci(struct Coda *, int ds_sem, int num_prenotazioni, pid_t pid);
int consuma(struct Coda *, int ds_sem, int id_worker);

// struttura Count con num_prenotazioni condivisa lato server
typedef struct{
	//TODO aggiungere variabile per la sincronizzazione
	int num_prenotazioni;
    pthread_mutex_t mutex;
}Count;

#endif
