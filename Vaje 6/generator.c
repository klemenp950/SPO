#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<unistd.h>

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

void lock(int semid);
void unlock(int semid);

int main(){
    union semun arg;
    key_t key_shm;
    key_t key_sem;
	int shmid;
	char *addr;
    int semid;

    if((key_shm = ftok("/usr", 'a')) < 0){
        fprintf(stderr, "Error: ftok 1\n");
        exit(1);
    }

    if((key_sem = ftok("/usr", 'b')) < 0){
        fprintf(stderr, "Error: ftok 2\n");
        exit(1);
    }

    if((semid = semget(key_sem, 1, 0644 | IPC_CREAT)) < 0){
        perror("shmget");
    }

    arg.val = 1; //1 procees naenkrat lahko zaklene semafor. 
	
    if ( (semctl( semid, 0, SETVAL, arg)) < 0)
		perror("semctl");

    if ( (shmid = shmget( key_shm, 2048, 0644 | IPC_CREAT)) < 0){
		perror("semget");
    }

    addr = shmat( shmid, 0 /*jedro izbere naslov*/, 0);
    if ( addr == (void *)-1 ){
		perror("shmat");
    }

    // printf("Prisel do zanke!\n");

    for (int i = 0; i < 4; i++)
    {   
        sleep(2);
        lock(semid);
        sprintf(addr, "ID %d: This is a message", (i+1));
        printf("ID %d: This is a message\n", (i+1));
        sleep(2);
        unlock(semid);
    }
    
    strcpy(addr, "\0");
    
    return 0;
}


void lock(int semid){
    // printf("Locking\n");
    struct sembuf operacije[1];
    operacije[0].sem_num = 0;
    operacije[0].sem_op = -1;
    operacije[0].sem_flg = 0;
    if ( semop( semid, operacije, 1) < 0){
        perror("semop");
        exit(1);
    }
}

void unlock(int semid){
    // printf("Unlocking\n");
    struct sembuf operacije[1];
    operacije[0].sem_num = 0;
    operacije[0].sem_op = 1;
    operacije[0].sem_flg = 0;
    if ( semop( semid, operacije, 1) < 0){
        perror("semop");
        exit(1);
    }
}