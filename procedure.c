#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "header.h"


int Wait_Sem (int id_sem, int numsem) {
       int err;
       struct sembuf sem_buf;

       sem_buf.sem_num=numsem;
       sem_buf.sem_flg=0;
       sem_buf.sem_op=-1;

       err = semop(id_sem,&sem_buf,1);   //semaforo rosso

       if(err<0) {
         perror("Errore WAIT");
       }

       return err;
}


int Signal_Sem (int id_sem, int numsem) {
       int err;
       struct sembuf sem_buf;

       sem_buf.sem_num=numsem;
       sem_buf.sem_flg=0;
       sem_buf.sem_op=1;

       err = semop(id_sem,&sem_buf,1);   //semaforo verde

       if(err<0) {
         perror("Errore SIGNAL");
       }

       return err;
}

void produci(struct Coda * c, int ds_sem, int num_prenotazioni, pid_t pid) {
    
    //TODO completare il codice del produttore
    printf("[CLIENT %d] Mi sospendo su SPAZIO_DISP\n", pid);
    Wait_Sem(ds_sem, SPAZIO_DISP);
    printf("[CLIENT %d] Mi riattivo da SPAZIO_DISP\n", pid);
    
    printf("[CLIENT %d] Mi sospendo su MUTEXP\n", pid);
    Wait_Sem(ds_sem, MUTEXP);
    printf("[CLIENT %d] Mi riattivo da MUTEXP\n", pid);

    c->buffer[c->testa] = num_prenotazioni;
    printf("Il valore prodotto è = %d\n", c->buffer[c->testa]);
    c->testa = (c->testa + 1) % DIM_BUFFER;


    printf("[CLIENT %d] Riattivazione processo in sospeso su MUTEXP\n", pid);
    Signal_Sem(ds_sem, MUTEXP);

    printf("[CLIENT %d] Riattivazione processo in sospeso su MSG_DISP\n", pid);
    Signal_Sem(ds_sem, MSG_DISP);

}

int consuma(struct Coda * c, int ds_sem, int id_worker) {
    int consumato;
   
   	//TODO completare il codice del consumatore
    printf("[WORKER %d] Mi sospendo su MSG_DISP\n", id_worker);
    Wait_Sem(ds_sem, MSG_DISP);
    printf("[WORKER %d] Mi riattivo da MSG_DISP\n", id_worker);

    printf("[WORKER %d] Mi sospendo su MUTEXC\n", id_worker);
    Wait_Sem(ds_sem, MUTEXC);
    printf("[WORKER %d] Mi riattivo da MUTEXC!\n", id_worker);


    consumato = c->buffer[c->coda];
    printf("Il valore consumato è = %d\n", consumato);
    c->coda = (c->coda + 1) % DIM_BUFFER;


    printf("[WORKER %d] Riattivazione worker in sospeso su MUTEXC\n", id_worker);
    Signal_Sem(ds_sem, MUTEXC);

    printf("[WORKER %d] Riattivazione processo in sospeso su SPAZIO_DISP\n", id_worker);
    Signal_Sem(ds_sem, SPAZIO_DISP);


    return consumato;
}

