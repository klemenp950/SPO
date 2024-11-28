#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/msg.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void lock(int semid);
void unlock(int semid);
void send(int msqid, char *msg);

int main() {
    union semun arg;
    key_t key_shm;
    key_t key_sem;
    key_t key_msq;
    int shmid;
    char *addr;
    int semid;
    int msqid;

    // Create key for semaphore, shared memory and message queue
    if ((key_shm = ftok("/usr", 'a')) < 0) {
        fprintf(stderr, "Error: ftok 1\n");
        exit(1);
    }

    if ((key_sem = ftok("/usr", 'b')) < 0) {
        fprintf(stderr, "Error: ftok 2\n");
        exit(1);
    }

    if ((key_msq = ftok("/usr", 'c')) < 0) {
        fprintf(stderr, "Error: ftok 3\n");
        exit(1);
    }
     // Create semaphore, shared memory and message queue
    if ((semid = semget(key_sem, 1, 0644 | IPC_CREAT)) < 0) {
        perror("semget");
        exit(1);
    }

    arg.val = 1; // 1 process at a time can lock the semaphore
    if ((semctl(semid, 0, SETVAL, arg)) < 0) {
        perror("semctl");
        exit(1);
    }

    if ((shmid = shmget(key_shm, 2048, 0644 | IPC_CREAT)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((msqid = msgget(key_msq, 0644 | IPC_CREAT)) < 0) {
        perror("msgget");
        exit(1);
    }


    addr = (char *) shmat(shmid, 0 /* kernel chooses address */, 0);
    if(addr == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    char prev[2048] = "";
    while (1) {
        lock(semid);
        if (addr[0] == '\0') { 
            unlock(semid);
            if((msgsnd(msqid, addr, strlen(addr) + 1, 0)) < 0) {
                perror("msgsnd");
            }
            break; // Exit loop
        }
        if (strcmp(prev, addr) != 0) { 
            printf("%s\n", addr);
            strcpy(prev, addr);
        }
        if((msgsnd(msqid, addr, strlen(addr) + 1, 0)) < 0) {
                perror("msgsnd");
            }
        unlock(semid);
        sleep(1);
    }

    send(msqid, addr);

    // Delete semaphore, shared memory
    semctl(semid, 0, IPC_RMID);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

void lock(int semid) {
    struct sembuf operacije[1];
    operacije[0].sem_num = 0;
    operacije[0].sem_op = -1;
    operacije[0].sem_flg = 0;
    if (semop(semid, operacije, 1) < 0) {
        perror("semop lock");
    }
}

void unlock(int semid) {
    struct sembuf operacije[1];
    operacije[0].sem_num = 0;
    operacije[0].sem_op = 1;
    operacije[0].sem_flg = 0;
    if (semop(semid, operacije, 1) < 0) {
        perror("semop unlock");
    }
}

void send(int msqid, char *msg) {
    if (msgsnd(msqid, msg, strlen(msg) + 1, 0) < 0) {
        perror("msgsnd");
    }
}