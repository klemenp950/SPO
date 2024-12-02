#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define M_PI 3.14159265358979323846

// gcc naloga.c -o naloga -lm -lpthread

int P_global = 0;
int N_global = 0;
pthread_t *thread;
int seed;
int N_threads;
pthread_mutex_t mutexP;
pthread_mutex_t mutexN;

struct timespec myTime;

void *thread_function(void *args);

int main(int argc, char *argv[]){
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <number_of_threads>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    pthread_mutex_init(&mutexP, NULL);
    pthread_mutex_init(&mutexN, NULL);
    N_threads = atoi(argv[1]);
    clock_gettime(CLOCK_REALTIME, &myTime);
    thread = malloc(sizeof(pthread_t) * N_threads);

    for (int i = 0; i < N_threads; i++)
    {
        pthread_create(&thread[i], NULL, thread_function, NULL);
    }

    for (int i = 0; i < N_threads; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&mutexP);
    pthread_mutex_destroy(&mutexN);

    printf("P = %d\n", P_global);
    printf("N = %d\n", N_global);
    printf("Area = %f\n", ((double)P_global / N_global) * M_PI);
    
    

    return 0;
}

void *thread_function(void *args){
    double random_number;
    double x;
    double y;
    for (int i = 0; i < 1e6; i++)
    {
        random_number =((double)rand_r(&seed)) / RAND_MAX; 
        y = ((double)rand_r(&seed)) / RAND_MAX;
        x = random_number * M_PI;
        if(y < sin(x)){
            pthread_mutex_lock(&mutexP);
            P_global++;
            pthread_mutex_unlock(&mutexP);
        }
        pthread_mutex_lock(&mutexN);
        if(N_global >= 1e9){
            pthread_mutex_unlock(&mutexN);
            return NULL;
        } else {
            N_global++;
        }
        pthread_mutex_unlock(&mutexN);

        
    }
    
}

